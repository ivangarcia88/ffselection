#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "customtypes.h"
#include "approxmaxmi.h"
#include "parallelmic.h"
#include "grid.h"


// File-scope global variables (These provide access to shared memory addresses from any thread).
static double **g_inputMatrix;
static double g_nAlpha;

static long g_totalResults;

static int g_max1DPartition;
static int g_numThreads;
static int g_totalCols;

static mine_result *g_results;
static grid *g_currentGrids;
static config g_params;


// ParallelMIC recieves index to pair and assigned thread id
double MaxEquipartedGridScore(int pair, long t)
{
	// Declaration of needed variables
	int rows;
	int cols;
	int n;

	double nAlpha;
	double score;
	double mic;

	double *varX;
	double *varY;

	// Initialize auxiliar variables
	varX = &*g_inputMatrix[g_results[pair].var1];
	varY = &*g_inputMatrix[g_results[pair].var2];
	nAlpha = g_nAlpha;
	n = g_totalCols;
	score = -1;
	mic = -1;

	// Initialize grid
	DiscoverMinMax(&g_currentGrids[t], n, varX, varY);

	// Explore viable grid resolutions
	for (cols = 2; cols < g_max1DPartition; cols++)
	{
		for (rows = 2; rows < g_max1DPartition; rows++)
		{
			// If the grid resolution is viable, compute ParallelMIC
			if (rows * cols < nAlpha)
			{
				SetResolution(&g_currentGrids[t], cols, rows);
				EquipartitionX(&g_currentGrids[t]);
				EquipartitionY(&g_currentGrids[t]);

				// Compute the initial solution score
				ComputeFitness(&g_currentGrids[t], n, varX, varY);
				score = g_currentGrids[t].fitness;

				// Force to keep the best score as mic
				if (mic < score)
					mic = score;
			}
			else
				break;
		}
	}

	return mic;
}


void *ParallelMICThread(void *t)
{
	// Set auxiliar variables to restrict the thread to a block of data
	double *varX;
	double *varY;
	long offset;
	long start;
	long end;
	int i;

	// Set auxiliar variables for the ParallelMIC computation
	bool usePearson;
	double minPearson;
	double maxPearson;
	double mic;
	
	offset = g_totalResults / g_numThreads;
	start = ((long) t) * offset;
	end = start + offset;

	// If the current thread is the last thread, extend the computation to
	// the end of the results vector to avoid missing the computation of
	// the last pairs.
	if ((long) t == g_numThreads - 1)
		end = g_totalResults;

	// Set values for usePearson, minPearson and maxPearson
	usePearson = g_params.usePearson && g_params.performPearson;
	minPearson = g_params.minPearson;
	maxPearson = g_params.maxPearson;
	
	// Iterate over the results array on a restricted block of results
	for (i = start; i < end; i++)
	{
		// Use Pearson as a ParallelMIC approximation if needed
		if (usePearson)
		{
			// If the data is too noisy
			if (fabs(g_results[i].pearson) <= minPearson)
				mic = g_results[i].pearson;
			
			// If the data seems to be linearly correlated verify with MaxEquipartedGridScore
			else if (fabs(g_results[i].pearson) >= maxPearson)
			{
				if (fabs(fabs(g_results[i].pearson) - MaxEquipartedGridScore(i, (long)t)) < 0.1)
					mic = g_results[i].pearson;
				else
				{
					varX = &*g_inputMatrix[g_results[i].var1];
					varY = &*g_inputMatrix[g_results[i].var2];
					mic = ApproxMaxMI(g_params, varX, varY, g_totalCols);
				}
			}

			// If the data is neither too noisy nor to notably linear, compute ParallelMIC as usual
			else
			{
				varX = &*g_inputMatrix[g_results[i].var1];
				varY = &*g_inputMatrix[g_results[i].var2];
				mic = ApproxMaxMI(g_params, varX, varY, g_totalCols);
			}
		}

		// Compute ParallelMIC if needed
		else
		{
			varX = &*g_inputMatrix[g_results[i].var1];
			varY = &*g_inputMatrix[g_results[i].var2];
			mic = ApproxMaxMI(g_params, varX, varY, g_totalCols);
		}

		// Store resulting MIC metric to the corresponding result
		g_results[i].parallelmic = mic;
	}
	pthread_exit(NULL);
}


bool PerformParallelMIC(double **inputMatrix, int totalRows, int totalCols, mine_result *results, long totalResults, int numThreads, config params)
{
	// Define array of threads and support data structures
	pthread_t thread[numThreads];
	pthread_attr_t attr;
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

	// Set JOINABLE attribute for the threads to be created
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Create the threads
	for (t = 0; t < numThreads; t++)
	{
		rc = pthread_create(&thread[t], &attr, ParallelMICThread, (void*)t);
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

	return true;
}
