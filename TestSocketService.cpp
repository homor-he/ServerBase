// TestSocketService.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include "TcpPthreadServer.h"
//#include "TemplateThreadPool.h"
#include "TimerTask.h"
#include "Test.h"
#include <iostream>
#include "TwentyFour.h"

using namespace std;

class Plus
{
public:
	void Debug(void* para)
	{
		char* str = (char*)para;
		cout << str << endl;
	}
};

void Debug1(void* para)
{
	/*char* str = (char*)para;
	cout << str << endl;*/

	UserData* pData = (UserData*)para;
	pData->endTime = clock();
	cout << "(Debug1)������1��,userid:" << pData->dwUserID << ",������ʱ�䣺" << (pData->endTime - pData->startTime) << endl;

	/*TimerNode* pTimerNode = FindTimerNodeInVector(gbs_TimerNodeList, pData->dwUserID);
	if (NULL != pTimerNode && pTimerNode->dwTimerType != TimerType::Repeat)
	{
		RemoveVectorInSort(gbs_TimerNodeList, pTimerNode);
		cout << "(Debug1)RemoveVectorInSort success" << endl;
	}
	else
	{
		cout << "(Debug1)RemoveVectorInSort fail" << endl;
	}*/
}

void Debug2(void* para)
{
	/*char* str = (char*)para;
	cout << str << endl;*/

	UserData* pData = (UserData*)para;
	pData->endTime = clock();
	cout << "(Debug2)������1��,userid:" << pData->dwUserID << ",������ʱ�䣺" << (pData->endTime - pData->startTime) << endl;

	/*TimerNode* pTimerNode = FindTimerNodeInVector(gbs_TimerNodeList, pData->dwUserID);
	if (NULL != pTimerNode && pTimerNode->dwTimerType != TimerType::Repeat && RemoveVectorInSort(gbs_TimerNodeList, pTimerNode))
	{
		cout << "(Debug2)RemoveVectorInSort success" << endl;
	}
	else
	{
		cout << "(Debug2)RemoveVectorInSort fail" << endl;
	}*/
}

//int main()
//{
//	//ʹ��iocp�������
//	//������ɶ˿�
//	//HANDLE IOCompleteionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//
//	//SYSTEM_INFO si;
//	//GetSystemInfo(&si);
//	//int processors = si.dwNumberOfProcessors;             //��ȡcpu����
//	//int numThreads = 2 * processors;
//	//TcpPthreadServer server(numThreads, "0.0.0.0", 9090, IOCompleteionPort);
//	//server.Start();
//
//
//	//ʹ�����Ա����
//	//ThreadPool pool = ThreadPool(1);
//	//Plus p;
//	//function<void(void*)> f = bind(&Plus::Debug, &p, placeholders::_1);
//	//pool.Start(f);
//	//
//	//Sleep(1000);
//	///*int num;
//	//cin >> num;*/
//	//char* str1 = "hello world";
//	//char* str2 = "hello brother";
//	//pool.AddTask((void*)str1, strlen(str1));
//	//pool.AddTask((void*)str2, strlen(str2));
//
//	//int num2;
//	//cin >> num2;
//
//
//	//TODO:ʹ�÷ǳ�Ա����
//	//TThreadPool<ThreadTask> pool= TThreadPool<ThreadTask>(1);
//	//
//	//function<void(void*)> f = bind(Debug, placeholders::_1);
//	//pool.Start(f);
//	//Sleep(1000);
//	//char* str1 = "hello father";
//	//char* str2 = "hello mom";
//	//pool.AddTask((void*)str1, strlen(str1));
//	//pool.AddTask((void*)str2, strlen(str2));
//	//int num2;
//	//cin >> num2;
//
//	//��ʱ������
//	UserData* userData1 = new UserData();
//	userData1->dwUserID = 4;
//	UserData* userData2 = new UserData();
//	userData2->dwUserID = 1;
//	UserData* userData3 = new UserData();
//	userData3->dwUserID = 3;
//	TThreadPool<TimerTask> pool = TThreadPool<TimerTask>(1);
//	pool.Start();
//	//Sleep(3000);
//	int random = rand() % pool.GetThreadNum();
//	Thread* thread = pool.GetThread(random);
//	TimerTask* task =  (TimerTask*)thread->GetTask();
//	userData1->startTime = clock();
//	task->AddTimer(userData1, sizeof(UserData), bind(Debug1, placeholders::_1), 0 * 1000, TimerType::Repeat, 30 * 1000);
//	task->AddTimer(userData2, sizeof(UserData), bind(Debug2, placeholders::_1), 0 * 1000);
//	task->AddTimer(userData3, sizeof(UserData), bind(Debug2, placeholders::_1), 20 * 1000);
//
//
//	/*int arr[] = { 2,66,21,4,5,7,9,23,15 };
//	Test::QuickSortTest(arr, sizeof(arr) / sizeof(int));*/
//
//	/*UINT64 time1 = clock();
//	vector<float> VEC = {1,8,6,6};
//	TwentyFour twentyFour;
//	twentyFour.Calculate(VEC);
//	twentyFour.Output();
//	UINT64 time2 = clock();
//	cout << "ʱ�䣺"<< (time2 - time1) << endl;*/
//	int x = 0;
//	cin >> x;
//	return 0;
//}

#define   BS_REQ	0x00000000	//������Ϣ���ͣ���32λΪ0
#define	  BS_ACK	0x80000000	//Ӧ����Ϣ���ͣ���32λΪ1           
#define   BS_MSG_ORIGINE     0x600
#define   BS_CONNECTIONTYPE_CLTPC 1100

//#pragma pack(push,1)	// �趨Ϊ1�ֽڶ���,�������״̬
struct header
{
	DWORD	dwMagic;			// ��Ϣħ��
	DWORD	dwSerial;			// ���к�
	WORD	wOrigine;			// ��Ϣ��Դ
	WORD	wReserve;			// ����
	DWORD	dwType;				// ��Ϣ����
	DWORD	dwParam;			// ��Ϣ��������Ϣ�汾������ֵ����־λ�ȣ�
	DWORD	dwLength;			// ʵ�����ݳ��ȣ���������Ϣͷ
};
//#pragma pack(pop)	// //�ָ�����״̬

typedef struct tagBSReqLC2AgentTest
{
	header		header;

	DWORD		dwUserID;
	DWORD       dwIndex;
	UINT64      u64Time;

	DWORD		dwParam1;
	DWORD		dwParam2;
}BSREQ_LC2AGENT_TEST, *PBSREQ_LC2AGENT_TEST;

int main()
{
	TcpSocketBase tcp;
	tcp.Socket(isOverLapped::False);
	string ip = "192.168.109.67";
	tcp.Connect(ip, 30001);
	
	header head = { 0 };
	head.dwType = BS_REQ | BS_MSG_ORIGINE;
	head.wOrigine = BS_CONNECTIONTYPE_CLTPC;
	head.dwLength = sizeof(header);

	char buffer[1024] = { 0 };
	memcpy_s(buffer, 1024, &head, sizeof(header));

	tcp.Send(buffer,1024);



	int x = 0;
	cin >> x;
	return 0;
}