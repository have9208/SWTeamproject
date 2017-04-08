#ifndef __FILE_H__
#define __FILE_H__

#define MAX_FILE_NAME_LENGTH 256

typedef struct FileMetadata
{
    int size;
    char fileName[MAX_FILE_NAME_LENGTH];
} FileMetadata;

#endif /* FILE_H */