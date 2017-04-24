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

int checkFile(char *fileBuffer,char *fileName, int fileSize);
int writeFile(int fileDescriptor,char *fileBuffer,char *fileName, int fileSize);
char *writeError();

#endif
