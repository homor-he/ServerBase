#ifndef __HTTPMSGHANDLER_H__
#define __HTTPMSGHANDLER_H__

#pragma once

//#include <iostream>
#include <string>
#include <iostream>

using namespace std;

enum HttpMethod
{
	UNDEFINED,
	GET,
	POST,
	HEAD,
};

enum URIState
{
	PARSE_URI_SUCCESS,
	PARSE_URI_ERROR,
};

class HttpMsgHandler
{
public:
	HttpMsgHandler();
	~HttpMsgHandler();

public:
	void OnNetHttpMsg(string & res, string & ackMsg);
	
private:
	URIState ParseURI(string & res);
private:
	HttpMethod m_method;
};




#endif // !__HTTPMSGHANDLER_H__
