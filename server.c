#include "server.h"

int main(int argc, char *argv[])
{
    struct SocketInfo sockInfo;
    char *buf;
    int size;
    
    serverSocket(&sockInfo);

    while(receive(&sockInfo, buf, &size))
    {
        //do something with file function(buf, size)
    }

    return 0;
}
