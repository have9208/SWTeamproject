#ifdef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include "network.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void serverSocket();
char* receive(char *buffer, int *size);

#endif /*__SERVER_NETWORK_H__*/
