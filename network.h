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

enum NetworkProtocol { UDP = 1, TCP = 2, AUTO = 3 };

#endif /* NETWORK_H */