#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"

int makeSocket();
struct sockaddr_in* connectSocket(char* ip, int port);
void closeSocket(int sock, struct sockaddr_in* server_addr);

void sendBuffer(int sock, struct sockaddr_in* addr, char* str, int size);
void sendFile(int sock, struct sockaddr_in* addr, char* data, int size);

#endif /* CLIENT_NETWORK_H */