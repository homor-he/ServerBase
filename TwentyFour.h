#ifndef __TWENTYFOUR_H__
#define __TWENTYFOUR_H__

#pragma once

#include "TwentyFour.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

enum Operation
{
	DEFAULT,
	PLUS = 1,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
};

struct SingleCtr
{
	SingleCtr()
	{
		lastSingCtr = NULL;
		num1 = 0;
		num2 = 0;
		res = 0;
		operation = Operation::DEFAULT;
	}
	SingleCtr * lastSingCtr;
	vector<float> origin;
	float num1;
	float num2;
	Operation operation;
	float res;
	vector<float> calProcess;
};

typedef struct Calculation
{
	vector<SingleCtr*> first;
	vector<SingleCtr*> second;
	vector<SingleCtr*> third;
}CALCULATION;


class TwentyFour
{
public:
	TwentyFour();
	~TwentyFour();

public:
	void Calculate(vector<float>& arr);
	void Calculate(SingleCtr * pSingleCtr, vector<float>& arr);
	void Output();
private:
	SingleCtr* GetSingCtr(vector<float>& arr, float num1, float num2, Operation ope);
	SingleCtr* GetSingCtr(SingleCtr * pSingleCtr, float num1, float num2, Operation ope, vector<float>& arr);
	string GetChar(float ope);
	void SetValue(SingleCtr* pSingleCtr, float num1, float num2, Operation ope);
	void pushInRes(string & calProcess);
public:
	Calculation m_Calculation;

	vector<string> realRes;
};

#endif
