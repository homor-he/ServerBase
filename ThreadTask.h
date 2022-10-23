#ifndef _THREADTASK_H__
#define _THREADTASK_H__

#pragma once

#include <iostream>
#include <functional>

using namespace std;

#define BS_BUFFER_LEN   2048

class ThreadTask
{
public:
	ThreadTask();
	ThreadTask(void* para, int len);
	virtual ~ThreadTask();
	virtual void excute(void* para);

	void quit();
	void setTaskFuncCallBack(function<void(void*)> callback);
	void setPara(void* para, int len);
protected:
	bool _quit;
	char buffer[BS_BUFFER_LEN];
	//普通方法回调
	//void(*funcCallBack)(void*);
	function<void(void*)> m_callback;
};

#endif // !_THREADTASK_H__


