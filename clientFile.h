#ifndef __CLIENT_FILE_H__
#define __CLIENT_FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFF_SIZE 1024

typedef struct DataFileType{
    char *file;
    int fileSize;
}DataFile;

int getFileSize(int fd);
int openFile(char *fileName);
DateFile* readFile();

#endif