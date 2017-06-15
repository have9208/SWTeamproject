#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "server.h"
#include "print.h"

int sockId;
pid_t protocolPid;
void (*oldSignal)(int);

void run(SocketInfo *sockInfom, RecievedDataInfo *dataInfo);

void exitSignal(int sig)
{
    if(protocolPid != 0)
    {
        kill(protocolPid, SIGINT);
    }

    close(sockId);
    signal(SIGINT, oldSignal);
}

int main(int argc, char *argv[])
{
    int nbyte;
    SocketInfo sockInfo;
    RecievedDataInfo dataInfo;
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
    
    serverSocket(&sockInfo);
    sockId = sockInfo.sockId;
    oldSignal = signal(SIGINT, exitSignal);

    while( acceptComp(&sockInfo, &dataInfo) )
    {
        while( (nbyte = receive(&sockInfo, &dataInfo)) != -1 )
        {
            if(nbyte == 0 && sockInfo.protocol == TCP)
            {
                printNotice("Close client connection.");
                break;
            }
            switch(dataInfo.type)
            {
                case META:
                    printf("META\n");
                    switch(dataInfo.fileMeta.code)
                    {
                        case LIST:
                            printf("Receive list command.\n");
                            break;
                        case UPLOAD:
                            checkFile(&ctx,&dataInfo);
                            sendCheckData(&sockInfo, &dataInfo);
                            break;
                        case DELETE:
                            printf("Receive delete command.\n");
                            break;
                    }         
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
                    printf("INTE\n");
                    sendIntegrity(&sockInfo, checkHash(&dataInfo));
                    break;
            }

        }
    }
    
    close(sockId);
    return 0;
}