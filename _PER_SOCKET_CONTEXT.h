#ifndef __PER_SOCKET_CONTEXT_H__
#define __PER_SOCKET_CONTEXT_H__

#pragma once

#include <winsock2.h>
#include <MSWSock.h>
#include <vector>
#include "PER_IO_CONTEXT.h"

//ÿ��SOCKET��Ӧ�����ݽṹ(����GetQueuedCompletionStatus����)��-
//SOCKET����SOCKET��Ӧ�Ŀͻ��˵�ַ�������ڸ�SOCKET��������(��Ӧ�ṹPER_IO_CONTEXT)��
typedef struct _PER_SOCKET_CONTEXT {
	SOCKET      m_Socket;                                  //���ӿͻ��˵�socket
	SOCKADDR_IN m_ClientAddr;                              //�ͻ��˵�ַ
	vector<_PER_IO_CONTEXT*>	m_arrayIoContext;		//�׽��ֲ�����������WSARecv��WSASend����һ��PER_IO_CONTEXT

														//���캯��
	_PER_SOCKET_CONTEXT() {
		m_Socket = INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
	}

	//��������
	~_PER_SOCKET_CONTEXT() {
		if (m_Socket != INVALID_SOCKET) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		// �ͷŵ����е�IO����������
		for (DWORD i = 0; i<m_arrayIoContext.size(); i++) {
			m_arrayIoContext[i]->Release();
			m_arrayIoContext[i] = NULL;
		}
		//m_arrayIoContext.clear();
		vector<_PER_IO_CONTEXT*>().swap(m_arrayIoContext);
	}

	//�����׽��ֲ���ʱ�����ô˺�������PER_IO_CONTEX�ṹ
	_PER_IO_CONTEXT* GetNewIoContext() {
		_PER_IO_CONTEXT* p = new _PER_IO_CONTEXT;
		m_arrayIoContext.push_back(p);
		p->AddRef();
		return p;
	}

	// ���������Ƴ�һ��ָ����IoContext
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
		// �ͷŵ����е�IO����������
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
