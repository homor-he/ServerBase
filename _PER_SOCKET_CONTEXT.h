#ifndef __PER_SOCKET_CONTEXT_H__
#define __PER_SOCKET_CONTEXT_H__

#pragma once

#include <winsock2.h>
#include <MSWSock.h>
#include <vector>
#include "PER_IO_CONTEXT.h"

//每个SOCKET对应的数据结构(调用GetQueuedCompletionStatus传入)：-
//SOCKET，该SOCKET对应的客户端地址，作用在该SOCKET操作集合(对应结构PER_IO_CONTEXT)；
typedef struct _PER_SOCKET_CONTEXT {
	SOCKET      m_Socket;                                  //连接客户端的socket
	SOCKADDR_IN m_ClientAddr;                              //客户端地址
	vector<_PER_IO_CONTEXT*>	m_arrayIoContext;		//套接字操作，本例是WSARecv和WSASend共用一个PER_IO_CONTEXT

														//构造函数
	_PER_SOCKET_CONTEXT() {
		m_Socket = INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
	}

	//析构函数
	~_PER_SOCKET_CONTEXT() {
		if (m_Socket != INVALID_SOCKET) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		// 释放掉所有的IO上下文数据
		for (DWORD i = 0; i<m_arrayIoContext.size(); i++) {
			m_arrayIoContext[i]->Release();
			m_arrayIoContext[i] = NULL;
		}
		//m_arrayIoContext.clear();
		vector<_PER_IO_CONTEXT*>().swap(m_arrayIoContext);
	}

	//进行套接字操作时，调用此函数返回PER_IO_CONTEX结构
	_PER_IO_CONTEXT* GetNewIoContext() {
		_PER_IO_CONTEXT* p = new _PER_IO_CONTEXT;
		m_arrayIoContext.push_back(p);
		p->AddRef();
		return p;
	}

	// 从数组中移除一个指定的IoContext
	void RemoveContext(_PER_IO_CONTEXT* pContext) {
		if (pContext == NULL) return;

		vector<_PER_IO_CONTEXT*>::iterator it = m_arrayIoContext.begin();
		while (it != m_arrayIoContext.end()) {
			_PER_IO_CONTEXT* p_obj = *it;
			if (pContext == p_obj) {
				DWORD dwRef = pContext->Release();
				if(dwRef == 0)
					it = m_arrayIoContext.erase(it);
				break;
			}

			it++;
		}
	}

	DWORD RemoveAllContext()
	{
		DWORD restCount = 0;
		// 释放掉所有的IO上下文数据
		for (DWORD i = 0; i < m_arrayIoContext.size(); i++) 
		{
			DWORD dwRef = m_arrayIoContext[i]->Release();
			if (dwRef != 0)
				restCount++;
			else
				m_arrayIoContext[i] = NULL;
		}
		return restCount;
	}

} PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;


#endif // !__PER_SOCKET_CONTEXT_H__
