#include <stdio.h>
#include <string.h>
#include "server.h"
#include "print.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    RecievedDataInfo dataInfo;
    int nbyte;
    pid_t protocolPid, acceptPid;

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
        if(sockInfo.protocol == TCP)
        {
            acceptPid = fork();

            if(acceptPid == -1)
            {
                printError("cant fork error\n");
                return 0;
            }
            else if(acceptPid > 0)
            {
                continue;
            }
        }


        while( (nbyte = receive(&sockInfo, &dataInfo)) != -1  )
        {
            if(nbyte == 0 && sockInfo.protocol == TCP)
            {
                printDelete("Close client connection.");
                return 0;
            }

            if(dataInfo.type == META)
            {
                checkFile(&dataInfo);
            }
            else
            {
                printNotice(dataInfo.buffer);
                writeFile(&dataInfo);
            }

        }
    }

    return 0;
}

