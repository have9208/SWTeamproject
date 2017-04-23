#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "serverNetwork.h"
#include "print.h"

void serverSocket(SocketInfo *sockInfo)
{
    int sockId, addrLen = sizeof(struct sockaddr);
    //if((sockId = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    if((sockId = socket(PF_INET, SOCK_STREAM, 0)) < 0)
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
        printError("bind fail");
        exit(0);
    }

    printNotice("Socket bind success.");

    if(listen(sockId, 10) == -1)
    {
        printError("listen fail");
        exit(0);
    }

    printNotice("Socket listen start.");

    sockInfo->sockId = sockId;
    sockInfo->addrLen = addrLen;
}

int receive(SocketInfo *sockInfo, char *buffer)
{
    //int nbyte = recvfrom(sockInfo->sockId, buffer, BLOCK_SIZE, 0, (struct sockaddr *)&(sockInfo->cliAddr), &(sockInfo->addrLen));
    int nbyte = recv(sockInfo->cliSockId, buffer, BLOCK_SIZE, 0);
    if(nbyte < 0)
    {
        //perror("recvfrom fail");
        printError("recv fail");
        exit(0);
    }

    return nbyte;
}

FileMetadata receiveFileMetadata(SocketInfo *sockInfo)
{
    FileMetadata fileMeta;
    //recvfrom(sockInfo->sockId, (char *)&fileMeta, sizeof(FileMetadata), 0, (struct sockaddr *)&(sockInfo->cliAddr), &(sockInfo->addrLen));
    recv(sockInfo->cliSockId, (char *)&fileMeta, sizeof(FileMetadata), 0);
    return fileMeta;
}