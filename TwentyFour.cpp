#include "TwentyFour.h"

TwentyFour::TwentyFour()
{
}

TwentyFour::~TwentyFour()
{
}

void TwentyFour::Calculate(vector<float>& arr)
{
	if (arr.size() == 1)    //递归返回条件
		return;

	for (int i = arr.size()-1; i >0; --i)
	{
		for (int j = i-1; j >=0; --j)     //取两个数
		{
			for (int ope = Operation::PLUS; ope <= Operation::DIVIDE; ope = ope + 1)   //加减乘除操作
			{
				//if (arr[i] < 0 || arr[j] < 0) return;
				SingleCtr* singCtr1 = GetSingCtr(arr, arr[i], arr[j], (Operation)ope);
				Calculate(singCtr1,arr);

				//if (ope == Operation::DIVIDE || ope == Operation::SUBTRACT)
				//{
					if (arr[i] == arr[j]) continue;
					SingleCtr* singCtr2 = GetSingCtr(arr, arr[j], arr[i], (Operation)ope);
					Calculate(singCtr2,arr);
				//}
			}
		}
	}
}

void TwentyFour::Calculate(SingleCtr * pSingleCtr, vector<float>& arr)
{
	if (pSingleCtr->origin.size() == 1)    //递归返回条件
		return;

	for (int i = pSingleCtr->origin.size()-1; i >0; --i)
	{
		for (int j = i-1; j >=0 ; --j)     //取两个数
		{
			for (int ope = Operation::PLUS; ope <= Operation::DIVIDE; ope = ope + 1)   //加减乘除操作
			{
				//if (pSingleCtr->origin[i] < 0 || pSingleCtr->origin[j] < 0) return;
				SingleCtr* singCtr = GetSingCtr(pSingleCtr, pSingleCtr->origin[i], pSingleCtr->origin[j], (Operation)ope,arr);
				singCtr->lastSingCtr = pSingleCtr;
				Calculate(singCtr,arr);

				//if (ope == Operation::DIVIDE || ope == Operation::SUBTRACT)
				//{
					if (pSingleCtr->origin[i] == pSingleCtr->origin[j]) continue;
					SingleCtr* singCtr2 = GetSingCtr(pSingleCtr, pSingleCtr->origin[j], pSingleCtr->origin[i], (Operation)ope,arr);
					singCtr2->lastSingCtr = pSingleCtr;
					Calculate(singCtr2,arr);
				//}
			}
		}
	}
}

void TwentyFour::Output()
{
	if (m_Calculation.third.size() == 0)
	{
		cout << "无法计算出24点" << endl;
		return;
	}
	
	for (vector<SingleCtr*>::iterator itor = m_Calculation.third.begin(); itor != m_Calculation.third.end(); ++itor)
	{
		SingleCtr* pSingCtr = *itor;
		string s = "";

		SingleCtr* firstSingCtr = pSingCtr->lastSingCtr->lastSingCtr;
		SingleCtr* SecondSingCtr = pSingCtr->lastSingCtr;
		if (firstSingCtr == NULL || SecondSingCtr == NULL) continue;
		s.append("(");
		s.append(to_string((int)firstSingCtr->calProcess[0]));
		s.append(GetChar(firstSingCtr->calProcess[2]));
		s.append(to_string((int)firstSingCtr->calProcess[1]));
		s.append(")");
		//第一种情况j=0.j=1对应的值都不等同于res
		if ((SecondSingCtr->calProcess[0] != firstSingCtr->res) && (SecondSingCtr->calProcess[1] != firstSingCtr->res))
		{
			if (pSingCtr->calProcess[0] == SecondSingCtr->res)
			{
				s.insert(0, GetChar(pSingCtr->calProcess[2]));
				s.insert(0,")");
				s.insert(0, to_string((int)SecondSingCtr->calProcess[1]));
				s.insert(0, GetChar(SecondSingCtr->calProcess[2]));
				s.insert(0, to_string((int)SecondSingCtr->calProcess[0]));
				s.insert(0, "(");
			}
			else
			{
				s.append(GetChar(pSingCtr->calProcess[2]));
				s.append("(");
				s.append(to_string((int)SecondSingCtr->calProcess[0]));
				s.append(GetChar(SecondSingCtr->calProcess[2]));
				s.append(to_string((int)SecondSingCtr->calProcess[1]));
				s.append(")");
			}
			//cout << s << endl;
			pushInRes(s);
			continue;
		}
		else
		{
			//第二种情况j=0和j=1两个值相等
			if (SecondSingCtr->calProcess[0] == SecondSingCtr->calProcess[1])
			{
				s.insert(0, "(");
				s.append(GetChar(SecondSingCtr->calProcess[2]));
				s.append(to_string((int)SecondSingCtr->calProcess[1]));
				s.append(")");
			}
			else
			{
				for (int j = 0; j < 3; ++j)
				{
					if (j != 2)
					{
						if (firstSingCtr->res == SecondSingCtr->calProcess[j])
						{
							//第三种情况j=1和res相等,在s前面拓展
							if (j == 1)
							{
								s.insert(0, "(");
								s.insert(0, GetChar(SecondSingCtr->calProcess[2]));
								s.insert(0, to_string((int)SecondSingCtr->calProcess[0]));
								s.append(")");
							}
							else  //第三种情况j=0和res相等，在s后面推展
							{
								s.insert(0, "(");
								s.append(GetChar(SecondSingCtr->calProcess[2]));
								s.append(to_string((int)SecondSingCtr->calProcess[1]));
								s.append(")");
							}
						}
						else
							continue;
					}
					else
						continue;
				}
			}

		}

		if (pSingCtr->calProcess[0] == pSingCtr->calProcess[1])
		{
			s.insert(0, "(");
			s.append(GetChar(pSingCtr->calProcess[2]));
			s.append(to_string((int)pSingCtr->calProcess[1]));
			s.append(")");
		}
		else
		{
			for (int j = 0; j < 3; ++j)
			{
				if (j != 2)
				{
					if (SecondSingCtr->res == pSingCtr->calProcess[j])
					{
						//第三种情况j=1和res相等,在s前面拓展
						if (j == 1)
						{
							s.insert(0, "(");
							s.insert(0, GetChar(pSingCtr->calProcess[2]));
							s.insert(0, to_string((int)pSingCtr->calProcess[0]));
							s.append(")");
						}
						else  //第三种情况j=0和res相等，在s后面推展
						{
							s.insert(0, "(");
							s.append(GetChar(pSingCtr->calProcess[2]));
							s.append(to_string((int)pSingCtr->calProcess[1]));
							s.append(")");
						}
					}
					else
						continue;

				}
				else
					continue;
			}
		}

		//cout << s << endl;
		pushInRes(s);
	}
}

SingleCtr* TwentyFour::GetSingCtr(vector<float>& arr, float num1, float num2, Operation ope)
{
	SingleCtr* singCtr = new SingleCtr;
	for (int k = 0; k < arr.size(); ++k)
	{
		singCtr->origin.push_back(arr[k]);
	}
	SetValue(singCtr, num1, num2, ope);

	switch (arr.size())
	{
	case 4:
		m_Calculation.first.push_back(singCtr);
		break;
	case 3:
		m_Calculation.second.push_back(singCtr);
		break;
	case 2:
		if (singCtr->res == 24)
		{
			m_Calculation.third.push_back(singCtr);
		}
		break;
	}
	return singCtr;
}

SingleCtr * TwentyFour::GetSingCtr(SingleCtr * pSingleCtr, float num1, float num2, Operation ope, vector<float>& arr)
{
	SingleCtr* singCtr = new SingleCtr;
	for (int k = 0; k < pSingleCtr->origin.size(); ++k)
	{
		singCtr->origin.push_back(pSingleCtr->origin[k]);
	}
	SetValue(singCtr, num1, num2, ope);

	switch (pSingleCtr->origin.size())
	{
	case 4:
		m_Calculation.first.push_back(singCtr);
		break;
	case 3:
		m_Calculation.second.push_back(singCtr);
		break;
	case 2:
		if (singCtr->res == 24)
		{
			m_Calculation.third.push_back(singCtr);
		}
		break;
	}
	return singCtr;
}

string TwentyFour::GetChar(float ope)
{
	switch ((int)ope)
	{
	case 1:
		return "+";
	case 2:
		return "-";
	case 3:
		return "*";
	case 4:
		return "/";
	}
	return "";
}

void TwentyFour::SetValue(SingleCtr * pSingleCtr, float num1, float num2, Operation ope)
{
	pSingleCtr->num1 = num1;
	pSingleCtr->num2 = num2;
	pSingleCtr->operation = (Operation)ope;

	switch (pSingleCtr->operation)
	{
	case Operation::PLUS:
		pSingleCtr->res = pSingleCtr->num1 + pSingleCtr->num2;
		break;
	case Operation::SUBTRACT:
		pSingleCtr->res = pSingleCtr->num1 - pSingleCtr->num2;
		break;
	case Operation::MULTIPLY:
		pSingleCtr->res = pSingleCtr->num1 * pSingleCtr->num2;
		break;
	case Operation::DIVIDE:
		pSingleCtr->res = pSingleCtr->num1 / pSingleCtr->num2;
		break;
	}

	vector<float>::iterator itor1 = find(pSingleCtr->origin.begin(), pSingleCtr->origin.end(), pSingleCtr->num1);
	if (itor1 != pSingleCtr->origin.end())
		pSingleCtr->origin.erase(itor1);
	vector<float>::iterator itor2 = find(pSingleCtr->origin.begin(), pSingleCtr->origin.end(), pSingleCtr->num2);
	if (itor2 != pSingleCtr->origin.end())
		pSingleCtr->origin.erase(itor2);
	pSingleCtr->origin.push_back(pSingleCtr->res);

	pSingleCtr->calProcess.push_back(pSingleCtr->num1);
	pSingleCtr->calProcess.push_back(pSingleCtr->num2);
	pSingleCtr->calProcess.push_back(pSingleCtr->operation);
}

void TwentyFour::pushInRes(string & calProcess)
{
	vector<string>::iterator itor = find(realRes.begin(), realRes.end(), calProcess);
	if (itor == realRes.end())
	{
		realRes.emplace_back(calProcess);
		cout << calProcess << endl;
	}
	else
	{
		cout << "重复" << endl;
	}
}


