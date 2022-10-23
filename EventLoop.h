#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__


#pragma once

#include "Thread.h"
#include <winnt.h>
#include <vector>
#include <iostream>
#include "ThreadTask.h"
#include "_PER_SOCKET_CONTEXT.h"
#include "HttpMsgHandler.h"
//#include "Channel.h"

using namespace std;

//typedef struct eventInfo
//{
//	eventInfo()
//	{
//		dwTransferId = 0;
//		dwRecvTypeID = 0;
//		hEvent = NULL;
//		pBufRecv = NULL;
//	};
//
//	int	dwTransferId;
//	int	dwRecvTypeID;
//	int	dwSerial;
//	HANDLE	hEvent;
//	void*	pBufRecv;
//}SYNCEVENTINFO, *PSYNCEVENTINFO;


class EventLoop : public ThreadTask
{
public:
	EventLoop();
	~EventLoop();
	typedef void(*Functor)();
	void excute(void* para);
	void runInLoop(Functor fc);
	void queueInLoop(Functor fc);
	void setIOCP(HANDLE iocp);
	bool HandleError(PER_SOCKET_CONTEXT *pContext, const DWORD& dwErr);
	bool IsSocketAlive(SOCKET s);
	//void setThreadFunc(ThreadFunc func);
	bool PostRecv(PER_IO_CONTEXT * pIoContext, PER_SOCKET_CONTEXT* pSocketContext);
	bool DoRecv(PER_IO_CONTEXT * pIoContext, PER_SOCKET_CONTEXT* pSocketContext);
	bool PostWrite(PER_IO_CONTEXT * pIoContext, int flag = 0);
private:
	bool _looping;
	vector<Functor> _pendingFunctors;
	//const int _threadID;
	//Channel* _pwakeupChannel;
	//eventInfo _eventInfo;

	OVERLAPPED *pOverlapped;
	PER_SOCKET_CONTEXT *pSocketContext;
	DWORD      dwBytesTransfered;
	HANDLE     _hIOCompletionPort;     //Íê³É¶Ë¿Ú
	//ThreadFunc _threadfunc;
};

#endif
