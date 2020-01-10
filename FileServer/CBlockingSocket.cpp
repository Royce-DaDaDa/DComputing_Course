#include "CBlockingSocket.h"

CBlockingSocket::CBlockingSocket()
{
	this->m_socket = INVALID_SOCKET;
	this->info = NULL;
}

CBlockingSocket::CBlockingSocket(SOCKET socket)
{
	this->m_socket = socket;
	this->info = NULL;
}

CBlockingSocket::~CBlockingSocket(void)
{
}


BOOL CBlockingSocket::Open(const char *ip, const char *port)
{
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (ip == NULL)
	{
		hints.ai_flags = AI_PASSIVE;
	}

	if (getaddrinfo(ip, port, &hints, &this->info) == 0)
	{
		this->m_socket = socket(this->info->ai_family, this->info->ai_socktype, this->info->ai_protocol);
		if (this->m_socket == INVALID_SOCKET)
		{
			cout << "Open Socket Failed!" << endl;
			return FALSE;
		}
	}
	else {
		cout << "GetAddrInfo Failed!" << endl;
		return FALSE;
	}
	return TRUE;
}

BOOL CBlockingSocket::Listen(const char *port)
{
	if (bind(this->m_socket, this->info->ai_addr, (int)this->info->ai_addrlen) == 0)
	{
		if (listen(this->m_socket, 5) == 0)
		{
			cout << "File Server is listening on port: " << port << endl;
			return TRUE;
		}
		else
		{
			cout << "Server Listen Failed!" << endl;
			return FALSE;
		}
	}
	else
	{
		cout << "Bind Socket Failed!" << endl;
		return FALSE;
	}
}


CBlockingSocket CBlockingSocket::Accept(struct sockaddr &their_addr)
{
	int addr_size = sizeof(their_addr);
	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;

	fd_set fdread;
	int ret;
	FD_ZERO(&fdread);
	FD_SET(m_socket, &fdread);

	if ((ret = select(0,//
		&fdread, //指针，指向一组等待可读性检查的套接口
		NULL,    //指针，指向一组等待可写性检查的套接口
		NULL,    //指针，指向一组等待错误检查的套接口
		NULL))   //select()最多等待时间
		== SOCKET_ERROR)
	{
		return INVALID_SOCKET;
	}

	if (ret > 0)
	{
		if (FD_ISSET(m_socket, &fdread))
		{
			ClientSocket = accept(m_socket, &their_addr, &addr_size);
			if (ClientSocket == INVALID_SOCKET) {
				printf("accept failed: %d\n", WSAGetLastError());
				return INVALID_SOCKET;
			}

			return CBlockingSocket(ClientSocket);
		}
	}

	return INVALID_SOCKET;
}

BOOL CBlockingSocket::Connect()
{
	if (connect(this->m_socket, this->info->ai_addr, (int)this->info->ai_addrlen) != SOCKET_ERROR)
	{
		return TRUE;
	}
	else
	{
		cout << "Socket Connect Failed!" << endl;
		return FALSE;
	}
}

INT CBlockingSocket::Send(const char *s, int count)
{
	int sendLen = send(this->m_socket, s, count, 0);
	if (sendLen == SOCKET_ERROR)
	{
		cout << "Send Failed!" << endl;
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

INT CBlockingSocket::Recv(char *recvbuf, int recvbuflen)
{
	int recvLen = recv(this->m_socket, recvbuf, recvbuflen, 0);//在ClientSocket的m_socket上
	if (recvLen == SOCKET_ERROR || recvLen == 0)
	{
		return -1;
	}
	else
	{
		return recvLen;
	}
}

void CBlockingSocket::Close()
{
	if (closesocket(this->m_socket) == 0)
	{
		cout << "Connection is down!" << endl;
	}
	else
	{
		cout << "Connection Close Failed!" << endl;
	}
}