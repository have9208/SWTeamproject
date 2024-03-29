#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "serverNetwork.h"
#include "print.h"

void serverSocket(SocketInfo *sockInfo)
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

int acceptComp(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    printDelete("change META");
    dataInfo->type = META;

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
        return sendto(sockInfo->sockId, buffer, size, 0, (struct sockaddr*)&(sockInfo->cliAddr), sockInfo->addrLen);
    }
    else
    {
        return send(sockInfo->cliSockId, buffer, size, 0);
    }

}

void sendData(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    sendComp(sockInfo, dataInfo->buffer, dataInfo->size);
}

void sendSize(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    sendComp(sockInfo, (char *)&(dataInfo->size), sizeof(int));
}

void sendCheckData(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int seq = dataInfo->fileSize;
    FileCheckData checkData;

    if(dataInfo->error)
    {
        checkData.error = OTHER_ERR;
    }
    else
    {
        if(dataInfo->type == CHK)
        {
            checkData.error = EXIST_ERR;
        }
        else
        {
            checkData.error = NONE_ERR;
        }
    }

    memcpy(checkData.hash, dataInfo->servHash, HASH_SIZE);
    checkData.size = (seq == -1)? seq : seq;
    sendComp(sockInfo, (char *)&checkData, sizeof(checkData));
}

void sendIntegrity(SocketInfo *sockInfo, char boolean)
{
    printDelete("change META");
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
        return nbyte;
    }

    dataInfo->size = nbyte;
    dataInfo->currentSize += nbyte;

    printAdd("load data");

    return nbyte;
}

int receiveMeta(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int nbyte = recvComp(sockInfo, (char *)&(dataInfo->fileMeta), sizeof(dataInfo->fileMeta));
    dataInfo->currentSize = 0;
    printAdd("fileMetaData load.");
    return nbyte;
}

int receiveCheckResult(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    int nbyte = recvComp(sockInfo, dataInfo->buffer, sizeof(enum ServerCommandCode));
    return nbyte;
}

int receiveHash(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    printNotice("load hash data start.");
    int nbyte = recvComp(sockInfo, dataInfo->cliHash, HASH_SIZE);
    printNotice("load hash data.");

    return nbyte;
}

int receive(SocketInfo *sockInfo, RecievedDataInfo *dataInfo)
{
    switch(dataInfo->type)
    {
        case META:
            return receiveMeta(sockInfo, dataInfo);
        case CHK:
            return receiveCheckResult(sockInfo, dataInfo);
        case DATA:
            return receiveData(sockInfo, dataInfo);
        case INTE:
            return receiveHash(sockInfo, dataInfo);
    }

    return 0;
}
