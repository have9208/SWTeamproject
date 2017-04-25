#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "network.h"
#include "clientFile.h"
#include "file.h"
#include "print.h"

typedef struct NetworkInfo
{
    int sock;
    struct sockaddr_in addr;
    enum NetworkProtocol p;
} NetworkInfo;

#define MODE_IP (0x1)
#define MODE_FILE (0x1 << 1)
#define MODE_PROTOCOL (0x1 << 2)

int makeSocket(enum NetworkProtocol p);
NetworkInfo* connectSocket(char* ip, int port, enum NetworkProtocol p);
void connectTCP(int fd, struct sockaddr_in* addr);
void closeSocket(NetworkInfo* n);

void sendBuffer(NetworkInfo* n, void* data, int size);

void sendFile(NetworkInfo* n, char* filename);
void sendFileData(NetworkInfo* n, DataFile* file);
void sendFileMetadata(NetworkInfo* n, FileMetadata* meta);
void sendHash(NetworkInfo* n, char* hash);

char* recvBuffer(NetworkInfo* n, int size);
char recvResult(NetworkInfo* n);

#endif /* CLIENT_NETWORK_H */