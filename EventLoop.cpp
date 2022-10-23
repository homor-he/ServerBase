#include "EventLoop.h"
#include "TcpPthreadServer.h"

void HttpProcess(string& req, string* resp)
{
	string first_line = "HTTP/1.0 200 OK\n";
	//string body = "<html><div><h1>hello world</h1></div></html\n";
#if 0
	string body = "<html>\
                <!测试 test>\
				<head>\
				<meta charset = \"gbk\" />\
				<title>初学socket</title>\
				<script>\
				function myFirst()\
				{\
					document.getElementById(\"we\").innerHTML = \"new world\";\
					const http = new XMLHttpRequest();\
					const url = 'http://localhost:9090/jianzhengqiji';\
					http.open(\"POST\",url);\
					http.send();\
				}\
				</script>\
				</head>\
                <body bgcolor=\"White\">\
				<h1 id = \"we\">被替代的内容</h1>\
				<button onclick = \"myFirst()\">见证奇迹</button>\
                <div>\
                <br/>\
                <h1 align=\"center\">hehe</h1>\
                <hr/>\
                <br/>\
                <p style = \"background-color:Red\">Hello World</p>\
                <hr/>\
                <p style = \"front-family:arial;color:yellow;front-size:20px;\">HAHA.</p>\
                <a href=\"http://www.baidu.com\">baidu</a>\
                </div>\
                <img src=\"https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1584426731206&di=768722bf51af6b78b2978550f762b620&imgtype=0&src=http%3A%2F%2Fa3.att.hudong.com%2F68%2F61%2F300000839764127060614318218_950.jpg\" width=\"104\" height=\"142\" />\
                </body>\
                </html>\n";
#endif

#if 1
	string body = "<html>\
                <!测试 test>\
				<head>\
				<meta charset = \"gbk\" />\
				<title>初学socket</title>\
				<script>\
				function myFirst()\
				{\
					document.getElementById(\"we\").innerHTML = \"new world\";\
					const url = 'ws://192.168.105.79:8080';\
					var ws = new WebSocket(url);\
					ws.onopen = function()\
					{\
						console.log(\"open\");\
						ws.send(\"hello\");\
					}\
				}\
				</script>\
				</head>\
                <body bgcolor=\"White\">\
				<h1 id = \"we\">被替代的内容</h1>\
				<button onclick = \"myFirst()\">见证奇迹</button>\
                <div>\
                <br/>\
                <h1 align=\"center\">hehe</h1>\
                <hr/>\
                <br/>\
                <p style = \"background-color:Red\">Hello World</p>\
                <hr/>\
                <p style = \"front-family:arial;color:yellow;front-size:20px;\">HAHA.</p>\
                <a href=\"http://www.baidu.com\">baidu</a>\
                </div>\
                <img src=\"https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1584426731206&di=768722bf51af6b78b2978550f762b620&imgtype=0&src=http%3A%2F%2Fa3.att.hudong.com%2F68%2F61%2F300000839764127060614318218_950.jpg\" width=\"104\" height=\"142\" />\
                </body>\
                </html>\n";
#endif
	string header = "Content-Type: text/html\ncharset: gbk\nContent-Length:"
		+ to_string(body.size()) + "\n\n";
	*resp = first_line + header + body;
	//service firewalld stop 关闭防火墙 #centos 7
	//service firewalld status

}

EventLoop::EventLoop()
{
	pOverlapped = NULL;
	pSocketContext = NULL;
	dwBytesTransfered = 0;

	_looping = true;
}

EventLoop::~EventLoop()
{

}

void EventLoop::excute(void* para)
{
	while (!_quit)
	{
		if (NULL == _hIOCompletionPort) continue;

		
		//检查完成端口状态
		BOOL bReturn = GetQueuedCompletionStatus(_hIOCompletionPort, &dwBytesTransfered, (PULONG_PTR)&pSocketContext, &pOverlapped, INFINITE);

		//接收EXIT_CODE退出标志，则直接退出
		if ((DWORD)pSocketContext == NULL) {
			break;
		}

		if (bReturn == 0)
		{
			DWORD dwErr = GetLastError();
			if (HandleError(pSocketContext, dwErr) == false)
			{
				break;
			}
			continue;
		}
		else
		{
			PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);
			if ((dwBytesTransfered == 0) && (pIoContext->m_OpType == RECV_POSTED || pIoContext->m_OpType == SEND_POSTED))
			{
				/*char aa[256];
				sprintf_s(aa, "客户端 %s:%d 断开连接, uniqueID:%lld", inet_ntoa(pSocketContext->m_ClientAddr.sin_addr), ntohs(pSocketContext->m_ClientAddr.sin_port),pIoContext->m_sockAccept);
				cout << aa << endl;*/
				UINT64 uniqueID = pIoContext->m_sockAccept;
				gbs_TcpThreadServer->RemoveContext(pSocketContext, uniqueID);
			}
			else
			{
				switch (pIoContext->m_OpType)
				{
					case RECV_POSTED:
					{
						pIoContext->m_nTotalBytes = dwBytesTransfered;
						//从buff中取出数据
						//char recvContent[1024] = { 0 };
						//sprintf_s(recvContent, "收到的内容：%s", pIoContext->m_wsaBuf.buf);
						//cout << recvContent << endl;
						DoRecv(pIoContext, pSocketContext);
						//先判断里面有没有带参数，不带参数执行PostRecv
						//PostRecv(pIoContext, pSocketContext);
						//带参数执行PostWrite
						PostWrite(pIoContext);
						
						//pIoContext->ResetBuffer();
					}
					case SEND_POSTED:
					{
						pIoContext->m_nSendBytes += dwBytesTransfered;
						if (pIoContext->m_nSendBytes < pIoContext->m_nTotalBytes)
						{
							//数据没发送完则继续发送数据
							pIoContext->m_wsaBuf.buf = pIoContext->m_szBuffer + pIoContext->m_nSendBytes;
							pIoContext->m_wsaBuf.len = pIoContext->m_nTotalBytes - pIoContext->m_nSendBytes;
							PostWrite(pIoContext,1);
						}
						else
						{
							PostRecv(pIoContext, pSocketContext);
						}
					}

				}
			}
		}
	}
}

void EventLoop::setIOCP(HANDLE iocp)
{
	_hIOCompletionPort = iocp;
}

bool EventLoop::HandleError(PER_SOCKET_CONTEXT * pContext, const DWORD & dwErr)
{
	// 如果是超时了，就再继续等吧  
	if (dwErr == WAIT_TIMEOUT) {
		// 确认客户端是否还活着...
		if (IsSocketAlive(pContext->m_Socket) == 0) {
			cout << "检测到客户端异常退出！" << endl;
			gbs_TcpThreadServer->RemoveContext(pContext);
			return true;
		}
		else {
			cout << "网络操作超时！重试中..." << endl;
			return true;
		}

	}
	else if (dwErr == ERROR_NETNAME_DELETED) {
		// 可能是客户端异常退出了
		cout << "检测到客户端异常退出！" << endl;
		gbs_TcpThreadServer->RemoveContext(pContext);
		return true;
	}
	else {
		cout << "完成端口操作出现错误，线程退出。错误代码:" << dwErr << endl;
		return false;
	}
	return false;
}

bool EventLoop::IsSocketAlive(SOCKET s)
{
	int nBytesSend = send(s, "", 0, 0);
	if (-1 == nBytesSend)
	{
		return false;
	}
	return true;
}

//void EventLoop::setThreadFunc(ThreadFunc func)
//{
//	_threadfunc = func;
//}
bool EventLoop::PostRecv(PER_IO_CONTEXT * pIoContext, PER_SOCKET_CONTEXT* pSocketContext)
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
		cout << "错误码："<< WSAGetLastError() << endl;
		cout << "(PostRecv)投递WSARecv失败！\n" << endl;
		return false;
	}

	return true;
}

bool EventLoop::DoRecv(PER_IO_CONTEXT * pIoContext, PER_SOCKET_CONTEXT * pSocketContext)
{
	//输出接收的数据
	SOCKADDR_IN* ClientAddr = &pSocketContext->m_ClientAddr;
	char aa[4096];
	sprintf_s(aa, "收到  %s:%d\n信息：%s", inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port), pIoContext->m_wsaBuf.buf);
	cout << aa << endl;
	//发送数据
	/*pIoContext->m_nSendBytes = 100;
	pIoContext->m_nTotalBytes = pIoContext->m_nTotalBytes;
	pIoContext->m_wsaBuf.len = pIoContext->m_nTotalBytes;*/
	return true;
}

bool EventLoop::PostWrite(PER_IO_CONTEXT * pIoContext, int flag)
{
	if (pIoContext == NULL) return false;
	if (flag == 0)
	{
		//收到数据后，处理回发
		string s = "";
		s.assign(pIoContext->m_wsaBuf.buf/*, pIoContext->m_wsaBuf.len*/);
		string requestStr = "";
		HttpProcess(s, &requestStr);
		//HttpMsgHandler httpMsgHandler;
		//httpMsgHandler.OnNetHttpMsg(s, requestStr);
		pIoContext->m_nTotalBytes = requestStr.size();
		memcpy_s(pIoContext->m_wsaBuf.buf, pIoContext->m_wsaBuf.len, requestStr.c_str(), pIoContext->m_nTotalBytes);
	}
	else if(flag == 1)
	{
		//回发的数据一次性没有发完
	}
	else 
	{
		//服务器主动发起
	}
	
	//pIoContext->m_nSendBytes = 0;

	//pIoContext->m_wsaBuf.len = pIoContext->m_nTotalBytes - pIoContext->m_nSendBytes;
	
	//strcpy_s(pIoContext->m_wsaBuf.buf, requestStr.size() /*+ 1*/, requestStr.c_str());

	DWORD dwFlags = 0;
	DWORD dwSendNumBytes = 0;
	WSABUF *p_wbuf = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	pIoContext->m_OpType = SEND_POSTED;

	int nRet = WSASend(pIoContext->m_sockAccept, p_wbuf, 1, &dwSendNumBytes, dwFlags, p_ol, NULL);
	if ((nRet == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		cout << WSAGetLastError() << endl;
		cout << "投递WSARecv失败！" << endl;
		return false;
	}
	return true;
}
