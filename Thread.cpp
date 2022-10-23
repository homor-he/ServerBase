#include "Thread.h"

Thread::Thread(string & name,int threadNo, bool flag)
{
	_started = false;
	_joined = false;
	_threadNo = 0;
	_threadID = threadNo;
	_handle = NULL;
	//_func = func;
	_name = name;
	_task = NULL;
	_flag = flag;
}

Thread::~Thread()
{
	if (_task != NULL)
	{
		_task->quit();
		{
			MutexLock lock(_mutex);
			this->join();             //主线程等待子线程执行完
			delete _task;
			_task = NULL;
		}
	}
}


void Thread::Run()
{
	setStartValue(true);
		
	if (_task == NULL)
		_cond.Wait();

	_task->excute(NULL);
	
	//动态扩容的线程自己运行后删除
	if (_flag)
		delete this;
	setStartValue(false);
}

void Thread::SetTask(ThreadTask * pTask)
{
	_task = pTask;
	_cond.Signal();
}

ThreadTask * Thread::GetTask()
{
	return _task;
}

void Thread::start()
{
	unsigned threadId = 0;
	_handle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction, this, 0, &threadId);
	this->_threadID = threadId;
}

void Thread::join()
{
	//默认等待2s,等待子线程结束
	WaitForSingleObject(_handle, 2000);
}

bool Thread::getStartedStatus()
{
	bool value = false;
	{
		MutexLock lock(_mutex);
		value = _started;
	}
	return value;
}

long Thread::getThreadID()
{
	return _threadID;
}

void Thread::setStartValue(bool isStarted)
{
	{
		MutexLock lock(_mutex);
		_started = isStarted;
	}
}


unsigned Thread::ThreadFunction(void * pArg)
{
	Thread* pThread = (Thread*)pArg;
	pThread->Run();
	return 0;
}

void Thread::setDefaultName()
{
	if (_name.empty()) {
		char buf[32];
		snprintf(buf, sizeof buf, "Thread");
		_name = buf;
	}
}
