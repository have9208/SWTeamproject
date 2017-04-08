#ifndef __CLIENT_FILE_H__
#define __CLIENT_FILE_H__

#define BUFF_SIZE 1024

typedef struct datFileType{
    char *file;
    int fileSize;
}datFile;

datFile* readFile();

#endif