#include "client.h"

int main(int argc, char* argv[])
{
    int sock;
    char* ip;
    struct sockaddr_in* server_addr;
    FileMetadata meta = { 0 };
    DataFile* file;

    if (argc < 2)
    {
        ip = "127.0.0.1";
    }
    else
    {
        ip = argv[1];
    }

    scanf("%s", meta.fileName);

    sock = makeSocket();
    server_addr = connectSocket(ip, PORT);
    connectTCP(sock, server_addr);

    file = readFile(meta.fileName);
    meta.size = file->fileSize;

    sendFileMetadata(sock, server_addr, &meta);
    sendFile(sock, server_addr, file);

    closeSocket(sock, server_addr);

    return 0;
}