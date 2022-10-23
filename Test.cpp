#include "Test.h"
#include <iostream>

using namespace std;

void Test::QuickSortTest(int * arr,int len)
{
	cout << "ÅÅÐòÇ°£º" << endl;
	for (int i = 0; i < len; ++i)
	{
		cout << arr[i] << ",";
	}
	QuickSort(arr, 0, len - 1);
	cout << endl;
	cout << "ÅÅÐòºó£º" << endl;
	for (int i = 0; i < len; ++i)
	{
		cout << arr[i] << ",";
	}
	cout << endl;
}
