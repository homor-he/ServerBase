#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#pragma once


#include <iostream>
#include <vector>
#include "Thread.h"
#include <functional>


class ThreadPool
{
public:
	ThreadPool(int numThreads);
	virtual ~ThreadPool();

	//���̳�taskʱ
	virtual void Start();
	//��������task��������ֱ��ʹ��addTaskֱ������
	void Start(function<void(void*)> CallBack);
	void AddTask(void * para, int len);

protected:
	//EventLoop * _baseLoop;
	bool _started;
	int _numThreads;
	int _next;
	vector<Thread*> _threads;
	//vector<EventLoop*>		 _loops;

	//��ͨ�����ص�
	//void (*funcCallBack)(void*);
	function<void(void*)> funcCallBack;
};

#endif // !__THREADPOOL_H__


