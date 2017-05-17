/**************************************

sha256.h : sha256 알고리즘 관련 헤더 파일
Struct DataFile : 파일, 파일 사이즈, 해쉬 값을 클라이언트 서버쪽에 넘겨주기 위한 구조체
getFileSize() : 클라이언트 서버쪽에 넘겨주기 위한 파일 사이즈를 구하기 위한 함수
openFile() : 파일을 오픈하는 함수
readFile() : clientFile의 메인이 되는 함수
gethash() : sha256.h를 이용해 해쉬값을 얻어내는 함수

**************************************/

#ifndef __CLIENT_FILE_H__
#define __CLIENT_FILE_H__

#include "sha256.h"
#include "print.h"
#include "file.h"
#include "network.h"
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#define BUFF_SIZE (BLOCK_SIZE)

typedef struct DataFile
{
    char *file;
    int fileSize;
    char fileName[MAX_FILE_NAME_LENGTH];
    unsigned char hash[HASH_SIZE];
} DataFile;

typedef struct MetaDir
{
    DataFile *files;
    char path[MAX_FILE_NAME_LENGTH];
    int childs;
} MetaDir;

MetaDir* listDirectory(char* dirName);
int getDirectoryFileCount(char *dirName);
char* fileSequenceChk(int *fd,int sequence,int fileSize,bool lastFile);

SHA256_CTX getHashCtx(int *fd,SHA256_CTX ctx,int fileSize);
unsigned char* getHash(int *fd,int fileSize);

int getFileSize(char *fileName);
int openFile(char *fileName);
bool isDir(char *fileName);
void closeDataFile(DataFile* data);
void closeDirectory(MetaDir* dir);

#endif