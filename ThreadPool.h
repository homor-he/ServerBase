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

	//当继承task时
	virtual void Start();
	//用于设置task方法，并直接使用addTask直接启用
	void Start(function<void(void*)> CallBack);
	void AddTask(void * para, int len);

protected:
	//EventLoop * _baseLoop;
	bool _started;
	int _numThreads;
	int _next;
	vector<Thread*> _threads;
	//vector<EventLoop*>		 _loops;

	//普通方法回调
	//void (*funcCallBack)(void*);
	function<void(void*)> funcCallBack;
};

#endif // !__THREADPOOL_H__


