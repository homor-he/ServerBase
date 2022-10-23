#ifndef __PER_IO_CONTEXT_H__
#define __PER_IO_CONTEXT_H__

#pragma once

#include <winsock2.h>
#include <MSWSock.h>
#include <vector>

using namespace std;

//���������� (1024*8)
#define MAX_BUFFER_LEN        8192  

// ����ɶ˿���Ͷ�ݵ�I/O����������
typedef enum _OPERATION_TYPE {
	ACCEPT_POSTED,                     // ��־Ͷ�ݵ�Accept����
	SEND_POSTED,                       // ��־Ͷ�ݵ��Ƿ��Ͳ���
	RECV_POSTED,                       // ��־Ͷ�ݵ��ǽ��ղ���
	NULL_POSTED                        // ���ڳ�ʼ����������
}OPERATION_TYPE;


//ÿ���׽��ֲ���(�磺AcceptEx, WSARecv, WSASend��)��Ӧ�����ݽṹ��OVERLAPPED�ṹ(��ʶ���β���)���������׽��֣����������������ͣ�
typedef struct _PER_IO_CONTEXT {
	OVERLAPPED     m_Overlapped;                               // ÿһ���ص�����������ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)              
	SOCKET         m_sockAccept;                               // ������������ʹ�õ�Socket
	WSABUF         m_wsaBuf;                                   // WSA���͵Ļ����������ڸ��ص�������������
	char           m_szBuffer[MAX_BUFFER_LEN];                 // �����WSABUF�������ַ��Ļ�����
	OPERATION_TYPE m_OpType;                                   // ��ʶ�������������(��Ӧ�����ö��)
	DWORD			m_nTotalBytes;	//�����ܵ��ֽ���
	DWORD			m_nSendBytes;	//�Ѿ����͵��ֽ�������δ��������������Ϊ0

									//���캯��
	_PER_IO_CONTEXT() {
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType = NULL_POSTED;

		m_nTotalBytes = 0;
		m_nSendBytes = 0;
	}
	//��������
	~_PER_IO_CONTEXT() {
		if (m_sockAccept != INVALID_SOCKET) {
			closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
	}
	//���û���������
	void ResetBuffer() {
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
	}

	DWORD  AddRef()
	{
		return ::InterlockedIncrement(&m_dwRef);
	}

	DWORD Release()
	{
		if (::InterlockedDecrement(&m_dwRef)) 
			return m_dwRef;
		delete this;
		return 0;
	}
private:
	ULONG							m_dwRef;
} PER_IO_CONTEXT, *PPER_IO_CONTEXT;

#endif // !__PER_IO_CONTEXT_H__
