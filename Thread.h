#ifndef __THREAD_H__
#define __THREAD_H__

#pragma once

#include <Windows.h>
#include <string>
#include <thread>
#include <process.h>
#include "ThreadTask.h"
#include "ThreadMutex.h"
#include "ThreadCondition.h"

using namespace std;

class Thread
{
public:
	Thread(string& name, int threadNo, bool flag = false);
	virtual ~Thread();
	virtual void Run();
	virtual void SetTask(ThreadTask* pTask);
	virtual ThreadTask* GetTask();

	void start();
	void join();
	bool getStartedStatus();
	long getThreadID();
	void setStartValue(bool isStarted);
protected:
	//start���ô˺������˺����ٵ���run������ִ��ʵ�ʵ�����
	static unsigned __stdcall ThreadFunction(void*);
protected:
	void setDefaultName();
protected:
	ThreadTask * _task;
	bool _started;				//�߳̿���״̬
	bool _joined;				//�߳�����״̬
	unsigned long _threadID;    //�߳�ID
	int _threadNo;              //�ڼ��������������߳�
	HANDLE  _handle;			//�߳̾��
	string _name;				//�߳�����
	//ThreadFunc _func;			//�̷߳���
	CThreadMutex _mutex;
	CThreadCondition _cond;
	//��ʶ�ǲ��Ƕ�̬���ݵ��߳� true:�Ƕ�̬���ݵ� false:����
	bool _flag;
};


#endif // !__THREAD_H__