#ifndef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "network.h"
#include "file.h"
#include "server.h"

typedef struct SocketInfo
{
    int sockId;
    int cliSockId;
    int addrLen;
    int protocol;
    struct sockaddr_in servAddr;
    struct sockaddr_in cliAddr;
}SocketInfo;

typedef struct RecievedDataInfo RecievedDataInfo;

void serverSocket(SocketInfo *sockInfo);
int acceptComp(SocketInfo *sockInfo, RecievedDataInfo *dataInfo);
int receive(SocketInfo *sockInfo, RecievedDataInfo *dataInfo);
void sendCheckData(SocketInfo *sockInfo, RecievedDataInfo *dataInfo);
void sendIntegrity(SocketInfo *sockInfo, RecievedDataInfo *dataInfo);

#endif /*__SERVER_NETWORK_H__*/
