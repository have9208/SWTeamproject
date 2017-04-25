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

void checkFile(RecievedDataInfo *RDI);
void writeFile(SHA256_CTX *ctx,RecievedDataInfo *RDI);
void deleteFile(RecievedDataInfo *RDI);

#endif
