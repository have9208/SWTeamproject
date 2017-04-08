#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "serverNetwork.h"

char buf[BLOCK_SIZE+1];
struct sockaddr_in servAddr, cliAddr;
int addrLen = sizeof(struct sockaddr);

int serverSocket()
{
	int sockId;
	if((sockId = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket fail");
		exit(0);
	}

	memset(&cliAddr, 0, addrLen);
	memset(&servAddr, 0, addrLen);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);

	if(bind(sockId, (struct sockaddr *)&servAddr, addrLen) < 0)
	{
		perror("bind fail");
		exit(0);
	}

	return sockId;
}

char* receive(int sockId, char *buffer, int *size)
{
	int nbyte = recvfrom(sockId, buf, BLOCK_SIZE, 0, (struct sockaddr *)&cliAddr, &addrLen);
	
	if(nbyte < 0)
	{
		perror("recvfrom fail");
		return NULL;
	}

	buf[nbyte] = 0;

	buffer = buf;
	*size = nbyte;

	return buf;
}