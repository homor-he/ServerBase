#include "HttpMsgHandler.h"

HttpMsgHandler::HttpMsgHandler()
{
	m_method = HttpMethod::UNDEFINED;
}

HttpMsgHandler::~HttpMsgHandler()
{
}

void HttpMsgHandler::OnNetHttpMsg(string & res, string & ackMsg)
{
	ParseURI(res);
	ackMsg = "HTTP/1.0 200 OK\n";
	/*string body = "<html>\
				  <head>\
				  <meta charset = \"gbk\">\
				  </head>\
				  <body>\
				  <p style = \"background-color:Red\">Hello World</p>\
				  </body>\
				  </html>\n";*/
	string body = "<html><div><h1>hello world</h1></div></html\n";
	string header = "Content-Type: text/html\ncharset: gbk\nContent-Length:"
		+ to_string(body.size()) + "\n\n";
	ackMsg = ackMsg + header + body;    
}

URIState HttpMsgHandler::ParseURI(string & res)
{
	int posGet = res.find("GET");
	int posPost = res.find("POST");
	int posHead = res.find("HEAD");

	size_t pos = 0;
	if (posGet >= 0)
	{
		pos = posGet;
		m_method = HttpMethod::GET;
	}
	else if (posPost >= 0)
	{
		pos = posPost;
		m_method = HttpMethod::POST;
	}
	else if (posHead >= 0)
	{
		pos = posHead;
		m_method = HttpMethod::HEAD;
	}
	else
	{
		return URIState::PARSE_URI_ERROR;
	}

	pos = res.find("/", pos);
	if (pos < 0)
	{
		return URIState::PARSE_URI_ERROR;
	}
	else
	{
		size_t pos2 = res.find(' ',pos);
		if (pos2 < 0)
		{
			return URIState::PARSE_URI_ERROR;
		}
		else if (pos2 - pos > 1)
		{
			cout << "URI have parameter!" << endl;
			string para = res.substr(pos + 1, pos2 - pos - 1);
			cout << "Parameter is " << para << endl;
			return URIState::PARSE_URI_SUCCESS;
		}
		else
		{
			cout << "URI have no parameter!" << endl;
			return URIState::PARSE_URI_ERROR;
		}
	}
}
