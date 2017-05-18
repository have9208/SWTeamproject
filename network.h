#ifndef __NETWORK_H__
#define __NETWORK_H__

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 31337
#define BLOCK_SIZE (1024 * 32)

enum NetworkProtocol { UDP, TCP };
enum ErrorCode { NONE_ERR, EXIST_ERR, OTHER_ERR };

typedef struct FileCheckData
{
    enum ErrorCode error;
    char hash[32];
    int size;
} FileCheckData;

#endif /* NETWORK_H */