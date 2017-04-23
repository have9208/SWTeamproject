#ifndef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "network.h"
#include "file.h"

typedef struct SocketInfo
{
    int sockId;
    int cliSockId;
    int addrLen;
    struct sockaddr_in servAddr;
    struct sockaddr_in cliAddr;
}SocketInfo;

void serverSocket(SocketInfo *sockInfo);
int receive(SocketInfo *sockInfo, char *buffer);
FileMetadata receiveFileMetadata(SocketInfo *sockInfo);

#endif /*__SERVER_NETWORK_H__*/
