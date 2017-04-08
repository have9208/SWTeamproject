#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

datFile* readFile()
{
    int fd,ret;
    char buf[BUFF_SIZE];
    datFile *fileBuf;
    
    fileBuf = (datFile*)malloc(sizeof(datFile));
    memset(fileBuf,0,sizeof(datFile));
    
    fd = open("test.txt",O_RDONLY);
    if(fd == -1)
    {
        printf("open() error \n");
    }
    
    while(1)
    {
        ret=read(fd, buf, BUFF_SIZE);
        if(ret > 0)
        {
            printf("%d\n",ret);
            fileBuf->file_size += ret;
        }
        else
        {
            break;
        }
    }
    
    fileBuf->file = (char*)malloc(sizeof(char)*fileBuf->file_size);
    memset(fileBuf->file,0,sizeof(char)*fileBuf->file_size);
    
    lseek(fd,0,SEEK_SET);
    
    if(read(fd, fileBuf->file,fileBuf->file_size) == -1)
    {
        printf("read() error \n");
    }
    
    return 0;
}