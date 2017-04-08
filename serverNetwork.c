#include "serverNetwork.h"

struct sockaddr_in servAddr, cliAddr;
int sockId, addrLen = sizeof(struct sockaddr);
char buf[BLOCK_SIZE+1];

int serverSocket()
{
	if(sockId = socket(PF_INET, SOCK_DGRAM, 0) < 0)
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

char* receive(char *buffer, int *size)
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