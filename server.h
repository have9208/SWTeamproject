#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include "serverFile.h"
#include "serverNetwork.h"

enum PacketType {META=1,CHK=2,DATA=3,INTE=4};

typedef struct RecievedDataInfo
{
    char buffer[BLOCK_SIZE];
    char tmpFile[256];
    char pathFile[256];
    unsigned char servHash[HASH_SIZE];
    unsigned char cliHash[HASH_SIZE];
    int type;
    int size;
    int currentSize;
    int fileDescriptor;
    int fileSequence;
    SHA256_CTX ctx;
    FileMetadata fileMeta;
}RecievedDataInfo;

#endif /*__SERVER_H__*/