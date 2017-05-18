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
    sockId = sockInfo.sockId;
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
            printf("NOW : %d\n",dataInfo.type);
            switch(dataInfo.type)
            {           
                case META:
                    printf("META\n");
                    printf("File name : %s\n",dataInfo.fileMeta.fileName);                   
                    checkFile(&ctx,&dataInfo);
                    sendCheckData(&sockInfo, &dataInfo);
                    break;
                case CHK:
                    printf("CHK\n");
                    verifyFile(&dataInfo);
                    break;
                case DATA:
                    printf("DATA\n");
                    writeFile(&ctx,&dataInfo);
                    break;
                case INTE:
                    sendIntegrity(&sockInfo, &dataInfo);
                    break;
            }

        }
    }

    return 0;
}
