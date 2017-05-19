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
    usleep(50);
    printf("data: %s\n", (char*)data);
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
    char *fullName = makeFullPath(parent, fileName), *hash, input;
    FileMetadata *meta;
    MetaDir *dir;
    NetworkInfo* n;
    int i, fd;
    FileCheckData error;
    enum ServerCommandCode code;

    // Directory
    if (isDir(fullName))
    {
        printAdd(fullName);

        n = connectSocket(netMeta->ip, netMeta->port, (netMeta->protocol == UDP ? UDP : TCP));
        
        dir = listDirectory(fullName);
        meta = makeFileMetadata(DIR_TYPE, 0, parent, fileName);

        error = sendFileMetadata(n, meta);

        closeSocket(n);

        if (error.error != OTHER_ERR)
        {
            for (i = 0; i < dir->childs; i++)
            {
                sendFile(netMeta, fullName, dir->files[i].fileName);
            }
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

        // Check Protocol
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

        error = sendFileMetadata(n, meta);

        switch (error.error)
        {
            case NONE_ERR:
                printNotice("NONE_ERR");
                code = REWRITE;
                break;
            case EXIST_ERR:
                printNotice("EXIST_ERR");
                printf("%d\n", error.size);
                if (error.size == -1)
                {
                    printNotice("hashCheck");
                    hash = getHash(fd, i);
                    
                    printHash(hash);
                    printHash(error.hash);
                    
                    if (hashCheck(hash, error.hash))
                    {
                        printNotice("hash OK");
                        code = IGNORE;
                    }
                    else
                    {
                        printf("다른 파일이 존재합니다. 덮어쓰시겠습니까? (Y/N) ");
                        scanf("%c", &input);
                        if (input == 'Y' || input == 'y')
                        {
                            code = REWRITE;
                        }
                        else
                        {
                            code = IGNORE;
                        }
                    }
                }
                else
                {
                    hash = getHash(fd, error.size);

                    printHash(hash);
                    printHash(error.hash);

                    if (hashCheck(hash, error.hash))
                    {
                        printNotice("EXIST_ERR");
                        code = APPEND;
                    }
                    else
                    {
                        printNotice("EXIST_ERR");
                        code = REWRITE;
                    }
                    free(hash);
                }

                printf("code: %x\n", code);
                sendServerCommandCode(n, code);

                break;
            case OTHER_ERR:
                printNotice("OTHER_ERR");
                code = IGNORE;
                break;
            default:
                printError("recvFileCheckData Error Code");
        }

        hash = getHash(fd, i);
        switch (code)
        {
            case APPEND:
                printNotice("APPEND");
                sendFileData(n, fd, error.size, i - error.size);
                sendHash(n, hash);
                break;
            case REWRITE:
                printNotice("REWRITE");
                sendFileData(n, fd, 0, i);
                sendHash(n, hash);
                break;
        }

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

FileCheckData sendFileMetadata(NetworkInfo* n, FileMetadata* meta)
{
    FileCheckData check;
    sendBuffer(n, meta, sizeof(*meta));

    printf("name: %s\n", meta->fileName);

    check = recvFileCheckData(n);

    return check;
}

void sendHash(NetworkInfo* n, unsigned char* hash)
{
    sendBuffer(n, hash, HASH_SIZE);

    printHash(hash);
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

void sendServerCommandCode(NetworkInfo* n, enum ServerCommandCode code)
{
    sendBuffer(n, (char*)&code, sizeof(code));
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

FileCheckData recvFileCheckData(NetworkInfo* n)
{
    FileCheckData* tmp = (FileCheckData*)recvBuffer(n, sizeof(FileCheckData));
    FileCheckData result = *tmp;
    free(tmp);

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

/* Hash */
int hashCheck(char* org, char* recv)
{
    return memcmp(org, recv, HASH_SIZE) ? 0 : 1;
}