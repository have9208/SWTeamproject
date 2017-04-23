#ifndef __CLIENT_FILE_H__
#define __CLIENT_FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "sha256.h"

#define BUFF_SIZE 1024

typedef struct DataFile
{
    char *file;
    int fileSize;
    unsigned char hash[HASH_SIZE];
} DataFile;

int getFileSize(int fd);
int openFile(char *fileName);
DataFile* readFile();
SHA256_CTX gethash(int fd,SHA256_CTX crt,DataFile *fileBuf);

#endif