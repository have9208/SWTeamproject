#ifndef __SERVER_FILE_H__
#define __SERVER_FILE_H__

#include "file.h"
#include <stdio.h>         // puts()
#include <string.h>        // strlen()
#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()
#include <sys/types.h>
#include <sys/stat.h>
#include "print.h"
#include "sha256.h"
#include "server.h"

typedef struct RecievedDataInfo RecievedDataInfo;

int checkFile(char *directoryPath,char *fileBuffer,char *fileName);
int writeFile(RecievedDataInfo *RDI,int fileDescriptor,char *fileBuffer,char *fileName, int size);

#endif
