#include "CBlockingSocket.h"
#define DEFAULT_BUFLEN 512
#include <windows.h>
#include <stdio.h>

DWORD WINAPI SendFileThread(LPVOID);

struct params {
	CBlockingSocket bs;
	char recvbuff[512];
};

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	if (argc != 2) {
		printf("usage: %s wrong\n", argv[0]);
		return 1;
	}
	HANDLE aThread[10];
	DWORD ThreadID;
	int iSendResult;
	char sendbuff[DEFAULT_BUFLEN] = { '\0' };
	char recvbuff[DEFAULT_BUFLEN] = { '\0' };
	int bufflen = DEFAULT_BUFLEN;
	char* port = argv[1];
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	SOCKADDR_IN ServerAddr;  // 存放本地地址信息
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(atoi(argv[1]));
	char hostname[128];
	gethostname(hostname, sizeof(hostname));
	hostent *hostIp = gethostbyname(hostname);
	SOCKADDR_IN ListenAddr;
	memset(&ListenAddr, 0, sizeof(SOCKADDR_IN));
	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_addr = *(in_addr*)hostIp->h_addr_list[0];
	printf("Echo server is listening on %s:%s\n", inet_ntoa(*(struct in_addr*)*(hostIp->h_addr_list)), port);

	CBlockingSocket bs;
	bs.Listen((SOCKADDR *)&ServerAddr, sizeof(ServerAddr));
	int recvcount;
	int count = 0;
	while (true) {
		printf("Waiting for connection.....\n");
		bs.Accept();
		recvcount = bs.Recv(recvbuff);
		if (recvcount > 0) {
			printf("  File requested from the client: %s\n", recvbuff);
		}
		params pm;
		pm.bs.consocket = bs.consocket;
		memcpy(&pm.recvbuff, recvbuff, 512);
		aThread[count] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)SendFileThread,
			&pm,       // no thread function arguments
			0,          // default creation flags
			&ThreadID); // receive thread identifier

		if (aThread[count] == NULL)
		{
			printf("CreateThread error: %d\n", GetLastError());
			return 1;
		}
		count++;
		memset(&recvbuff, '\0', DEFAULT_BUFLEN);
	}
}



DWORD WINAPI SendFileThread(LPVOID lp) {
	params *pm = (params *)lp;
	char sendbuff[512];
	FILE *read;
	if ((read = fopen(pm->recvbuff, "r")) == NULL) {
		pm->bs.Close();
		printf("  %s does not exist\n", pm->recvbuff);
		printf("  Connection closed\n");
		return 0;
	}
	printf("  Sending file to client\n");
	while (fgets(sendbuff, DEFAULT_BUFLEN, read)) {
		pm->bs.Send(sendbuff, DEFAULT_BUFLEN);
		memset(&sendbuff, '\0', DEFAULT_BUFLEN);
	}
	pm->bs.Close();
	printf("  File sent to the client: %s\n", pm->recvbuff);
	printf("  Connection closed\n");
	memset(&pm->recvbuff, '\0', DEFAULT_BUFLEN);
	return 1;
}