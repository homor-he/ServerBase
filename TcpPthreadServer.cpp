#include "TcpPthreadServer.h"

TcpPthreadServer* gbs_TcpThreadServer = NULL;

TcpPthreadServer::TcpPthreadServer(const string & ip, short port):_ip(ip),_port(port)
{
	gbs_TcpThreadServer = this;
	
}

bool TcpPthreadServer::Start(Handler handler)
{
	listen_sock.Socket(isOverLapped::False);

	listen_sock.Bind(_ip, _port);

	listen_sock.Listen(5);

	while (true)
	{
		Arg* arg = new Arg;
		arg->handler = handler;
		if (!listen_sock.Accept(&arg->new_sock, &arg->ip, &arg->port))
		{
			continue;
		}
		cout << "[" << arg->ip << ":" << arg->port << "]" << "�ͻ���������" << endl;

		thread newThread(ThreadEntry,arg);
		newThread.join();
		//newThread.detach();
	}
	return true;
}

void* TcpPthreadServer::ThreadEntry(void * arg)
{
	Arg* target = (Arg*)arg;
	while (true)
	{
		string req;
		bool ret = target->new_sock.Recv(&req);
		if (!ret)
		{
			cout << "[" << target->ip << ":" << target->port << "]" << "�ͻ����ѶϿ�����" << endl;
			break;
		}
		cout << "[" << target->ip << ":" << target->port << "]" << endl << "req:" << endl << req << endl;

		string resp;
		target->handler(req, &resp);

		target->new_sock.Send((char*)resp.c_str(),resp.size());
		cout << "resp:" << endl << resp << endl;
	}
	target->new_sock.Close();
	target = NULL;
	return NULL;
}

TcpSocketBase TcpPthreadServer::GetTcpSocketObj()
{
	return 	listen_sock;
}

/////////////////////////////����

TcpPthreadServer::TcpPthreadServer(int threadNum, const string & ip, short port,HANDLE iocp)
{
	gbs_TcpThreadServer = this;
	::InitializeCriticalSection(&m_csContextList);
	_threadNum = threadNum;
	_pool = new TThreadPool<EventLoop>(threadNum);
	_started = false;
	_ip = ip;
	_port = port;
	_hIOCompletionPort = iocp;
	_pListenContext = new PER_SOCKET_CONTEXT;

	if (port < 0 || port>65535) _listenFd = -1;                 //���׽���
	listen_sock.Socket(isOverLapped::True);
	//_baseloop->setIOCP(iocp);									//����ѭ�������ɶ˿ھ��

	if (!CreateIoCompletionPort((HANDLE)listen_sock.GetFd(), _hIOCompletionPort, (DWORD)_pListenContext, 0))
	{
		cout << "�� Listen Socket����ɶ˿�ʧ�ܣ�" << endl;
	}
	listen_sock.Bind(_ip, _port);
	listen_sock.Listen(SOMAXCONN);
	if (listen_sock.GetFd() < 0)
	{
		_listenFd = -1;
	}
	else
	{
		_listenFd = listen_sock.GetFd();
	}
	
}

TcpPthreadServer::~TcpPthreadServer()
{
	delete _pool;
	_pool = NULL;

	delete _pListenContext;
	_pListenContext = NULL;

	ClearContextList();
	::DeleteCriticalSection(&m_csContextList);

	CloseHandle(_hIOCompletionPort);

	listen_sock.Close();     //�ر��׽���
}

bool TcpPthreadServer::HandleNewConn()
{
	Arg arg;

    while (listen_sock.Accept(&arg.new_sock, &arg.ip, &arg.port))
	{
		//EventLoop * loop = _pool->getNextLoop();
		cout << "New connection from " << arg.ip << ":" << arg.port << endl;
		
		//����socket��������ʽaccept
		int ul = 1;
		if (ioctlsocket(arg.new_sock.GetFd(), FIONBIO, (unsigned long*)&ul) == SOCKET_ERROR)
		{
			perror("set socket non block fail");
			abort();
		}

		//���������socket����ɶ˿ڰ�
		PER_SOCKET_CONTEXT* pNewSocketContext = new PER_SOCKET_CONTEXT;
		SOCKADDR_IN clientAddr;
		int len = sizeof(clientAddr);

		getpeername(arg.new_sock.GetFd(), (sockaddr*)&clientAddr, &len);
		pNewSocketContext->m_Socket = arg.new_sock.GetFd();
		memcpy(&(pNewSocketContext->m_ClientAddr), &clientAddr, len);

		HANDLE hTemp = CreateIoCompletionPort((HANDLE)pNewSocketContext->m_Socket, _hIOCompletionPort, (ULONG_PTR)pNewSocketContext, 0);
		if (hTemp == NULL)
		{
			cout << "ִ��CreateIoCompletionPort()���ִ���.������롣�������:" << GetLastError() << endl;
			return false;
		}

		//�����socket��Ͷ�ݵ�һ��WSARecv����
		PER_IO_CONTEXT* pNewIoContext = pNewSocketContext->GetNewIoContext();
		//pNewIoContext->m_OpType = SEND_POSTED;

		if (PostRecv(pNewIoContext,pNewSocketContext) == false)
		{
			pNewSocketContext->RemoveContext(pNewIoContext);
			delete pNewSocketContext;
			pNewSocketContext = NULL;
			return false;
		}

		::EnterCriticalSection(&m_csContextList);
		m_arrayClientContext.push_back(pNewSocketContext);
		::LeaveCriticalSection(&m_csContextList);
	}
	return true;
}

void TcpPthreadServer::Start()
{
	_pool->Start();

	for (int i = 0; i < _pool->GetThreadNum(); ++i)
	{
		EventLoop* pTask = (EventLoop*)_pool->GetThread(i)->GetTask();       //��eventloop����õ���ɶ˿ھ��
		pTask->setIOCP(_hIOCompletionPort);
	}

	HandleNewConn();
}

bool TcpPthreadServer::PostRecv(PER_IO_CONTEXT * pIoContext, PER_SOCKET_CONTEXT* pSocketContext)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	pIoContext->ResetBuffer();
	pIoContext->m_OpType = RECV_POSTED;
	pIoContext->m_sockAccept = pSocketContext->m_Socket;
	pIoContext->m_nSendBytes = 0;
	pIoContext->m_nTotalBytes = 0;

	int nBytesRecv = WSARecv(pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL);

	if ((nBytesRecv == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		cout << WSAGetLastError() << endl;
		cout << "Ͷ��WSARecvʧ�ܣ�" << endl;
		return false;
	}

	return true;
}

void TcpPthreadServer::RemoveContext(PER_SOCKET_CONTEXT * pSocketContext,UINT64 unqiueID)
{
	::EnterCriticalSection(&m_csContextList);
	vector<PER_SOCKET_CONTEXT*>::iterator it = m_arrayClientContext.begin();
	while (it != m_arrayClientContext.end())
	{
		if (pSocketContext == *it)
		{
			if (pSocketContext->RemoveAllContext() == 0)
			{
				char aa[256];
				sprintf_s(aa, "�ͻ��� %s:%d �Ͽ�����, uniqueID:%lld", inet_ntoa(pSocketContext->m_ClientAddr.sin_addr), ntohs(pSocketContext->m_ClientAddr.sin_port), unqiueID);
				cout << aa << endl;
				delete pSocketContext;
				pSocketContext = NULL;
				it = m_arrayClientContext.erase(it);
			}
			break;
		}
		it++;
	}
	::LeaveCriticalSection(&m_csContextList);
}

void TcpPthreadServer::ClearContextList()
{
	::EnterCriticalSection(&m_csContextList);

	for (DWORD i = 0; i<m_arrayClientContext.size(); i++) {
		delete m_arrayClientContext.at(i);
	}
	m_arrayClientContext.clear();

	::LeaveCriticalSection(&m_csContextList);
}
