#include <stdio.h>
#include <string.h>
#include "server.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    FileMetadata fileMeta;
    char buffer[BLOCK_SIZE+1];
    int size, currentSize;
    int fileDescriptor;
    pid_t pid;
    
    serverSocket(&sockInfo);

    while(1)
    {
        /*
        currentSize = 0;
        fileMeta = receiveFileMetadata(&sockInfo);

        while( (size = receive(&sockInfo, buffer)) != -1  )
        {
            writeFile(buffer, fileMeta.fileName, size);
            currentSize += size;

            if(fileMeta.size <= currentSize){
                break;
            }
        }
        */

        if( (sockInfo.cliSockId = accept(sockInfo.sockId, (struct sockaddr *)&(sockInfo.cliAddr), &(sockInfo.addrLen))) == -1 )
        {
            printf("Accept error");
            return 0;
        }

        switch( (pid = fork()) )
        {
            case -1:
            printf("cant fork error\n");
            return 0;
            case 0:
            while(1)
            {
                currentSize = 0;
                fileMeta = receiveFileMetadata(&sockInfo);
                
                fileDescriptor = createFile(buffer, fileMeta.fileName, size);
                while( (size = receive(&sockInfo, buffer)) != -1  )
                {
                    if(size == 0)
                    {
                        close(sockInfo.cliSockId);
                        return 0;
                    }
                    writeFile(buffer, fileMeta.fileName, size);
                    currentSize += size;

                    if(fileMeta.size <= currentSize){
                        break;
                    }
                }
                close(fileDescriptor);
            }
            break;
        }
        
    }

    return 0;
}

