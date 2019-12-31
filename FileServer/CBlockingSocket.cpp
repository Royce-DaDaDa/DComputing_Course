#include "CBlockingSocket.h"
#define DEFAULT_BUFLEN 512
CBlockingSocket::CBlockingSocket() {
	consocket = INVALID_SOCKET;
	listensocket = INVALID_SOCKET;
}
bool CBlockingSocket::Open(const char* ip, const char* port) {
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;
	// Resolve the server address and port
	iResult = getaddrinfo(ip, port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		consocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (consocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(consocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(consocket);
			consocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	printf("Connection established to remote server at %s:%s \n", ip, port);
	freeaddrinfo(result);
	return true;
}
bool CBlockingSocket::Close() {
	closesocket(consocket);
	return true;
}

bool CBlockingSocket::Listen(SOCKADDR* server,int size) {
	listensocket = socket(AF_INET, SOCK_STREAM, 0);
	int iResult;
	iResult = bind(listensocket, server, size);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(listensocket);
		WSACleanup();
		return false;
	}

	iResult = listen(listensocket, 3);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listensocket);
		WSACleanup();
		return false;
	}
	return true;
}

bool CBlockingSocket::Accept() {
	SOCKADDR_IN ClientAddr;
	int len = sizeof(ClientAddr);
	consocket = accept(listensocket, (SOCKADDR *)&ClientAddr, &len);

	if (consocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(listensocket);
		WSACleanup();
		return false;
	}
	printf("Accepted connection from %s\n", inet_ntoa(ClientAddr.sin_addr));
	return true;
}

bool CBlockingSocket::Send(char *buff, int bufflen) {
	int iResult;
	iResult = send(consocket, buff, bufflen, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(consocket);
		WSACleanup();
		return false;
	}
	return true;
}

int CBlockingSocket::Recv(char *buff) {
	int iResult;
	iResult=recv(consocket, buff, DEFAULT_BUFLEN, 0);
	return iResult;
}