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
    int fileDescriptor;
    SHA256_CTX ctx;
    unsigned char servHash[HASH_SIZE], cliHash[HASH_SIZE];
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
                    currentSize = 0;
                    fileMeta = receiveFileMetadata(&sockInfo);
                    printNotice("fileMetaData load.");
                    
                    fileDescriptor = checkFile(buffer, fileMeta.fileName, size);
                    
                    sha256_init(&ctx);
                    strcpy(servHash, "");
                    while( (size = receive(&sockInfo, buffer, currentSize, fileMeta.size)) != -1  )
                    {
                        if(size == 0)
                        {
                            printNotice("Close client connection.");
                            close(sockInfo.cliSockId);
                            return 0;
                        }
                        writeFile(fileDescriptor,buffer, fileMeta.fileName, size);
                        printNotice(buffer);
                        sha256_update(&ctx,buffer, size);
                        currentSize += size;
                        printNotice("load data");

                        if(fileMeta.size <= currentSize){
                            printNotice("end load file");
                            break;
                        }
                    }
                    sha256_final(&ctx, servHash);
                    receiveHash(&sockInfo, cliHash, HASH_SIZE);
                    sendIntegrity(&sockInfo, (char)(memcmp(servHash, cliHash, HASH_SIZE) == 0));
                    close(fileDescriptor);
                }
                break;
        }
        
    }

    return 0;
}

