#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "customtypes.h"
#include "sgmic.h"
#include "grid.h"

#ifndef MAX_RANDOM
#define MAX_RANDOM 18446744073709551616.0f
#endif

#ifndef MAX
#define MAX(a, b) (a > b ? a : b)
#endif

// File-scope global variables (These provide access to shared memory addresses from any thread).
static double **g_inputMatrix;
static double g_nAlpha;

static long g_totalResults;

static int g_max1DPartition;
static int g_numThreads;
static int g_totalCols;

static mine_result *g_results;
static rng_config *g_rng_cfgs;
static grid *g_currentGrids;
static config g_params;


double RandomChoice(rng_config *generator)
{
	return MarsagliaRNG(generator) / MAX_RANDOM;
}


double AceptanceProbability(double oldScore, double newScore, double temperature)
{
	return exp((newScore - oldScore) / temperature);
}


// Sgmic recieves index to pair and assigned thread id
double Sgmic(int pair, long t)
{
	// Declaration of needed variables
	int annealingNeighbors;
	int rows;
	int cols;
	int n;
	int i;

	double aceptanceProbability;
	double randomChoice;

	double currTemperature;
	double coolingFactor;
	double minTemp;
	double nAlpha;
	double best;
	double mic;

	double *varX;
	double *varY;

	// Initialize annealing variables
	annealingNeighbors = g_params.annealingNeighbors;
	coolingFactor = g_params.coolingFactor;
	minTemp = g_params.minTemp;

	// Initialize auxiliar variables
	varX = &*g_inputMatrix[g_results[pair].var1];
	varY = &*g_inputMatrix[g_results[pair].var2];
	nAlpha = g_nAlpha;
	n = g_totalCols;
	best = -1;
	mic = -1;

	// Initialize grid
	DiscoverMinMax(&g_currentGrids[t], n, varX, varY);

	// Explore viable grid resolutions
	for (cols = 2; cols < g_max1DPartition; cols++)
	{
		for (rows = 2; rows < g_max1DPartition; rows++)
		{
			// If the grid resolution is viable, compute MIC through Simulated Annealing
			if (rows * cols < nAlpha)
			{
				// Start from an equipartitioned state at full temperature
				SetResolution(&g_currentGrids[t], cols, rows);
				EquipartitionX(&g_currentGrids[t]);
				EquipartitionY(&g_currentGrids[t]);
				currTemperature = 1;

				// Compute the initial solution score
				ComputeFitness(&g_currentGrids[t], n, varX, varY);
				mic = g_currentGrids[t].fitness;
				best = mic;

				while (currTemperature > minTemp)
				{
					for (i = 0; i < annealingNeighbors; i++)
					{
						// Generate random neighboring solution (placement)
						SetRandomPlacements(&g_currentGrids[t], &g_rng_cfgs[t]);

						// Compute the new solution score
						ComputeFitness(&g_currentGrids[t], n, varX, varY);

						// If the new solution is better, keep it
						if (mic < g_currentGrids[t].fitness)
						{
							mic = g_currentGrids[t].fitness;
							best = mic;
						}

						// If the new solution is not better, maybe keep it
						else if (g_currentGrids[t].fitness < mic)
						{
							aceptanceProbability = AceptanceProbability(mic, g_currentGrids[t].fitness, currTemperature);
							randomChoice = RandomChoice(&g_rng_cfgs[t]);

							if (aceptanceProbability > randomChoice)
								mic = g_currentGrids[t].fitness;
						}
					}
					currTemperature = currTemperature * coolingFactor;
				}
			}
			else
				break;
		}
	}

	// Force to keep the best score
	if (mic < best)
		mic = best;

	return mic;
}


void *SgmicThread(void *t)
{
	// Set auxiliar variables to restrict the thread to a block of data
	long offset;
	long start;
	long end;
	int i;

	// Set auxiliar variables for the SGMIC computation
	bool useRapidMIC;
	double minRapidMIC;
	double maxRapidMIC;
	double mic;

	offset = g_totalResults / g_numThreads;
	start = ((long) t) * offset;
	end = start + offset;

	// If the current thread is the last thread, extend the computation to
	// the end of the results vector to avoid missing the computation of 
	// the last pairs.
	if ((long) t == g_numThreads - 1)
		end = g_totalResults;

	// Set values for useRapidMIC, minRapidMIC and maxRapidMIC
	useRapidMIC = g_params.useRapidMIC && g_params.performRapidMIC;
	minRapidMIC = g_params.minRapidMIC;
	maxRapidMIC = g_params.maxRapidMIC;

	// Iterate over the results array on a restricted block of results
	for (i = start; i < end; i++)
	{
		// Use RapidMIC as SGMIC approximation if needed
		if(useRapidMIC)
		{
			// If RapidMIC indicates data is too noisy or a good fit then avoid computing SGMIC
			if (g_results[i].rapidmic <= minRapidMIC || g_results[i].rapidmic >= maxRapidMIC)
				mic = g_results[i].rapidmic;
			else
				mic = MAX(g_results[i].rapidmic, Sgmic(i, (long) t));
		}

		// Compute SGMIC if needed
		else
			mic = Sgmic(i, (long) t);

		// Store resulting MIC metric to the corresponding result
		g_results[i].sgmic = mic;
	}
	pthread_exit(NULL);
}


bool PerformSGMIC(double **inputMatrix, int totalRows, int totalCols, mine_result *results, long totalResults, int numThreads, config params)
{
	// Define array of threads and support data structures
	pthread_t thread[numThreads];
	pthread_attr_t attr;
	long master_seed;
	void *status;
	long t;
	int rc;

	// Initialize file-scope global variables
	g_nAlpha = pow(totalCols, params.alpha);
	g_max1DPartition = GetMax1DPartition(g_nAlpha);
	g_totalResults = totalResults;
	g_inputMatrix = inputMatrix;
	g_numThreads = numThreads;
	g_totalCols = totalCols;
	g_results = results;
	g_params = params;

	// Initialize grids
	g_currentGrids = new grid[numThreads];
	for (t = 0; t < numThreads; t++)
		g_currentGrids[t] = CreateGrid(g_max1DPartition);

	// Initialize configs for Marsaglia RNGs
	g_rng_cfgs = new rng_config[numThreads];
	master_seed = time(NULL);

	for (t = 0; t < numThreads; t++)
	{
		g_rng_cfgs[t].x = master_seed + t * 1;
		g_rng_cfgs[t].y = master_seed + t * 2;
		g_rng_cfgs[t].z = master_seed + t * 3;
	}

	// Set JOINABLE attribute for the threads to be created
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Create the threads
	for (t = 0; t < numThreads; t++)
	{
		rc = pthread_create(&thread[t], &attr, SgmicThread, (void*)t);
		if (rc)
			printf("Error: Return code from pthread_create() is %d\n", rc);
	}

	// Join the threads
	for (t = 0; t < numThreads; t++)
	{
		rc = pthread_join(thread[t], &status);
		if (rc)
			printf("Error: Return code from pthread_join() is %d\n", rc);
	}

	// Deallocate grids
	for (t = 0; t < numThreads; t++)
		ClearPlacements(&g_currentGrids[t]);
	delete[] g_currentGrids;
	delete[] g_rng_cfgs;

	return true;
}
