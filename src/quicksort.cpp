#include "quicksort.h"

void Swap(double *x, int *idx, int i, int j)
{
	double ftmp;
	int itmp;

	ftmp = x[i];
	x[i] = x[j];
	x[j] = ftmp;

	itmp = idx[i];
	idx[i] = idx[j];
	idx[j] = itmp;
}


int Partition(double *array, int *idx, int lo, int hi)
{
	double pivot;
	int mid;
	int i;
	int j;

	mid = lo + (hi - lo) / 2;
	pivot = array[mid];

	// Move the pivot to the front.
	Swap(array, idx, mid, lo);
	i = lo + 1;
	j = hi;
	while (i <= j)
	{
		while(i <= j && array[i] <= pivot)
			i++;

		while(i <= j && array[j] > pivot)
			j--;

		if (i < j)
			Swap(array, idx, i, j);
	}
	Swap(array, idx, i - 1, lo);
	return i - 1;
}

void Quicksort(double *array, int *idx, int lo, int hi)
{
	if (lo >= hi)
		return;

	int part = Partition(array, idx, lo, hi);
	Quicksort(array, idx, lo, part - 1);
	Quicksort(array, idx, part + 1, hi);
}
