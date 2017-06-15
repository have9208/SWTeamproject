#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "server.h"
#include "print.h"

int sockId;
pid_t protocolPid, connectionPid;
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
    SocketInfo sockInfo;
    RecievedDataInfo dataInfo;

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
        connectionPid = (sockInfo.protocol == TCP)? fork() : 0;

        if(connectionPid == 0)
        {
            run(&sockInfom, &dataInfo);
            if(sockInfo.protocol == TCP)
            {
                break;   
            }
        }
        else if(connectionPid == -1)
        {
            printError("cant fork error");
            return 0;
        }
        
    }
    
    close(sockId);
    return 0;
}

void run(SocketInfo *sockInfom, RecievedDataInfo *dataInfo){
    int nbyte;
    SHA256_CTX ctx;

    while( (nbyte = receive(sockInfo, dataInfo)) != -1 )
    {
        if(nbyte == 0 && sockInfo->protocol == TCP)
        {
            printNotice("Close client connection.");
            break;
        }
        switch(dataInfo->type)
        {
            case CMD:
                break;
            case META:
                printf("META\n");
                printf("File name : %s\n",dataInfo->fileMeta.fileName);                 
                checkFile(&ctx,dataInfo);
                sendCheckData(sockInfo, dataInfo);
                break;
            case CHK:
                printf("CHK\n");
                verifyFile(dataInfo);
                break;
            case DATA:
                printf("DATA\n");   
                writeFile(&ctx,dataInfo);
                break;
            case INTE:
                printf("INTE\n");
                sendIntegrity(sockInfo, checkHash(dataInfo));
                break;
        }

    }
}