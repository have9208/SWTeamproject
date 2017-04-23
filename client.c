#include "client.h"

int main(int argc, char* argv[])
{
    int sock;
    int opt;
    char* ip;
    struct sockaddr_in* server_addr;
    FileMetadata meta = { 0 };
    DataFile* file;

    char mode = 0;

    while((opt = getopt(argc, argv, "l:f:")) != -1)
    {
        switch(opt)
        { 
            case 'l':
                ip = malloc(sizeof(optarg));
                strcpy(ip, optarg);
                mode |= MODE_IP;
                break; 
            case 'f':
                strcpy(meta.fileName, optarg);
                mode |= MODE_FILE;
                break;
            default:
                break;
        }
    } 

    if (!(mode & MODE_IP))
    {
        ip = "127.0.0.1";
    }

    if (!(mode & MODE_FILE))
    {
        printClientHelp(argv[0]);
    }

    sock = makeSocket();
    server_addr = connectSocket(ip, PORT);
    connectTCP(sock, server_addr);

    file = readFile(meta.fileName);
    meta.size = file->fileSize;

    sendFileMetadata(sock, server_addr, &meta);
    sendFile(sock, server_addr, file);
    sendHash(sock, server_addr, file->hash);

    closeSocket(sock, server_addr);

    return 0;
}