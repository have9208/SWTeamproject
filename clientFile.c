#include "clientFile.h"
int openFile(char *fileName)
{
    int fd;
    if(fd = open(fileName,O_RDONLY) < 0)
    {
        printf("open() error \n");
    }
    
    return fd;
}

int getFileSize(int fd)
{
    int ret;
    char buf[BUFF_SIZE];
    DataFile size;
    
    
    while(1)
    {
        ret=read(fd, buf, BUFF_SIZE);
        if(ret > 0)
        {
            printf("%d\n",ret);
            size.fileSize += ret;
        }
        else
        {
            break;
        }
    }
    return size.fileSize;
}
DataFile* readFile(char *fileName)
{
    int fd;
    char buf[BUFF_SIZE];
    DataFile *fileBuf;
    
    fileBuf = (DataFile*)malloc(sizeof(DataFile));
    memset(fileBuf,0,sizeof(DataFile));
    
    fd = openFile(fileName);
    
    fileBuf->fileSize = getFileSize(fd);
    
    
    fileBuf->file = (char*)malloc(sizeof(char)*fileBuf->fileSize);
    memset(fileBuf->file,0,sizeof(char)*fileBuf->fileSize);
    
    lseek(fd,0,SEEK_SET);
    
    if(read(fd, fileBuf->file,fileBuf->fileSize) == -1)
    {
        printf("read() error \n");
    }
    
    return 0;
}