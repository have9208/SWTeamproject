#include <stdio.h>
#include <string.h>
#include "server.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    FileMetadata fileMeta;
    char buffer[BLOCK_SIZE+1];
    int size, currentSize;
    
    serverSocket(&sockInfo);

    while(1)
    {
        currentSize = 0;
        fileMeta = receiveFileMetadata(&sockInfo);

        while( size = receive(&sockInfo, buffer) >= 0  )
        {
            writeFile(buffer, fileMeta.fileName, size);
            currentSize += size;

            if(fileMeta.size <= currentSize){
                break;
            }
        }
        
    }

    return 0;
}
