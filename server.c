#include <stdio.h>
#include <string.h>
#include "server.h"
#include "print.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    RecievedDataInfo dataInfo;
    int size, fileDescriptor;
    pid_t protocolPid;

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

        while( (size = receive(&sockInfo, &dataInfo)) != -1  )
        {
            if(size == 0 && sockInfo.protocol == TCP)
            {
                printNotice("Close client connection.");
                close(sockInfo.cliSockId);
                return 0;
            }

            printNotice(dataInfo.buffer);

            //writeFile(fileDescriptor, &dataInfo);

            if(dataInfo.type == INTE)
            {
                sendIntegrity(&sockInfo, &dataInfo);
                //close(fileDescriptor);
            }
        }
    }

    return 0;
}

