#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads)
{
	_numThreads = numThreads;
	_started = false;
	_next = 0;
	if (numThreads <= 0)
	{
		cout << "numThreads_<=0" << endl;
		abort();							  //�쳣��ֹ�ý���
	}
}

ThreadPool::~ThreadPool()
{
	for (size_t i = 0; i < _threads.size(); ++i)
	{
		delete _threads[i];
	}

	vector<Thread*>().swap(_threads);
}

void ThreadPool::Start()
{
	_started = true;
	for (int i = 0; i < _numThreads; ++i)
	{
		string name = "EventLoopThread" + to_string(i + 1);
		Thread* t = new Thread(name, i + 1);
		//t->SetThreadFunc(&EventLoopThread::threadFunc);
		_threads.push_back(t);
		//_loops.push_back(loop);
		t->start();
	}
}

void ThreadPool::Start(function<void(void*)> CallBack)
{
	/*if (func == nullptr)
		return;*/
	funcCallBack = CallBack;
	Start();
}

void ThreadPool::AddTask(void * para, int len)
{
	for (int i = _next; i < _numThreads; ++i)
	{
		Thread* pThread = _threads.at(i);
		
		_next++;
		_next = _next%_numThreads;
		if (!pThread->getStartedStatus())			//����߳�û�п�ʼ�������н���
		{
			ThreadTask* pTask = pThread->GetTask();
			if (pTask == NULL) return;
			pTask->setPara(para, len);
			pThread->Run();
			return;
		}
		
	}

	//����̶߳�æµ�Ļ����Կ��Ƕ�̬����
	string name = "EventLoopThread" + to_string(_numThreads);
	Thread* t = new Thread(name, _numThreads, true);
	t->start();
	ThreadTask* pTask = new ThreadTask(para, len);
	pTask->setTaskFuncCallBack(funcCallBack);
	t->SetTask(pTask);
	cout << "extraThread" << endl;
}


