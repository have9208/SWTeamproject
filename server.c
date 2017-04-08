#include <stdio.h>
#include <string.h>
#include "server.h"

char *buffer[] = {
    "hello world\n","new World","struct SocketInfo sockInfo;\n"
};

int main(int argc, char *argv[])
{
    struct SocketInfo sockInfo;
    FileMetadata fileMeta;
    char buf[BLOCK_SIZE+1];
    int size;
    
    serverSocket(&sockInfo);

    fileMeta = receiveFileMetadata(&sockInfo);

    while(receive(&sockInfo, buf, &size))
    {
        writeFile(buf, fileMeta.fileName, size);
    }
   
    return 0;
}
