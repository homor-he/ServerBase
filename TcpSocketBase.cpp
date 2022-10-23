#include "TcpSocketBase.h"


TcpSocketBase::TcpSocketBase()
{
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
	{
		return;
	}

	_sock = INVALID_SOCKET;
}

TcpSocketBase::~TcpSocketBase()
{
	WSACleanup();
}

bool TcpSocketBase::Socket(isOverLapped check)
{
	if (check == isOverLapped::True)
	{
		_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}
	else
	{
		_sock = socket(AF_INET, SOCK_STREAM, 0);
	}
	
	if (_sock < 0)
	{
		cout << "创建套接字错误";
		return false;
	}
	cout << "创建套接字成功" << _sock << endl;
	return true;
}

bool TcpSocketBase::Close() 
{
	closesocket(_sock);
	_sock = INVALID_SOCKET;
	cout << "关闭套接字成功" << _sock << endl << endl;
	return true;
}

bool TcpSocketBase::Bind(const string & ip, short port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	int ret = bind(_sock, (sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		cout << "bind fail" << endl;
		return false;
	}
	cout << "bind success" << endl;
	return true;
}

bool TcpSocketBase::Listen(int num) const
{
	int ret = listen(_sock, num);
	if (ret < 0)
	{
		cout << "listen fail" << endl;
		return false;
	}
	cout << "listen success" << endl;
	return true;
}

bool TcpSocketBase::Accept(TcpSocketBase * peer, string * ip, USHORT * port) const
{
	sockaddr_in client_addr;
	int addrLen = sizeof(client_addr);
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = WSAAccept(_sock, (sockaddr*)&client_addr, &addrLen, NULL, NULL);
	//clientSocket = accept(_sock, (sockaddr*)&client_addr, &addrLen);
	if (NULL == clientSocket)
	{
		cout << "accept fail" << endl;
		return false;
	}
	cout << "accept success" << endl;
	peer->_sock = clientSocket;
	if (NULL != ip)
	{

		*ip = inet_ntoa(client_addr.sin_addr);
	}
	if (NULL != port)
	{
		
		*port = ntohs(client_addr.sin_port);
		
	}
	return true;
}

bool TcpSocketBase::Recv(string * buf) const
{
	buf->clear();
	char tmp[1024 * 10] = { 0 };
	int read_size = recv(_sock, tmp, sizeof(tmp), 0);
	if (read_size < 0)
	{
		cout << "recv fail" << endl;
		return false;
	}

	if (read_size == 0)
	{
		cout << "recv null" << endl;
		return false;
	}

	buf->assign(tmp, read_size);
	return true;
}

bool TcpSocketBase::Send(char * buf, int bufLen) const
{
	int len = send(_sock, buf, bufLen, 0);
	if (len < 0)
	{
		cout << "send fail" << endl;
		return false;
	}
	cout << "send success" << endl;
	return true;
}

bool TcpSocketBase::Connect(string & ip, short port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);
	int ret = connect(_sock, (sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		cout << "connect fail" << endl;
		return false;
	}
	cout << "connect success" << endl;
	return true;
}

SOCKET TcpSocketBase::GetFd() const
{
	return _sock;
}

int TcpSocketBase::SetSocketNonBlocking(SOCKET fd)
{
	int ul = 1;
	return ioctlsocket(_sock,FIONBIO,(unsigned long*)&ul);
}
