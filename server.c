#include <stdio.h>
#include <string.h>
#include "server.h"
#include "print.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    FileMetadata fileMeta;
    char buffer[BLOCK_SIZE+1];
    int size, currentSize;
    RecievedDataInfo RDI;
    pid_t pid;
    
    serverSocket(&sockInfo);

    while(1)
    {

        if( (sockInfo.cliSockId = accept(sockInfo.sockId, (struct sockaddr *)&(sockInfo.cliAddr), &(sockInfo.addrLen))) == -1 )
        {
            printError("Accept error");
            return 0;
        }

        printNotice("Accept success");

        switch( (pid = fork()) )
        {
            case -1:
                printError("cant fork error\n");
                return 0;
            case 0:
                while(1)
                {
                    //TO DO
                    //Delete Session
                    RDI.currentSize = 0;
                    RDI.fileMeta = receiveFileMetadata(&sockInfo);
                    printNotice("fileMetaData load.");
                    //TO DO
                    //Temp Test
                    char directory[256] = "/temp_test/test2/";
                    checkFile(&RDI,directory);

                    while( (size = receive(&sockInfo, buffer, RDI.currentSize, RDI.fileMeta.size)) != -1  )
                    {
                        if(size == 0)
                        {
                            printNotice("Close client connection.");
                            close(sockInfo.cliSockId);
                            return 0;
                        }
                        printNotice(buffer);                          
                        writeFile(&RDI, size);
                    }
                    receiveHash(&sockInfo, RDI.cliHash, HASH_SIZE);
                    sendIntegrity(&sockInfo, (char)(memcmp(RDI.servHash, RDI.cliHash, HASH_SIZE) == 0));
                    
                }
                break;
        }
        
    }

    return 0;
}

