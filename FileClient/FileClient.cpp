#include "CBlockingSocket.h"
#include<stdio.h>
#define DEFAULT_BUFLEN 512
int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	int iResult;
	if (argc != 3) {
		printf("usage: %s wrong\n", argv[0]);
		return 1;
	}
	char * ip = argv[1];
	char * port = argv[2];
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	char filename[20] = {'\0'};
	printf("Please input the filename:");
	scanf("%s", &filename);
	int choice;
	printf("1.Save the file to disk    2.Print out on the console \nYour choice:");
	scanf("%d", &choice);
	if (choice != 1 && choice != 2) {
		printf("Bad input!");
		return 1;
	}
	CBlockingSocket bs;
	bs.Open(ip, port);
	int bufflen = strlen(filename);
	if (bs.Send(filename, bufflen)) {
		printf("  Requesting file on the server:%s\n", filename);
	}
	FILE *write;
	write = fopen(filename, "w");
	char buff[DEFAULT_BUFLEN] = { '\0' };
	int bytecount = 0;
	int flag = 0,flag2=0;
	while (true) {
		int res;
		res = bs.Recv(buff);
		if (res > 0) {
			if (flag == 0) {
				printf("  Receiving file %s\n", filename);
				if (choice == 2) {
					printf("  ====================\n");
				}
				flag = 1;
			}
			if (choice == 1) {
				fputs(buff, write);
			}
			else if (choice == 2) {
				printf("%s", buff);
			}
			bytecount += strlen(buff);
		}
		else if (res == 0) {
			if (choice == 2) {
				printf("\n  ====================");
			}
			printf("\n  Received %d bytes\nConnection is down", bytecount);
			break;
		}
		else {
			printf("  recv failed with error: %d\n", WSAGetLastError());
		}
		memset(&buff, '\0', DEFAULT_BUFLEN);
	}
	if (choice == 1) {
		fclose(write);
	}

	bs.Close();
}