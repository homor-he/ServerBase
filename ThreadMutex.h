#ifndef __THREADMUTEX_H__
#define __THREADMUTEX_H__


#pragma once

#include <windows.h>

class CThreadMutex
{
public:
	CThreadMutex() {::InitializeCriticalSection(&_critSec);};
	~CThreadMutex() 
	{
		::EnterCriticalSection(&_critSec);
		::DeleteCriticalSection(&_critSec);
	}

public:
	void Lock() {::EnterCriticalSection(&_critSec);}
	void UnLock() { ::LeaveCriticalSection(&_critSec); }
	void TryLock() { TryEnterCriticalSection(&_critSec); }

private:
	CRITICAL_SECTION _critSec;
};


class MutexLock
{
public:
	explicit MutexLock(CThreadMutex & mutex) :_mutex(mutex) { _mutex.Lock(); }
	~MutexLock() { _mutex.UnLock(); }

private:
	CThreadMutex &_mutex;
};

#endif // !__THREADMUTEX_H__