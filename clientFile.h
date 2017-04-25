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
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#define BUFF_SIZE 1024
#define DIR_SIZE 32

typedef struct DataFile
{
    char *file;
    int fileSize;
    unsigned char hash[HASH_SIZE];
} DataFile;

typedef struct MetaDir
{
    DataFile fileBuf;
    bool dir;
    char path[BUFF_SIZE];
} MetaDir;

int getFileSize(int fd);
int openFile(char *fileName);
DataFile* readFile();
SHA256_CTX gethash(int fd,SHA256_CTX crt,DataFile *fileBuf);
MetaDir* list_directory (char* dirname);
#endif