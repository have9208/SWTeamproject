#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include "serverFile.h"
#include "serverNetwork.h"
#include "network.h"

enum PacketType {CMD,META,CHK,DATA,INTE};

typedef struct RecievedDataInfo
{
    char error; 
    char buffer[BLOCK_SIZE];
    char pathFile[256];
    char tmpFile[256];
    unsigned char servHash[HASH_SIZE];
    unsigned char cliHash[HASH_SIZE];
    int type;
    int size;
    int currentSize;
    int fileDescriptor;
    int fileSize;
    SHA256_CTX ctx;
    FileMetadata fileMeta;
}RecievedDataInfo;

#endif /*__SERVER_H__*/