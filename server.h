#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include "serverFile.h"
#include "serverNetwork.h"

enum PacketType {META=1,DATA=2,INTE=3};

typedef struct RecievedDataInfo
{
    char buffer[BLOCK_SIZE];
    unsigned char servHash[HASH_SIZE];
    unsigned char cliHash[HASH_SIZE];
    int type;
    int size;
    int currentSize;
    SHA256_CTX ctx;
    FileMetadata fileMeta;
}RecievedDataInfo;

#endif /*__SERVER_H__*/