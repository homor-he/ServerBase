#include "QuickSort.h"

int Partition(int* arr, int low, int high)
{
	int pivot = arr[low];
	while (low < high)
	{
		while (low < high && arr[high] > pivot)
		{
			--high;
		}
		arr[low] = arr[high];
		while (low < high && arr[low] <= pivot)
		{
			++low;
		}
		arr[high] = arr[low];
	}
	arr[low] = pivot;
	return low;
}

int * QuickSort(int* arr, int low, int high)
{
	if (low < high)
	{
		int index = Partition(arr, low, high);
		QuickSort(arr, low, index - 1);
		QuickSort(arr, index + 1, high);
	}
	return arr;
}