#include "clientNetwork.h"

int makeSocket()
{
    int sock;
    sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (sock == -1)
    {
        puts( "socket 생성 실패");
        exit(1);
    }

    return sock;
}

struct sockaddr_in* connectSocket(char* ip, int port)
{
    struct sockaddr_in *server_addr;
    server_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);
    server_addr->sin_addr.s_addr= inet_addr(ip);
    
    return server_addr;
}

void closeSocket(int sock, struct sockaddr_in* addr)
{
    close(sock);
    free(addr);
}

void sendBuffer(int sock, struct sockaddr_in* addr, void* data, int size)
{
    sendto(sock, data, size, 0, (struct sockaddr*)addr, sizeof(*addr));
}

void sendFile(int sock, struct sockaddr_in* addr, DataFile* file)
{
    int i, c, end;
    char* data = file->file;
    c = (file->fileSize / BLOCK_SIZE);
    end = ((file->fileSize % BLOCK_SIZE) ? 1 : 0);

    for (i = 0; i < c; i++)
    {
        sendBuffer(sock, addr, &data[i * BLOCK_SIZE], BLOCK_SIZE);
    }

    if (end)
    {
        sendBuffer(sock, addr, &data[i * BLOCK_SIZE], file->fileSize % BLOCK_SIZE);
    }
}

void sendFileMetadata(int sock, struct sockaddr_in* addr, FileMetadata* meta)
{
    sendBuffer(sock, addr, meta, sizeof(*meta));
}