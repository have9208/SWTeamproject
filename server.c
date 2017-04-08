#include <stdio.h>
#include <string.h>
#include "server.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    FileMetadata fileMeta;
    char buffer[BLOCK_SIZE+1];
    int size;
    
    serverSocket(&sockInfo);

    fileMeta = receiveFileMetadata(&sockInfo);

    while(receive(&sockInfo, buffer, &size))
    {
        writeFile(buffer, fileMeta.fileName, size);
    }
   
    return 0;
}
