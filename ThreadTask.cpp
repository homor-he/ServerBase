#include "ThreadTask.h"

ThreadTask::ThreadTask()
{
	_quit = false;
	memset(buffer, 0, BS_BUFFER_LEN);
}

ThreadTask::ThreadTask(void * para, int len)
{
	_quit = false;
	memset(buffer, 0, BS_BUFFER_LEN);
	memcpy_s(buffer, BS_BUFFER_LEN, para, len);
}

ThreadTask::~ThreadTask()
{
}

void ThreadTask::excute(void* para)
{
	if(buffer != NULL && m_callback != NULL)
	{
		m_callback((void*)buffer);
	}
}

void ThreadTask::quit()
{
	_quit = true;
}

void ThreadTask::setTaskFuncCallBack(function<void(void*)> callback)
{
	m_callback = callback;
}

void ThreadTask::setPara(void * para, int len)
{
	memset(buffer, 0, BS_BUFFER_LEN);
	memcpy_s(buffer, BS_BUFFER_LEN, para, len);
}

