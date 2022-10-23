#ifndef __THREADCONDITION_H__
#define __THREADCONDITION_H__

#pragma once

#include <Windows.h>

class CThreadCondition
{
public:
	CThreadCondition() { _phEvent = CreateEvent(NULL,TRUE,FALSE,NULL); }
	~CThreadCondition() { if (NULL != _phEvent) { CloseHandle(_phEvent); } }

public:
	void Wait();
	void Signal();

private:
	HANDLE _phEvent;
};


#endif // !__THREADCONDITION_H__