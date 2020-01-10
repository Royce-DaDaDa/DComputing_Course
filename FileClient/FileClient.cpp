#include "CBlockingSocket.h"
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#define DEFAULT_BUFLEN 512

using namespace std;
DWORD WINAPI GetFileThread(LPVOID);
struct src {
	char ip[15];
	char port[5] ;
	int startpos ;
	char filename[20];
	char realname[20];
};

HANDLE writeMutex;

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	writeMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	HANDLE aThread[10];
	DWORD ThreadID;
	char filename[20] = {'\0'};
	printf("Please input the bttorrrent name:");
	scanf("%s", &filename);

	fstream reader;
	reader.open(filename, ios::in | ios::binary);
	if (!reader) {
		printf("File does not exist\n");
	}
	reader.seekg(0, ios::end);
	int length = reader.tellg();
	reader.seekg(0, ios::beg);
	//cout << length<<"\n";
	int row = (length / 45);
	//cout << size;
	char readbuff[1024] = {'\0'};
	reader.read(readbuff, length);
	int startpos = 0;
	int count = 0;
	char realname[20];
	memcpy(realname, readbuff, 20);
	for (int k = 0; k < 20; k++) {
		if (realname[k] == ' ') {
			realname[k] = '\0';
		}
	}

	char totallength[5];
	memcpy(totallength, readbuff + 20, 5);
	int totallen = atoi(totallength);
	fstream writer;
	writer.open(realname, ios::out | ios::ate);
	char *wbuff = (char *)malloc(totallen * sizeof(char));
	memset(wbuff, 'a', totallen);
	writer.write(wbuff, totallen);
	writer.close();



	src *s=(src *)malloc(row*sizeof(src));
	for (int i = 0; i < row;i++) {

		memset(s[i].ip, '\0', 15);
		memset(s[i].port, '\0', 5);
		memset(s[i].filename, '\0', 20);
		char size[5];
		//cout << readbuff;
		memcpy(s[i].ip, readbuff+25+i*45 ,15);
		memcpy(s[i].port,readbuff +25+ 15 + i*45, 5);
		memcpy(s[i].filename, readbuff  +25+ 20 + i * 45,   20);
		memcpy(size, readbuff  +25+ 40 + i * 45,  5);
		s[i].startpos = startpos;
		int j;
		for (j = 0; j < 15; j++) {
			if (s[i].ip[j] == ' ') {
				s[i].ip[j] = '\0';
			}
		}
		for (j = 0; j < 5; j++) {
			if (s[i].port[j] == ' ') {
				s[i].port[j] = '\0';
			}
		}
		for (j = 0; j < 20; j++) {
			if (s[i].filename[j] == ' ') {
				s[i].filename[j] = '\0';
			}
		}
		memcpy(s[i].realname, realname, 20);

		aThread[count] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)GetFileThread,
			&s[i],       // no thread function arguments
			0,          // default creation flags
			&ThreadID); // receive thread identifier
		ResumeThread(aThread[count]);
		count++;
		startpos += atoi(size);
	}
	Sleep(1000);
	WaitForMultipleObjects(row, aThread, TRUE, INFINITE);
	reader.close();
	for (int i = 0; i < row; i++)
		CloseHandle(aThread[i]);

	CloseHandle(writeMutex);
}

DWORD WINAPI GetFileThread(LPVOID lp) {
	src *p = (src *)lp;
	CBlockingSocket bs;
	DWORD dwCount = 0, dwWaitResult;
	dwWaitResult = WaitForSingleObject(
		writeMutex,    // handle to mutex
		INFINITE);  // no time-out interval
	fstream writer;
	char buff[DEFAULT_BUFLEN] = { '\0' };
	int bytecount = 0;
	int bufflen = strlen(p->filename);
	switch (dwWaitResult)
	{
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:

		bs.Open(p->ip, p->port);
		
		if (bs.Send(p->filename, bufflen)) {
			printf("  Requesting file on the server:%s\n", p->filename);
		}


		writer.open(p->realname, ios :: binary | ios::out | ios::in );
		writer.seekp(p->startpos, ios::beg);

		while (true) {
			int res;
			res = bs.Recv(buff);
			if (res > 0) {
					// TODO: Write to the database
					printf("Thread %d writing to file...\n",
						GetCurrentThreadId());
					bytecount += strlen(buff);
					writer.write(buff, bytecount);

					// Release ownership of the mutex object

					// The thread got ownership of an abandoned mutex
					// The database is in an indeterminate state
			}
			else if (res == 0) {
				printf("\n  Received %d bytes\nConnection is down", bytecount);
				break;
			}
			else {
				printf("  recv failed with error: %d\n", WSAGetLastError());
				break;
			}
			memset(&buff, '\0', DEFAULT_BUFLEN);
		}
		writer.close();
		bs.Close();
		break;
	case WAIT_ABANDONED:
		printf("wait error\n");
		return FALSE;
	}
	ReleaseMutex(writeMutex);
	return 1;
}