#ifndef __TEMPLATETHREADPOOL_H__
#define __TEMPLATETHREADPOOL_H__

#pragma once


#include <iostream>
#include <vector>
#include "Thread.h"
#include <functional>

template <class T = ThreadTask>
class TThreadPool
{
public:
	TThreadPool(int numThreads);
	virtual ~TThreadPool();

	//��threadTask���̳�ʱ
	virtual void Start();
	//��������task��������ֱ��ʹ��addTaskֱ�����ã���ʱ����pool����ʱ������T= ThreadTask
	void AddTask(void * para, int len, function<void(void*)> CallBack = 0);

	int GetThreadNum();
	Thread* GetThread(int index);
protected:
	bool _started;
	int _numThreads;
	int _next;
	vector<Thread*> _threads;
	function<void(void*)> funcCallBack;
};

template <class T = ThreadTask>
TThreadPool<T>::TThreadPool(int numThreads)
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

template <class T>
TThreadPool<T>::~TThreadPool()
{
	for (size_t i = 0; i < _threads.size(); ++i)
	{
		delete _threads[i];
	}

	vector<Thread*>().swap(_threads);
}

template <class T>
void TThreadPool<T>::Start()
{		
	_started = true;
	for (int i = 0; i < _numThreads; ++i)
	{
		string name = "EventLoopThread" + to_string(i + 1);
		Thread* t = new Thread(name, i + 1);
		_threads.push_back(t);
		T *pTask = new T;
		t->SetTask(pTask);
		t->start();
	}
}

template <class T>
void TThreadPool<T>::AddTask(void * para, int len, function<void(void*)> CallBack)
{
	for (int i = _next; i < _numThreads; ++i)
	{
		Thread* pThread = _threads.at(i);
		_next++;
		_next = _next%_numThreads;
		if (!pThread->getStartedStatus())			//����߳�û�п�ʼ�������н���
		{
			T* pTask = (T*)pThread->GetTask();
			if (pTask == NULL) return;
			pTask->setPara(para, len);
			pTask->setTaskFuncCallBack(CallBack);
			cout << "threadID:" << pThread->getThreadID() << endl;
			pThread->Run();
			return;
		}
	}

	//����̶߳�æµ�Ļ����Կ��Ƕ�̬����
	string name = "EventLoopThread" + to_string(_numThreads);
	Thread* t = new Thread(name, _numThreads, true);
	t->start();
	T* pTask = new T(para, len);
	pTask->setTaskFuncCallBack(funcCallBack);
	t->SetTask(pTask);
	cout << "extraThread" << endl;
}

template<class T>
int TThreadPool<T>::GetThreadNum()
{
	return _threads.size();
}

template<class T>
Thread * TThreadPool<T>::GetThread(int index)
{
	return _threads[index];
}

#endif // !__THREADPOOL_H__
