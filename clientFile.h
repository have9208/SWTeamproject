#ifndef __CLIENT_FILE_H__
#define __CLIENT_FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFF_SIZE 1024

typedef struct datFileType{
    char *file;
    int fileSize;
}datFile;

datFile* readFile();

#endif