#include "clientNetwork.h"

int makeSocket(enum NetworkProtocol p)
{
    int sock;
    if (p == UDP)
    {
        sock = socket(PF_INET, SOCK_DGRAM, 0);
    }
    else if (p == TCP)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
    }

    if (sock == -1)
    {
        printError("socket 생성 실패");
        exit(1);
    }

    return sock;
}

NetworkInfo* connectSocket(char* ip, int port, enum NetworkProtocol p)
{
    NetworkInfo* net;

    net = (NetworkInfo*)malloc(sizeof(NetworkInfo));

    net->sock = makeSocket(p);

    memset(&net->addr, 0, sizeof(net->addr));
    net->addr.sin_family = AF_INET;
    net->addr.sin_port = htons(port);
    net->addr.sin_addr.s_addr= inet_addr(ip);

    if (p == TCP)
    {
        connectTCP(net->sock, &net->addr);
    }

    net->p = p;

    return net;
}

void connectTCP(int fd, struct sockaddr_in* addr)
{
    if (connect(fd, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) < 0)
    {
        printError("Connect error");
        exit(0);
    }
}

void closeSocket(NetworkInfo* n)
{
    close(n->sock);
    free(n);
}

void sendBuffer(NetworkInfo* n, void* data, int size)
{
    if (n->p == TCP)
    {
        send(n->sock, data, size, 0);
    }
    else
    {
        sendto(n->sock, data, size, 0, (struct sockaddr*)&n->addr, sizeof(n->addr));
    }
}

void sendFile(NetworkInfo* n, char* parent, char* fileName)
{
    int i;
    DataFile* file;
    FileMetadata meta = { 0 };
    char buf[MAX_FILE_NAME_LENGTH] = "";
    MetaDir *dir;

    if (strcmp(parent, ""))
    {
        strcpy(buf, parent);
        strcat(buf, "/");
        strcat(buf, fileName);
    }
    else
    {
        strcpy(buf, fileName);
    }
    
    if (isDir(buf))
    {
        printAdd(buf);
        
        dir = listDirectory(buf);
        meta.size = dir->childs;
        meta.type = DIR_TYPE;
        strcpy(meta.parent, parent);
        strcpy(meta.fileName, fileName);
        sendFileMetadata(n, &meta);
        for (i = 0; i < dir->childs; i++)
        {
            sendFile(n, buf, dir->files[i].fileName);
        }

        closeDirectory(dir);
        printDelete(buf);
    }
    else
    {
        printNotice(fileName);
        file = readFile(buf);
        if (file != 0)
        {
            strcpy(meta.fileName, fileName);
            strcpy(meta.parent, parent);
            meta.size = file->fileSize;
            meta.type = FILE_TYPE; 

            sendFileMetadata(n, &meta);
            sendFileData(n, file);
            sendHash(n, file->hash);

            if (!recvResult(n))
            {
                printError("Crash !!");
            }
            else
            {
                printNotice("Success !!");
            }

            closeDataFile(file);
        }
    }
}

void sendFileData(NetworkInfo* n, DataFile* file)
{
    int i, c, end, size = 0, now = 0;
    char* data = file->file;
    struct timeval after, before, start;
    char msg[64];

    c = (file->fileSize / BLOCK_SIZE);
    end = ((file->fileSize % BLOCK_SIZE) ? 1 : 0);

    gettimeofday(&after, NULL);
    gettimeofday(&start, NULL);

    for (i = 0; i < c; i++)
    {
        if (!(i % 66))
        { 
            before = after;
        }
        sendBuffer(n, &data[i * BLOCK_SIZE], BLOCK_SIZE);
        size += BLOCK_SIZE;
        now += BLOCK_SIZE;

        if (!(i % 66))
        {
            gettimeofday(&after, NULL);
            printSpeedByte(before, after, size, now, file->fileSize);
            size = 0;
        }
    }

    if (end)
    {
        sendBuffer(n, &data[i * BLOCK_SIZE], file->fileSize % BLOCK_SIZE);
        size += file->fileSize % BLOCK_SIZE;
        now += BLOCK_SIZE;
    }

    before = after;
    gettimeofday(&after, NULL);

    printSpeedByte(start, after, file->fileSize, now, file->fileSize);
    puts("");
    // printNotice("data send end");
}

void sendFileMetadata(NetworkInfo* n, FileMetadata* meta)
{
    sendBuffer(n, meta, sizeof(*meta));
}

void sendHash(NetworkInfo* n, unsigned char* hash)
{
    sendBuffer(n, hash, HASH_SIZE);
}


char* recvBuffer(NetworkInfo* n, int size)
{
    char* buffer = (char*)malloc(size);
    int addr_size = sizeof(n->addr);
    if (n->p == TCP)
    {
        read(n->sock, buffer, size);
    }
    else
    {
        recvfrom(n->sock, buffer, size, 0 ,(struct sockaddr*)&n->addr, &addr_size);
    }
    return buffer;
}

char recvResult(NetworkInfo* n)
{
    char* buf = recvBuffer(n, 1);
    char result = *buf;
    free(buf);
    return result;
}