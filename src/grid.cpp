#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>

#include "customtypes.h"
#include "grid.h"

#define MAX_RANDOM 18446744073709551616.0f

grid CreateGrid(int max1DPartition)
{
	grid g;
	g.nx = -1;
	g.ny = -1;
	g.min_x = -1;
	g.min_y = -1;
	g.max_x = -1;
	g.max_y = -1;
	g.fitness = -1;
	g.placements_x = new double[max1DPartition];
	g.placements_y = new double[max1DPartition];
	return g;
}


unsigned int GetPositionX(grid *g, double val)
{
	if (val >= g -> max_x)
		return g -> nx - 1;
	if (val <= g -> min_x)
		return 0;

	if (val < g -> placements_x[0])
		return 0;
	if (val >= g -> placements_x[g -> nx - 2])
		return g -> nx - 1;

	for (unsigned int i = 1; i < g -> nx - 1; i++)
	{
		if (val < g -> placements_x[i])
			return i;
	}

	// Ideally this line should not be reached under normal conditions
	return 0;
}


unsigned int GetPositionY(grid *g, double val)
{
	if (val >= g -> max_y)
		return 0;
	if (val <= g -> min_y)
		return g -> ny - 1;

	if (val > g -> placements_y[0])
		return 0;
	if (val <= g -> placements_y[g -> ny - 2])
		return g -> ny - 1;
	for (unsigned int i = 1; i < g -> ny - 1; i++)
	{
		if (val > g -> placements_y[i])
			return i;
	}

	// Ideally this line should not be reached under normal conditions
	return 0;
}


int GetMax1DPartition(double nAlpha)
{
	for (int i = 2; i < nAlpha; i++)
	{
		if (2 * i >= nAlpha)
			return i;
	}
	return 2;
}


void PrintGridInfo(grid *g)
{
	unsigned int i;
	printf("Grid Resolution: (%d, %d)\n", g -> nx, g -> ny);
	printf("\n");
	printf("X range: [%0.3f, %0.3f]\n", g -> min_x, g -> max_x);
	printf("Y range: [%0.3f, %0.3f]\n", g -> min_y, g -> max_y);
	printf("\n");
	printf("Placements X:\n  [ ");
	for (i = 0; i < g -> nx - 1; i++)
		printf("%0.3f ", g -> placements_x[i]);
	printf("]\n");

	printf("\n");
	printf("Placements Y:\n  [ ");
	for (i = 0; i < g -> ny - 1; i++)
		printf("%0.3f ", g -> placements_y[i]);
	printf("]\n");

	printf("\n");
}


void Reverse(double *array, int n)
{
	int i;
	int j;
	double tmp;

	i = 0;
	j = n - 1;

	while (i < j)
	{
		tmp = array[i];
		array[i] = array[j];
		array[j] = tmp;
		i++;
		j--;
	}
}


bool EquipartitionX(grid *g)
{
	unsigned int i;
	double offset;

	offset = (g -> max_x - g -> min_x) / g -> nx;

	for (i = 0; i < g -> nx - 1; i++)
		SetPlacementX(g, i, g -> min_x + (i + 1) * offset);
	return true;
}


bool EquipartitionY(grid *g)
{
	unsigned int i;
	double offset;

	offset = (g -> max_y - g -> min_y) / g -> ny;

	for (i = 0; i < g -> ny - 1; i++)
		SetPlacementY(g, i, g -> min_y + (i + 1) * offset);
	Reverse(g -> placements_y, g -> ny - 1);
	return true;
}


// CopyGrid(from, to) Currently not in use
bool CopyGrid(grid *g1, grid *g2)
{
	unsigned int i;
	unsigned int nx;
	unsigned int ny;

	nx = g1 -> nx;
	ny = g1 -> ny;
	g2 -> min_x = g1 -> min_x;
	g2 -> min_y = g1 -> min_y;
	g2 -> max_x = g1 -> max_x;
	g2 -> max_y = g1 -> max_y;
	g2 -> fitness = g1 -> fitness;

	SetResolution(g2, nx, ny);

	for (i = 0; i < nx - 1; i++)
		g2 -> placements_x[i] = g1 -> placements_x[i];

	for (i = 0; i < ny - 1; i++)
		g2 -> placements_y[i] = g1 -> placements_y[i];

	return true;
}


bool ClearPlacements(grid *g)
{
	if (g -> placements_x != NULL || g -> placements_y != NULL)
	{
		delete[] g -> placements_x;
		delete[] g -> placements_y;
		g -> placements_x = NULL;
		g -> placements_y = NULL;
	}
	return true;
}


bool SetRandomPlacements(grid *g, rng_config *rng_conf)
{
	unsigned int i;

	for (i = 0; i < g -> nx - 1; i++)
		if (!SetPlacementX(g, i, RandomPlacement(g -> min_x, g -> max_x, rng_conf)))
			return false;

	for (i = 0; i < g -> ny - 1; i++)
		if (!SetPlacementY(g, i, RandomPlacement(g -> min_y, g -> max_y, rng_conf)))
			return false;

	return true;
}


bool SetPlacementX(grid *g, unsigned int i, double val)
{
	try
	{
		g -> placements_x[i] = val;
	}
	catch (...)
	{
		return false;
	}
	return true;
}


bool SetPlacementY(grid *g, unsigned int i, double val)
{
	try
	{
		g -> placements_y[i] = val;
	}
	catch (...)
	{
		return false;
	}
	return true;
}


bool SetResolution(grid *g, unsigned int nx, unsigned int ny)
{
	g -> nx = nx;
	g -> ny = ny;
	return true;
}


bool DiscoverMinMax(grid *g, unsigned int data_len, double *data_x, double *data_y)
{
	unsigned int i;
	try
	{
		g -> min_x = data_x[0];
		g -> max_x = data_x[0];
		g -> min_y = data_y[0];
		g -> max_y = data_y[0];
		for (i = 0; i < data_len; i++)
		{
			// Get g -> min_x and g -> max_x
			if (data_x[i] < g -> min_x)
				g -> min_x = data_x[i];
			if (data_x[i] > g -> max_x)
				g -> max_x = data_x[i];

			// Get g -> min_y and g -> max_y
			if (data_y[i] < g -> min_y)
				g -> min_y = data_y[i];
			if (data_y[i] > g -> max_y)
				g -> max_y = data_y[i];
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}


// Marsaglia RNG - Period 2^96-1
unsigned long MarsagliaRNG(rng_config *rng_conf)
{
	unsigned long t;
	rng_conf -> x ^= rng_conf -> x << 16;
	rng_conf -> x ^= rng_conf -> x >> 5;
	rng_conf -> x ^= rng_conf -> x << 1;

	t = rng_conf -> x;
	rng_conf -> x = rng_conf -> y;
	rng_conf -> y = rng_conf -> z;
	rng_conf -> z = t ^ rng_conf -> x ^ rng_conf -> y;

	return rng_conf -> z;
}


double RandomPlacement(double a, double b, rng_config *rng_conf)
{
	return ((b - a) * ((double) MarsagliaRNG(rng_conf) /  MAX_RANDOM )) + a;
}


bool SortPlacementsX(grid *g)
{
	std::sort(g -> placements_x, g -> placements_x + g -> nx - 2);
	return true;
}


bool SortPlacementsY(grid *g)
{
	std::sort(g -> placements_y, g -> placements_y + g -> ny - 2);
	Reverse(g -> placements_y, g -> ny - 1);
	return true;
}


double ComputeFitness(grid *g, unsigned int data_len, double *data_x, double *data_y)
{
	int i;
	int n;
	int col;
	int row;
	int cols;
	int rows;

	double I;
	double *p_x;
	double *p_y;
	double *p_xy;

	// Initialize variables
	n = data_len;
	rows = g -> ny;
	cols = g -> nx;

	p_x = new double[g -> nx];
	p_y = new double[g -> ny];
	p_xy = new double[g -> nx * g -> ny];

	for (i = 0; i < rows; i++)
		p_y[i] = 0;

	for (i = 0; i < cols; i++)
		p_x[i] = 0;

	for (i = 0; i < cols * rows; i++)
		p_xy[i] = 0;

	// Ensure placements X and Y are sorted
	SortPlacementsX(g);
	SortPlacementsY(g);

	// Count how many data points are contained by each row, col and cell in the grid
	for (i = 0; i < n; i++)
	{
		col = GetPositionX(g, data_x[i]);
		row = GetPositionY(g, data_y[i]);
		p_xy[col + row * cols] ++;
		p_x[col] ++;
		p_y[row] ++;
	}

	// Compute probabilities for each row, col and cell of the grid
	for (i = 0; i < rows; i++)
		p_y[i] /= n;

	for (i = 0; i < cols; i++)
		p_x[i] /= n;

	for (i = 0; i < cols * rows; i++)
		p_xy[i] /= n;

	// Compute Information Coefficient
	I = 0.0;
	for (col = 0; col < cols; col++)
	{
		for (row = 0; row < rows; row++)
		{
			if (p_xy[col + row * cols] != p_x[col] * p_y[row] &&
				p_xy[col + row * cols] != 0 && 
				p_x[col] * p_y[row] != 0)
				I += p_xy[col + row * cols] * log(p_xy[col + row * cols] / (p_y[row] * p_x[col]));
		}
	}

	I = I / log(std::min(rows, cols));

	g -> fitness = I;

	delete[] p_x;
	delete[] p_y;
	delete[] p_xy;
	return I;
}
