#include <pthread.h>
#include <stdio.h>
#include <math.h>

#include "customtypes.h"
#include "pearson.h"

static mine_result *g_results;
static double **g_inputMatrix;
static long g_totalResults;
static int g_numThreads;
static int g_totalCols;


void *Pearson(void *t)
{
	// Set auxiliar variables to restrict the thread to a block of data
	long offset;
	long start;
	long end;
	int i;
	int j;

	offset = g_totalResults / g_numThreads;
	start = ((long) t) * offset;
	end = start + offset;

	// If the current thread is the last thread, extend the computation to
	// the end of the results vector to avoid missing the computation of
	// the last pairs.
	if ((long) t == g_numThreads - 1)
		end = g_totalResults;

	// Set auxiliar variables for the Pearson correlation computation
	double *varX;
	double *varY;

	double pearson;
	double errXY;
	double errX2;
	double errY2;
	double errX;
	double errY;
	double avgX;
	double avgY;
	int n;

	n = g_totalCols;

	// Iterate over the results array on a restricted block of results
	for (i = start; i < end; i++)
	{
		// Initialize auxiliary variables
		varX = &*g_inputMatrix[g_results[i].var1];
		varY = &*g_inputMatrix[g_results[i].var2];
		pearson = 0;
		errXY = 0;
		errX2 = 0;
		errY2 = 0;
		errX = 0;
		errY = 0;
		avgX = 0;
		avgY = 0;

		// Compute the Pearson correlation (Magic happens here!).
		for (j = 0; j < n; j++)
		{
			avgX += varX[j];
			avgY += varY[j];
		}
		avgX /= n;
		avgY /= n;

		for (j = 0; j < n; j++)
		{
			errX += varX[j] - avgX;
			errY += varY[j] - avgY;
			errXY += errX * errY;
			errX2 += errX * errX;
			errY2 += errY * errY;
		}

		errX2 = sqrt(errX2);
		errY2 = sqrt(errY2);

		pearson = errXY / (errX2 * errY2);

		// Store Pearson correlation to the corresponding result
		g_results[i].pearson = pearson;
	}
	pthread_exit(NULL);
}


bool PerformPearson(double **inputMatrix, int totalRows, int totalCols, mine_result *results, long totalResults, int numThreads)
{
	// Initialize file-scope global variables
	g_totalResults = totalResults;
	g_inputMatrix = inputMatrix;
	g_numThreads = numThreads;
	g_totalCols = totalCols;
	g_results = results;

	// Define array of threads and support data structures
	pthread_t thread[numThreads];
	pthread_attr_t attr;
	void *status;
	int rc;
	long t;

	// Set JOINABLE attribute for the threads to be created
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Create the threads
	for (t = 0; t < numThreads; t++)
	{
		rc = pthread_create(&thread[t], &attr, Pearson, (void*)t);
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

	return true;
}
