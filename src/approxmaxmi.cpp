#include <math.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "quicksort.h"
#include "customtypes.h"
#include "approxmaxmi.h"

// Define MAX and MIN functions
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)


/*
Eq1 (Section 3.2.1)
Input
	N : q x p matrix containing the number of pointsfor each cell of the grid formed by P and partitions.
	q : number of rows of N (number of partitions in Q)
	p : number of cols of N (number of partitions in P)
	n : total number of points
Output
	H : Returns the entropy induced by the points on the * partition Q.
*/
double Eq1(int **N, int q, int p, int n)
{
	double partialEntropy;
	double H;
	int sum;
	int i;
	int j;

	H = 0.0;
	for (i = 0; i < q; i++)
	{
		sum = 0;
		for (j = 0; j < p; j++)
			sum += N[i][j];

		partialEntropy = (double) sum / (double) n;

		if (partialEntropy != 0)
			H += partialEntropy * log(partialEntropy);
	}

	return -H;
}


/*
Eq2 line 5 of Algorithm 2 in SOM (H({<c_0, c_s, c_t>})).
Input
	np : p vector containing the number of points for each cell of the grid formed by P
	p : length of np (number of partitions in P)
	s : s in c_s
	t : t in c_t
Output
	H : Entropy induced by the points on the partition <c_0, c_s, c_t>.
*/
double Eq2(int *np, int p, int s, int t)
{
	if (s == t)
		return 0.0;

	double partialEntropy1;
	double partialEntropy2;
	double H;
	int total;
	int sum1;
	int sum2;
	int i;

	H = 0.0;

	sum1 = 0;
	for (i = 0; i < s; i++)
		sum1 += np[i];

	sum2 = 0;
	for (i = s; i < t; i++)
		sum2 += np[i];

	total = sum1 + sum2;
	partialEntropy1 = (double) sum1 / (double) total;
	partialEntropy2 = (double) sum2 / (double) total;

	if (partialEntropy1 != 0)
		H += partialEntropy1 * log(partialEntropy1);

	if (partialEntropy2 != 0)
		H += partialEntropy2 * log(partialEntropy2);

	return -H;
}


/*
Eq3 line 5 of Algorithm 2 in SOM (H({<c_0, c_s, c_t>},Q)).
Input:
	N : q x p matrix containing the number of points for each cell of the grid formed by P and Q partitions.
	np : p vector containing the number of points for each cell of the grid formed by P
	q : number of rows of N (number of partitions in Q)
	p : number of cols of N and length of np (number of partitions in P)
	s : s in c_s
	t : t in c_t
Return:
	H : Entropy induced by the points on the partition <c_0, c_s, c_t> and Q.
*/
double Eq3(int **N, int *np, int q, int p, int s, int t)
{
	double partialEntropy1;
	double partialEntropy2;
	double H;
	int sum1;
	int sum2;
	int tot;
	int i;
	int j;

	tot = 0;
	H = 0.0;

	for (i = 0; i < t; i++)
		tot += np[i];

	for (i = 0; i < q; i++)
	{
		sum1 = 0;
		for (j = 0; j < s; j++)
			sum1 += N[i][j];

		sum2 = 0;
		for (j = s; j < t; j++)
			sum2 += N[i][j];

		partialEntropy1 = (double) sum1 / (double) tot;
		partialEntropy2 = (double) sum2 / (double) tot;

		if (partialEntropy1 != 0)
			H += partialEntropy1 * log(partialEntropy1);

		if (partialEntropy2 != 0)
			H += partialEntropy2 * log(partialEntropy2);
	}

	return -H;
}


/*
Eq4 line 13 of Algorithm 2 in SOM (H({c_s, c_t>},Q)).
Input:
	N : q x p matrix containing the number of points
	for each cell of the grid formed by P and Q
	partitions.
	np : p vector containing the number of points for each cell of the grid formed by P
	q : number of rows of N (number of partitions in Q)
	p : number of cols of N and length of np (number of partitions in P)
	s : s in c_s
	t : t in c_t
Return:
	H : Entropy induced by the points on the partition <c_s, c_t> and Q.
*/
double Eq4(int **N, int *np, int q, int p, int s, int t)
{
	if (s == t)
		return 0.0;

	double partialEntropy;
	double H;
	int sum;
	int tot;
	int i;
	int j;

	tot = 0;
	H = 0.0;

	for (i = s; i < t; i++)
		tot += np[i];

	for (i = 0; i < q; i++)
	{
		sum = 0;
		for (j = s; j < t; j++)
			sum += N[i][j];

		partialEntropy = (double) sum / (double) tot;

		if (partialEntropy != 0)
			H += partialEntropy * log(partialEntropy);
	}

	return -H;
}


/*
EquipartitionAxis See Algorithm 3 in SOM
Input:
	Dy : y-data Sorted in increasing order
	n : length of Dy
	y : an integer greater than 1
	Qm(OUT) : the map Q. Qm must be a preallocated vector of size n.
Return:
	q : the real value of y. It can be < y (the map Q: D -> {0, ...,y-1}.)
*/
int EquipartitionAxis(double *Dy, int n, int y, int *Qm)
{
	double rowsize;
	int currrowbeginindex;
	int curr;
	int h;
	int s;
	int i;
	int j;

	rowsize =  (double) n / (double) y;
	currrowbeginindex = -1;
	curr = 0;
	i = 0;
	j = 0;

	while (i < n)
	{
		s = 1;
		for (j = i + 1; j < n; j++)
		{
			if (Dy[i] == Dy[j])
				s++;
			else
				break;
		}

		if (currrowbeginindex != -1)
		{
			h = i - currrowbeginindex;
		}
		else
		{
			currrowbeginindex = 0;
			h = 0;
		}

		if (h != 0 && fabs(h + s - rowsize) >= fabs(h-rowsize))
		{
			curr++;
			currrowbeginindex = i;
			rowsize = (double) (n - i) / (double) (y - curr);
		}

		for (j = 0; j < s; j++)
			Qm[i + j] = curr;

		i += s;
	}

	return curr + 1;
}


/*
ClumpsPartition
Input:
	Dx : x-data Sorted in increasing order
	n : length of Dx
	Qm : the map Q computed by EquipartitionAxis Sorted in increasing order by Dx-values.
	k_hat : maximum number of clumps
Return:
	p : number of clumps in Pm
 */
int ClumpsPartition(double *Dx, int n, int *Qm, int k_hat)
{
	double *Dp;
	int kstart;
	int flag;
	int preQ;
	int p;
	int s;
	int i;
	int j;

	kstart = 0;
	flag = 0;
	preQ = 0;
	p = 0;
	s = 0;
	i = 0;

	// Start clumps partitions
	while (i < n)
	{
		if (i == 0)
		{
			preQ = Qm[i];
			Qm[i] = p;
		}
		else
		{
			if (Dx[i] == Dx[i - 1])
			{
				if (flag == 1)
				{
					preQ = Qm[i];
					Qm[i] = Qm[i - 1];
				}

				else if (preQ!=Qm[i])
				{
					flag = 1;
					preQ = Qm[i];

					if (kstart != s)
					{
						kstart = s;
						p++;
					}

					for (j = s; j <= i; j++)
						Qm[j]=p;
				}

				if (!flag)
					Qm[i]=Qm[i-1];

			}

			else
			{
				s = i;
				if (flag == 1)
				{
					p++;
					preQ = Qm[i];
					kstart = i;
					Qm[i] = p;
					flag = 0;
				}
				else
				{
					if (Qm[i] == preQ)
					{
						preQ = Qm[i];
						Qm[i] = Qm[i-1];
					}
					else
					{
						preQ = Qm[i];
						p++;
						Qm[i] = p;
						kstart = i;
					}
				}
			}
		}
		i++;
	} // End while

	// End clumps partitions
	p = p + 1;

	// Check SuperClumps
	if (k_hat >= p) // Use clumps
		return p;
	else 			// use SuperCumpls
	{
		Dp = new double[n];

		for (i = 0; i < n; i++)
			Dp[i] = (double) Qm[i];
		p = EquipartitionAxis(Dp, n, k_hat, Qm);

		delete[] Dp;
		return p;
	}
}


/*
OptimizeAxis
Input:
	Dx : x-data Sorted in increasing order by Dx-values
	Dy : y-data Sorted in increasing order by Dx-values
	n : length of Dx and Dy
	Qm : the map Q computed by EquipartitionAxis Sorted in increasing order by Dx-values.
	q : number of clumps in Qm
	Pm : the map P computed by GetSuperclumpsPartition
	Sorted in increasing order by Dx-values.
	p : number of clumps in Pm
	x : grid size on x-values
	I(OUT) : the normalized mutual information vector. It
*/
void OptimizeAxis(double *Dx, double *Dy, int n, int *Qm, int q, int *Pm, int p, int x, double *I)
{
	// If only 1 clump is considered, set all I elements to zero and return
	if (p == 1)
	{
		for (int i = 0; i < x - 1; i++)
			I[i] = 0.0;
		return;
	}

	// Define datastructures and variables
	double **hpq2;
	double **IM;	// Mutual information matrix

	double hppq;	// Stores the entropy corresponding to H(P) - H(P, Q)
	double hq;		// Stores the entropy corresponding to H(Q)
	double r1;
	double r2;
	double f;

	int **N;		// Contains the number of samples for each cell Q x P
	int *np;		// Contains the number of samples for each cell P
	int *c;			// Contains c_1, ..., c_k

	int L;
	int s;
	int t;
	int j;
	int i;

	// Allocate data structures
	hpq2 = new double*[p + 1];
	IM = new double*[p + 1];
	np = new int[p];
	N = new int*[q];
	c = new int[p];

	for (i = 0; i <= p; i++)
	{
		IM[i] = new double[x + 1];
		hpq2[i] = new double[p + 1];
	}

	for (i = 0; i < q; i++)
		N[i] = new int[p];


	// Initialize data structures
	for (i = 0; i < q; i++)
		memset(N[i], 0, p * sizeof(int));

	memset(np, 0, p * sizeof(int));

	for (i = 0; i < n; i++)
	{
		N[Qm[i]][Pm[i]] += 1;
		np[Pm[i]] += 1;
	}

	// Compute c_1, ..., c_k
	c[0] = np[0];

	// Precompute H(<c_s, c_t>, Q) matrix
	for (i = 0; i <= p; i++)
	{
		if (i != 0 && i != p)
			c[i] = np[i] + c[i-1];

		for (j = 0; j <= x; j++)
			IM[i][j] = 0.0;
	}

	// Compute H(Q)
	hq = Eq1(N, q, p, n);

	// Find the optimal partitions of size 2
	// Algorithm 2 in SOM, lines 4-8
	for (t = 2; t <= p; t++)
	{
		hppq = -DBL_MAX;
		for (s = 1; s <= t; s++)
		{
			f = Eq2(np, p, s, t) - Eq3(N, np, q, p, s, t);
			if (f > hppq)
			{
				IM[t][2] = hq + f;
				hppq = f;
			}
		}
	}

	for (t = 3; t <= p; t++)
	{
		for (s = 2; s <= t; s++)
			hpq2[s][t] = Eq4(N, np, q, p, s, t);
	}

	// Inductively build the rest of the table of optimal partitions
	// Algorithm 2 in SOM, lines 11-17
	for (L = 3; L <= x; L++)
	{
		for (t = L; t <= p; t++)
		{
			hppq = -DBL_MAX;
			for (s = L - 1; s <= t; s++)
			{
				r1 = (double) c[s - 1] / (double) c[t - 1];
				r2 = (double) (c[t - 1] - c[s - 1]) / (double) c[t - 1];
				f = (r1 * (IM[s][L-1] - hq)) - (r2 * hpq2[s][t]);

				if (f > hppq)
				{
					IM[t][L] = hq + f;
					hppq = f;
				}
			}
		}
	}

	// Algorithm 2 line 19
	if (x > p)
	{
		for (i = p + 1; i <= x; i++)
			IM[p][i] = IM[p][p];
	}

	// Normalize results
	for (i = 2; i <= x; i++)
		I[i-2] = IM[p][i] / MIN(log(i), log(q));

	// Deallocate resources
	for (i = 0; i < q; i++)
		delete[] N[i];

	for (i = 0; i <= p; i++)
	{
		delete[] IM[i];
		delete[] hpq2[i];
	}

	delete[] hpq2;
	delete[] IM;
	delete[] np;
	delete[] N;
	delete[] c;

	return;
}


// Sort x and idx (of length n) according to x
void Sort(double *x, int *idx, int n)
{
	Quicksort(x, idx, 0, n-1);
}


// Initialize the problem (allocate memory, fill and sort the information)
void AllocateProblem(parallelmic_data *micData, double alpha)
{
	// Auxiliar variables
	double nAlpha;
	int Gy_max;
	int gx;
	int gy;
	int i;

	nAlpha = MAX(pow(micData -> n, alpha), 4);
	Gy_max = MAX((int)floor(nAlpha / 2.0), 2);
	Gy_max--;

	// Allocate memory for the data structure
	micData -> x_x = new double[micData -> n];
	micData -> x_y = new double[micData -> n];
	micData -> y_x = new double[micData -> n];
	micData -> y_y = new double[micData -> n];
	micData -> idx_x = new int[micData -> n];
	micData -> idx_y = new int[micData -> n];
	micData -> Gy = new int[Gy_max];

	micData -> score = new parallelmic_score;
	micData -> score -> I = new double* [Gy_max];
	micData -> score -> p = new int[Gy_max];
	micData -> score -> m = Gy_max;

	// Fill data structures
	for (gy = 2; gy < Gy_max + 2; gy++)
	{
		gx = (int) floor(nAlpha / gy);

		// If the resolution is valid
		if (gx * gy <= nAlpha)
		{
			micData -> score -> p[gy - 2] = gx - 1;
			micData -> Gy[gy - 2] = gy;
		}
	}

	for (i = 0; i < micData -> n; i++)
	{
		micData -> x_x[i] = micData -> x[i];
		micData -> y_y[i] = micData -> y[i];
		micData -> idx_x[i] = i;
		micData -> idx_y[i] = i;
	}

	// Sort x by y-value and Sort y by x-value
	Sort(micData -> x_x, micData -> idx_x, micData -> n);
	Sort(micData -> y_y, micData -> idx_y, micData -> n);
	for (i = 0; i<micData -> n; i++)
	{
		micData -> x_y[i] = micData -> x[micData -> idx_y[i]];
		micData -> y_x[i] = micData -> y[micData -> idx_x[i]];
	}
}


/*
FixXPartition
Input:
	micData: Structure that contains MIC data
	clumps: input parameter
	n : length of Dx and Dy
	indexP : iterator
	I(OUT) : the normalized mutual information vector. It
	Qm(OUT) : the map Q computed by EquipartitionAxis Sorted in increasing order by Dx-values
	Pm(OUT) : the map P computed by GetSuperclumpsPartition
*/
void FixXpartition(parallelmic_data *micData, double clumps, int indexP, double *I, int *Pm, int *Qm)
{
	int p;
	int q;
	int j;
	int k;

	k = MAX((int)(clumps * (micData -> score -> p[indexP] + 1)), 1);
	q = EquipartitionAxis(micData -> y_y, micData -> n, micData -> Gy[indexP], Qm);

	for (j = 0; j < micData -> n; j++)
		Pm[micData -> idx_y[j]] = Qm[j];

	for (j = 0; j < micData -> n; j++)
		Qm[j] = Pm[micData->idx_x[j]];

	memcpy(Pm, Qm, sizeof(int) * micData -> n);
	p = ClumpsPartition(micData -> x_x, micData -> n, Pm, k);
	OptimizeAxis(micData -> x_x, micData -> y_x, micData -> n, Qm, q, Pm, p, micData -> score -> p[indexP] + 1, I);
}


/*
FixYPartition
Input:
	micData: Structure that contains MIC data
	clumps: input parameter
	n : length of Dx and Dy
	I(OUT) : the normalized mutual information vector. It
	Qm(OUT) : the map Q computed by EquipartitionAxis Sorted in increasing order by Dx-values
	Pm(OUT) : the map P computed by GetSuperclumpsPartition
*/
void FixYpartition(parallelmic_data *micData, double clumps, int indexP, double *I, int *Pm, int *Qm)
{
	int p;
	int q;
	int j;
	int k;

	k = MAX((int)(clumps * (micData -> score -> p[indexP] + 1)), 1);
	q = EquipartitionAxis(micData -> x_x, micData -> n, micData -> Gy[indexP], Qm);

	for (j = 0; j < micData -> n; j++)
		Pm[micData -> idx_x[j]] = Qm[j];

	for (j = 0; j < micData -> n; j++)
		Qm[j] = Pm[micData -> idx_y[j]];

	memcpy(Pm, Qm, sizeof(int) * micData -> n);
	p = ClumpsPartition(micData -> y_y, micData -> n, Pm, k);
	OptimizeAxis(micData -> y_y, micData -> x_y, micData -> n, Qm, q, Pm, p, micData -> score -> p[indexP] + 1, I);
}


// Computate MIC score for a pair of variables
parallelmic_score *ComputeScoreMIC(parallelmic_data *micData, double clumps)
{
	double *it;
	int *qM_s;
	int *qM;
	int i;
	int j;

	qM = new int[micData -> n];
	qM_s = new int[micData -> n];

	/* x vs. y */
	for (i = 0; i < micData -> score -> m; i++)
	{
		micData -> score -> I[i] = new double[micData -> score -> p[i]];
		FixXpartition(micData, clumps, i, micData -> score -> I[i], qM, qM_s);
	}

	/* y vs. x */
	for (i = 0; i < micData -> score -> m; i++)
	{
		it = new double[micData -> score -> p[i]];
		FixYpartition(micData, clumps, i, it, qM, qM_s);
		for (j = 0; j < micData -> score -> p[i]; j++)
			micData -> score -> I[j][i] = MAX(it[j], micData -> score -> I[j][i]);		
		delete[] it;
	}

	delete[] qM;
	delete[] qM_s;

	return micData -> score;
}


// Frees the memory used by the computation of MIC
void DeallocateProblem(parallelmic_data *micData)
{
	int i;

	// Delete problem score structure
	for (i = 0; i < micData -> score -> m; i++)
		delete[] micData -> score -> I[i];
	delete[] micData -> score -> I;
	delete[] micData -> score -> p;
	micData -> score -> I = NULL;
	delete micData -> score;

	// Delete problem arrays
	delete[] micData -> Gy;
	delete[] micData -> x_x;
	delete[] micData -> y_y;
	delete[] micData -> x_y;
	delete[] micData -> y_x;
	delete[] micData -> idx_x;
	delete[] micData -> idx_y;

	// Set pointers to a safe state
	micData -> Gy = NULL;
	micData -> x_x = NULL;
	micData -> y_y = NULL;
	micData -> x_y = NULL;
	micData -> y_x = NULL;
	micData -> idx_x = NULL;
	micData -> idx_y = NULL;
	micData -> score = NULL;
}


/*
ApproxMaxMi:
  Given the arrays varX and varY compute their MIC score and return it

Input
  params : Configuration structure that stores the parameters needed for ApproxMaxMi (alpha and c)
  varX : Var 1 of the tuple
  varY : Var 2 of the tuple
  n : Number of registers per var in the dataset

Return:
  mic : MIC score
*/
double ApproxMaxMI(config params, double *varX, double *varY, int n)
{
	double mic;
	int i;
	int j;

	// Fill parallelmic_data structure store the processed data
	parallelmic_data micData;
	micData.x = varX;
	micData.y = varY;
	micData.n = n;

	// Allocates and initializes the memory for the problem
	AllocateProblem(&micData, params.alpha);

	// Compute characteristic matrix
	ComputeScoreMIC(&micData, params.c);

	// Retrieve the best score (MIC) from the characteristic matrix
	mic = -1;
	for (i = 0; i < micData.score -> m; i++)
	{
		for (j = 0; j < micData.score -> p[i]; j++)
		{
			if (micData.score->I[i][j] > mic)
				mic = micData.score->I[i][j];
		}
	}

	// Deallocate the used memory
	DeallocateProblem(&micData);

	return mic;
}
