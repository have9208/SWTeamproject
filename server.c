#include <stdio.h>
#include <unistd.h>
#include "server.h"

int main(int argc, char *argv[])
{
    char *buf;
    int pid, size;
    int sockId = serverSocket();

    while(receive(buf, &size))
    {
        
    }

    return 0;
}