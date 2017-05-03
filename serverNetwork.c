#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "serverNetwork.h"
#include "print.h"

void serverSocket(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int sockId, addrLen = sizeof(struct sockaddr);
    
    if((sockId = socket(PF_INET, (sockInfo->protocol == UDP)? SOCK_DGRAM : SOCK_STREAM, 0)) < 0)
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

    sockInfo->sockId = sockId;
    sockInfo->addrLen = addrLen;

    dataInfo->type = META;

    if(sockInfo->protocol == TCP)
    {
        printNotice("TCP Socket bind success.");

        if(listen(sockId, 10) == -1)
        {
            printError("listen fail");
            exit(0);
        }

        printNotice("Socket listen start.");

    }
    else
    {
        printNotice("UDP Socket bind success.");
    }

}

int acceptComp(SocketInfo *sockInfo)
{
    if(sockInfo->protocol == TCP)
    {
        if( (sockInfo->cliSockId = accept(sockInfo->sockId, (struct sockaddr *)&(sockInfo->cliAddr), &(sockInfo->addrLen))) == -1 )
        {
            printError("Accept error");
            exit(0);
        }

        printNotice("Accept success");
        return 1;
    }

    return 1;
}

int recvComp(SocketInfo *sockInfo, char *buffer, int size)
{
    int nbyte;
    if(sockInfo->protocol == UDP)
    {
        nbyte = recvfrom(sockInfo->sockId, buffer, size, 0, (struct sockaddr *)&(sockInfo->cliAddr), &(sockInfo->addrLen));
    }
    else
    {
        nbyte = recv(sockInfo->cliSockId, buffer, size, 0);

        if(nbyte == 0)
        {
            close(sockInfo->cliSockId);
        }
    }

    return nbyte;
}

int sendComp(SocketInfo *sockInfo, char *buffer, int size)
{
    if(sockInfo->protocol == UDP)
    {
        return sendto(sockInfo->sockId, buffer, size, 0, (struct sockaddr*)&(sockInfo->cliAddr), sizeof(sockInfo->addrLen));
    }
    else
    {
        return send(sockInfo->cliSockId, buffer, size, 0);
    }
}

void sendIntegrity(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    char boolean = (char)(memcmp(dataInfo->servHash, dataInfo->cliHash, HASH_SIZE) == 0);

    if(boolean == 0)
    {
        printNotice("integrity fail."); 
    }
    else
    {
        printNotice("integrity success."); 
    }

    dataInfo->type = META;
    sendComp(sockInfo, &boolean, 1);
}

int receiveData(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int remainterSize = dataInfo->fileMeta.size - dataInfo->currentSize;
    int size = (remainterSize >= BLOCK_SIZE)? BLOCK_SIZE : remainterSize;
    int nbyte = recvComp(sockInfo, dataInfo->buffer, size);

    if(nbyte < 0)
    {
        printError("recv fail");
        exit(0);
    }

    dataInfo->size = nbyte;
    dataInfo->currentSize += nbyte;

    printNotice("load data");
    
    if( (remainterSize -= nbyte) <= 0 )
    {
        dataInfo->type = INTE;
        printNotice("end load file");
    }

    return nbyte;
}

int receiveMeta(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int nbyte = recvComp(sockInfo, (char *)&(dataInfo->fileMeta), sizeof(dataInfo->fileMeta));
    dataInfo->currentSize = 0;
    printNotice("fileMetaData load.");
    return nbyte;
}

int receiveHash(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int nbyte = recvComp(sockInfo, dataInfo->cliHash, HASH_SIZE);
    printNotice("load hash data.");
    return nbyte;
}

int receive(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int type = dataInfo->type;
    switch(type)
    {
        case META:
            return receiveMeta(sockInfo, dataInfo);
        case DATA:
            return receiveData(sockInfo, dataInfo);
        case INTE:
            return receiveHash(sockInfo, dataInfo);
    }

    return 0;
}