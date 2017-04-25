#include <stdio.h>
#include <string.h>
#include "server.h"
#include "print.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    RecievedDataInfo dataInfo;
    int nbyte;
    pid_t protocolPid;
    char directory[256] = "/temp_test/test2/";

    switch( (protocolPid = fork()) )
    {
        case -1:
        printError("cant fork error\n");
        return 0;
        case 0:
        sockInfo.protocol = TCP;
        break;
        default:
        sockInfo.protocol = UDP;
        break;
    }
    
    serverSocket(&sockInfo, &dataInfo);

    while( acceptComp(&sockInfo) )
    {
        //fileDescriptor = checkFile(&dataInfo);

        while( (nbyte = receive(&sockInfo, &dataInfo)) != -1  )
        {
            if(nbyte == 0 && sockInfo.protocol == TCP)
            {
                printNotice("Close client connection.");
                close(sockInfo.cliSockId);
                return 0;
            }

            if(dataInfo.type == INTE)
            {
                checkFile(&dataInfo, directory);
            }
            else if(dataInfo.type == DATA)
            {
                printNotice(dataInfo.buffer);
                writeFile(&dataInfo, dataInfo.size);
            }
            else if(dataInfo.type == INTE)
            {
                sendIntegrity(&sockInfo, &dataInfo);
                close(dataInfo.fileDescriptor);
            }

        }
    }

    return 0;
}

