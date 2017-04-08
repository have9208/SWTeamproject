#ifndef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "network.h"
#include "file.h"

struct SocketInfo
{
    int sockId;
    int addrLen;
    struct sockaddr_in servAddr;
    struct sockaddr_in cliAddr;
};

void serverSocket(struct SocketInfo *sockInfo);
char* receive(struct SocketInfo *sockInfo, char *buffer, int *size);

#endif /*__SERVER_NETWORK_H__*/
