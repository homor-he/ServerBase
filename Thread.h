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
	//start调用此函数，此函数再调用run函数，执行实际的任务
	static unsigned __stdcall ThreadFunction(void*);
protected:
	void setDefaultName();
protected:
	ThreadTask * _task;
	bool _started;				//线程开启状态
	bool _joined;				//线程阻塞状态
	unsigned long _threadID;    //线程ID
	int _threadNo;              //第几个额外启动的线程
	HANDLE  _handle;			//线程句柄
	string _name;				//线程名字
	//ThreadFunc _func;			//线程方法
	CThreadMutex _mutex;
	CThreadCondition _cond;
	//标识是不是动态扩容的线程 true:是动态扩容的 false:不是
	bool _flag;
};


#endif // !__THREAD_H__