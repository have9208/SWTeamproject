#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "server.h"
#include "print.h"

int sockId;
pid_t protocolPid;
void (*oldSignal)(int);

void exitSignal(int sig)
{
    printf("test");
    if(protocolPid != 0)
    {
        kill(protocolPid, SIGINT);
    }

    close(sockId);
    signal(SIGINT, oldSignal);
}

int main(int argc, char *argv[])
{
    SocketInfo sockInfo;
    RecievedDataInfo dataInfo;
    int nbyte;
    SHA256_CTX ctx;

    switch( (protocolPid = fork()) )
    {
        case -1:
            printError("cant fork error");
            return 0;
        case 0:
            sockInfo.protocol = UDP;
            break;
        default:
            sockInfo.protocol = TCP;
            break;
    }
    
    serverSocket(&sockInfo, &dataInfo);
    oldSignal = signal(SIGINT, exitSignal);

    while( acceptComp(&sockInfo) )
    {
        while( (nbyte = receive(&sockInfo, &dataInfo)) != -1 )
        {
            if(nbyte == 0 && sockInfo.protocol == TCP)
            {
                printNotice("Close client connection.");
                break;
            }

            if(dataInfo.type == META)
            {
                checkFile(&ctx,&dataInfo);
            }
            else if(dataInfo.type == DATA)
            {
                writeFile(&ctx,&dataInfo);
            }
            else if(dataInfo.type == INTE)
            {
                sendIntegrity(&sockInfo, &dataInfo);
            }

        }
    }

    return 0;
}
