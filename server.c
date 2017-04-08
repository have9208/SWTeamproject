#include <stdio.h>
#include "server.h"

int main(int argc, char *argv[])
{
    char *buf;
    int pid, size;
    int sockId = serverSocket();

    while(receive(sockId, buf, &size))
    {
        //do something with file function(buf, size)
    }

    return 0;
}
