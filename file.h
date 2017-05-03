#ifndef __FILE_H__
#define __FILE_H__

#define MAX_FILE_NAME_LENGTH 256

enum fileType { FILE_TYPE = 1, DIR_TYPE = 2 };

typedef struct FileMetadata
{
    int size;
    enum fileType type;
    char parent[MAX_FILE_NAME_LENGTH];
    char fileName[MAX_FILE_NAME_LENGTH];
} FileMetadata;

#endif /* FILE_H */