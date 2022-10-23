#ifndef __TCPSOCKETBASE_H__
#define __TCPSOCKETBASE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "windows.h"
#include <iostream>
#include <string>
#include <sys/types.h>
//#include <sys/socket.h>
#include <winsock2.h>
#include <MSWSock.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

enum isOverLapped
{
	True,
	False,
};

class  TcpSocketBase
{
public:
	TcpSocketBase();
	 ~TcpSocketBase();
	 //int getListenFd() { return _sock; }
public:
	bool Socket(isOverLapped check);
	bool Close();
	bool Bind(const string& ip, short port);
	bool Listen(int num) const;
	bool Accept(TcpSocketBase* peer, string* ip = NULL, USHORT* port = NULL) const;
	bool Recv(string* buf) const;
	bool Send(char * buf, int bufLen) const;
	bool Connect(string& ip, short port);
	SOCKET GetFd() const;
	int  SetSocketNonBlocking(SOCKET fd);
private:
	SOCKET _sock;
	WSADATA wsa;
};




#endif

