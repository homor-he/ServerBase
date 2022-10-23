#ifndef __TCPPTHREADSERVER_H__
#define __TCPPTHREADSERVER_H__

//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000

#include <thread>
#include <iostream>
#include <stdio.h>
#include <string>
#include "TcpSocketBase.h"
#include "TemplateThreadPool.h"
#include "EventLoop.h"
//#include "EventLoop.h"
//#include "EventLoopThreadPool.h"
//#include "Channel.h"
//using namespace std;

typedef void(_cdecl *Handler)(string& req, string* resp);

struct Arg 
{
	TcpSocketBase new_sock;
	string ip;
	USHORT port;
	Handler handler;
};


class TcpPthreadServer
{
public:
	TcpPthreadServer(const string& ip, short port);
	~TcpPthreadServer();

public:
	bool Start(Handler handler);
	static void* ThreadEntry(void *arg);
	TcpSocketBase GetTcpSocketObj();
private:
	TcpSocketBase listen_sock;
	string _ip;
	short _port;

///////////////////////////新增


public: 
	TcpPthreadServer(int threadNum, const string& ip, short port,HANDLE iocp);
	bool HandleNewConn();
	void Start();
	bool PostRecv(PER_IO_CONTEXT* pIoContext, PER_SOCKET_CONTEXT* pSocketContext);
	void RemoveContext(PER_SOCKET_CONTEXT * pSocketContext, UINT64 unqiueID = 0);
private:
	void ClearContextList();
private:
	int _threadNum;
	bool _started;
	//Channel* _acceptChannel;
	int _listenFd;

public:
	TThreadPool<EventLoop>* _pool;
	HANDLE _hIOCompletionPort;            //完成端口
	PER_SOCKET_CONTEXT* _pListenContext;  //数据结构
	CRITICAL_SECTION m_csContextList;	  //用于workek线程同步
	vector<PER_SOCKET_CONTEXT*> m_arrayClientContext;
};

extern TcpPthreadServer* gbs_TcpThreadServer;
#endif
