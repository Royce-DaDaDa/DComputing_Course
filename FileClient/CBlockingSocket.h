#pragma once
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class CBlockingSocket {
public:
	SOCKET consocket;
	SOCKET listensocket;
	CBlockingSocket();
	bool Open(const char* ip, const char* port);
	bool Close();
	bool Listen(SOCKADDR *server);
	bool Accept();
	bool Send(char *buff, int bufflen);
	int Recv(char *buff);
};