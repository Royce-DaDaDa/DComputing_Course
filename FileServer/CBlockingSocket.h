#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

//为了实现服务器能够响应TCP多连接同时处理，需要使用select I/O模型，此类为基于select I/O 模型的Socket工具类
using namespace std;

class CBlockingSocket
{
private:
	SOCKET m_socket;
	struct addrinfo *info;
public:
	CBlockingSocket();
	CBlockingSocket(SOCKET socket);
	virtual ~CBlockingSocket();
	BOOL Open(const char *ip, const char *port);
	BOOL Listen(const char *port);
	//BOOL Accept(SOCKADDR_IN &addrClient, CBlockingSocket &csock);
	CBlockingSocket Accept(struct sockaddr &their_addr);
	BOOL Connect();
	INT Send(const char *sendbuf, int sendbuflen);
	INT Recv(char *recvbuf, int recvbuflen);
	void Close();
};