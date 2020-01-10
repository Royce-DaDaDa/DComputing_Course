#include "CBlockingSocket.h"
#define DEFAULT_BUFLEN 512
#include <windows.h>
#include <stdio.h>

DWORD WINAPI SendFileThread(LPVOID);

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


	CBlockingSocket bs;
	bs.Open(NULL, argv[1]);
	bs.Listen(argv[1]);
	int recvcount;
	int count = 0;
	while (true) {
		printf("Waiting for connection.....\n");
		struct sockaddr their_addr;
		struct sockaddr_in their_addrin;
		CBlockingSocket cs = bs.Accept(their_addr);
		//bs.Close();
		//closesocket(bs.consocket);
		//bs.consocket = SOCKET_ERROR;
		aThread[count] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)SendFileThread,
			&cs,       // no thread function arguments
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
	CBlockingSocket*cs = (CBlockingSocket *)lp;
	CBlockingSocket ClientSocket = *cs;//用局部变量保存线程传进来的地址传递的参数，防止主线程中socket被改写。
	char sendbuff[512] = {'\0'};
	char recvbuff[DEFAULT_BUFLEN] = { '\0' };
	ClientSocket.Recv(recvbuff, sizeof(recvbuff));
	FILE *read;
	if ((read = fopen(recvbuff, "r")) == NULL) {
		ClientSocket.Close();
		printf("  %s does not exist\n", recvbuff);
		printf("  Connection closed\n");
		return 0;
	}
	printf("  Sending file to client\n");
	while (fgets(sendbuff, DEFAULT_BUFLEN, read)) {
		ClientSocket.Send(sendbuff, DEFAULT_BUFLEN);
		memset(&sendbuff, '\0', DEFAULT_BUFLEN);
	}
	ClientSocket.Close();
	printf("  File sent to the client: %s\n",recvbuff);
	printf("  Connection closed\n");
	memset(&recvbuff, '\0', DEFAULT_BUFLEN);
	return 1;
	//free(pm->bs.consocket);
}