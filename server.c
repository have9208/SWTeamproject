#include <stdio.h>
#include <string.h>
#include "server.h"
#include "print.h"

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    RecievedDataInfo dataInfo;
    int nbyte;
    SHA256_CTX ctx;
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

        while( (nbyte = receive(&sockInfo, &dataInfo)) != -1  )
        {
            if(nbyte == 0 && sockInfo.protocol == TCP)
            {
                printNotice("Close client connection.");
                return 0;
            }

            if(dataInfo.type == META)
            {
                checkFile(&dataInfo);
            }
            else
            {
                printNotice(dataInfo.buffer);
                writeFile(&ctx,&dataInfo);
            }

            if(dataInfo.type == INTE)
            {
                sendIntegrity(&sockInfo, &dataInfo);
            }

        }
    }

    return 0;
}
