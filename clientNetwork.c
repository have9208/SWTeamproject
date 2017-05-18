#include "clientNetwork.h"

int makeSocket(enum NetworkProtocol p)
{
    int sock;
    if (p == UDP)
    {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
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

void sendFile(NetworkMetaInfo* netMeta, char* parent, char* fileName)
{
    char *fullName = makeFullPath(parent, fileName), *hash;
    FileMetadata *meta;
    MetaDir *dir;
    NetworkInfo* n;
    int i, fd;

    // Directory
    if (isDir(fullName))
    {
        printAdd(fullName);

        n = connectSocket(netMeta->ip, netMeta->port, (netMeta->protocol == UDP ? UDP : TCP));
        
        dir = listDirectory(fullName);
        meta = makeFileMetadata(DIR_TYPE, 0, parent, fileName);

        sendFileMetadata(n, meta);

        closeSocket(n);

        for (i = 0; i < dir->childs; i++)
        {
            sendFile(netMeta, fullName, dir->files[i].fileName);
        }

        closeDirectory(dir);
        closeFileMetadata(meta);
        printDelete(fullName);
    }
    // Data file
    else
    {
        printNotice(fileName);

        i = getFileSize(fullName);
        fd = openFile(fullName);
        meta = makeFileMetadata(FILE_TYPE, i, parent, fileName);
        hash = getHash(fd, i);

        if (netMeta->protocol == AUTO)
        {
            if (i <= 64 * 1024)
            {
                n = connectSocket(netMeta->ip, netMeta->port, UDP);
            }
            else
            {
                n = connectSocket(netMeta->ip, netMeta->port, TCP);
            }
        }
        else
        {
            n = connectSocket(netMeta->ip, netMeta->port, netMeta->protocol);
        }

        sendFileMetadata(n, meta);
        sendFileData(n, fd, 0, i);
        sendHash(n, hash);

        free(hash);
        closeSocket(n);
        closeFileMetadata(meta);
        close(fd);

    }
}

void sendFileData(NetworkInfo* n, int fd, int offset, int size)
{
    int i, c, end, len = 0;
    struct timeval after, before, start;
    char *buf;

    c = (size / BLOCK_SIZE);
    end = ((size % BLOCK_SIZE) ? 1 : 0);

    gettimeofday(&after, NULL);
    gettimeofday(&start, NULL);

    for (i = 0; i < c; i++)
    {
        usleep(30);
        if (!(i % 40))
        { 
            before = after;
        }

        buf = fileSequenceChk(fd, offset, BLOCK_SIZE);
        sendBuffer(n, buf, BLOCK_SIZE);
        free(buf);

        offset += BLOCK_SIZE;
        len += BLOCK_SIZE;

        if (!(i % 40))
        {
            gettimeofday(&after, NULL);
            printSpeedByte(before, after, len, offset, size);
            size = 0;
        }
    }

    if (end)
    {
        i = size % BLOCK_SIZE;
        buf = fileSequenceChk(fd, offset, i);
        sendBuffer(n, buf, i);
        free(buf);

        offset += i;
        len += i;
    }

    gettimeofday(&after, NULL);

    printSpeedByte(start, after, size, offset, size);

}

void sendFileMetadata(NetworkInfo* n, FileMetadata* meta)
{
    sendBuffer(n, meta, sizeof(*meta));
}

void sendHash(NetworkInfo* n, unsigned char* hash)
{
    usleep(500);
    sendBuffer(n, hash, HASH_SIZE);

    printNotice("Hash!!");
    if (!recvResult(n))
    {
        printError("Crash !!");
    }
    else
    {
        // printNotice("Success !!");
    }
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
        recvfrom(n->sock, buffer, size, 0, NULL, NULL);
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


/* File Info */
char* makeFullPath(char* parent, char* fileName)
{
    int len = strlen(parent) + 1 + strlen(fileName) + 1;
    char* buf = malloc(sizeof(char) * len);

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

    return buf;
}

FileMetadata* makeFileMetadata(enum fileType type, int size, char* parent, char* fileName)
{
    FileMetadata* meta = malloc(sizeof(FileMetadata));

    meta->type = type;
    meta->size = size;
    strcpy(meta->parent, parent);
    strcpy(meta->fileName, fileName);

    return meta;
}

void closeFileMetadata(FileMetadata* meta)
{
    free(meta);
}