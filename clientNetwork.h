#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "clientFile.h"
#include "file.h"
#include "print.h"

#define MODE_IP (0x1)
#define MODE_FILE (0x1 << 1)

int makeSocket();
struct sockaddr_in* connectSocket(char* ip, int port);
void connectTCP(int fd, struct sockaddr_in* addr);
void closeSocket(int sock, struct sockaddr_in* server_addr);

void sendBuffer(int sock, struct sockaddr_in* addr, void* data, int size);
void sendFile(int sock, struct sockaddr_in* addr,  DataFile* file);
void sendFileMetadata(int sock, struct sockaddr_in* addr, FileMetadata* meta);

#endif /* CLIENT_NETWORK_H */