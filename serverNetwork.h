#ifndef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include "network.h"

int serverSocket();
char* receive(int sockId, char *buffer, int *size);

#endif /*__SERVER_NETWORK_H__*/
