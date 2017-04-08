#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "serverNetwork.h"

char buf[BLOCK_SIZE+1];

void serverSocket(struct SocketInfo *sockInfo)
{
    int sockId, addrLen = sizeof(struct sockaddr);
    if((sockId = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket fail");
        exit(0);
    }

    memset(&(sockInfo->cliAddr), 0, addrLen);
    memset(&(sockInfo->servAddr), 0, addrLen);

    sockInfo->servAddr.sin_family = AF_INET;
    sockInfo->servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockInfo->servAddr.sin_port = htons(PORT);

    if(bind(sockId, (struct sockaddr *)&(sockInfo->servAddr), addrLen) < 0)
    {
        perror("bind fail");
        exit(0);
    }

    sockInfo->sockId = sockId;
    sockInfo->addrLen = addrLen;
}

char* receive(struct SocketInfo *sockInfo, char *buffer, int *size)
{
    int nbyte = recvfrom(sockInfo->sockId, buf, BLOCK_SIZE, 0, (struct sockaddr *)&(sockInfo->cliAddr), &(sockInfo->addrLen));
    
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