#ifndef __PER_SOCKET_CONTEXT_H__
#define __PER_SOCKET_CONTEXT_H__

#pragma once

#include <winsock2.h>
#include <MSWSock.h>
#include <vector>

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
			delete m_arrayIoContext.at(i);
		}
		m_arrayIoContext.clear();
	}

	//�����׽��ֲ���ʱ�����ô˺�������PER_IO_CONTEX�ṹ
	_PER_IO_CONTEXT* GetNewIoContext() {
		_PER_IO_CONTEXT* p = new _PER_IO_CONTEXT;
		m_arrayIoContext.push_back(p);

		return p;
	}

	// ���������Ƴ�һ��ָ����IoContext
	void RemoveContext(_PER_IO_CONTEXT* pContext) {
		if (pContext == NULL) return;

		vector<_PER_IO_CONTEXT*>::iterator it = m_arrayIoContext.begin();
		while (it != m_arrayIoContext.end()) {
			_PER_IO_CONTEXT* p_obj = *it;
			if (pContext == p_obj) {
				delete pContext;
				pContext = NULL;
				it = m_arrayIoContext.erase(it);
				break;
			}

			it++;
		}


	}

} PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;



#endif // !__PER_SOCKET_CONTEXT_H__