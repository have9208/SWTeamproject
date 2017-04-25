#include "clientFile.h"

MetaDir* list_directory (char* dirname)
{
    DIR* dp;
    struct dirent*  dirp;
    struct stat     fstat;
    char buff[BUFF_SIZE];
    DataFile *fileBuf;
    MetaDir *dirBuf;
    int count = 0;

    dirBuf = (MetaDir*)malloc(sizeof(MetaDir) * DIR_SIZE);
        
    dp = opendir(dirname);
    if (!dp)
    {
        printError("open direcgory error");
        exit(1);
    }
 
    chdir(dirname);
    
    while ((dirp = readdir(dp)) != NULL)
    {
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
        {
            continue;
        }

        if(stat(dirp->d_name, &fstat) == -1 )
        {
            printError("stat file error");
            continue;
        }

        if( S_ISDIR(fstat.st_mode) )
        {

            dirBuf[count].dir =true;
            getcwd(dirBuf[count].path,BUFF_SIZE);
            count++;
            list_directory( dirp->d_name );
        }
        else
        {
            dirBuf[count].dir =false;
            fileBuf = readFile(dirp->d_name);
            dirBuf[count].fileBuf = *fileBuf;
            getcwd(dirBuf[count].path,BUFF_SIZE);
            count++;
        } 
    }
    closedir(dp);

    chdir("..");
    
    return dirBuf;
}

DataFile* readFile(char *fileName)
{
    int fd;
    char buf[BUFF_SIZE];
    DataFile *fileBuf;
    SHA256_CTX ctx;
    
    fileBuf = (DataFile*)malloc(sizeof(DataFile));
    memset(fileBuf,0,sizeof(DataFile));
    
    fd = openFile(fileName);
    
    fileBuf->fileSize = getFileSize(fd);
    
    ctx = gethash(fd, ctx, fileBuf);
    sha256_final(&ctx,fileBuf->hash);
    
    fileBuf->file = (char*)malloc(sizeof(char)*fileBuf->fileSize);
    memset(fileBuf->file,0,sizeof(char)*fileBuf->fileSize);
    
    lseek(fd,0,SEEK_SET);
    
    if(read(fd, fileBuf->file,fileBuf->fileSize) == -1)
    {
        printError("read() error \n");
    }
    
    close(fd);
    
    return fileBuf;
}
SHA256_CTX gethash(int fd,SHA256_CTX ctx,DataFile *fileBuf)
{
    int ret;
    
    lseek(fd,0,SEEK_SET);
    sha256_init(&ctx);
    char buf[BUFF_SIZE];
    while(1)
    {
        ret=read(fd, buf, BUFF_SIZE);
        if(ret > 0)
        {
            sha256_update(&ctx,buf,ret);
        }
        else
        {
            break;
        }
    }
    
    return ctx;
}

int openFile(char *fileName)
{
    int fd;
    if((fd = open(fileName,O_RDONLY)) < 0)
    {
        printError("open() error \n");
    }
    
    return fd;
}

int getFileSize(int fd)
{
    int ret,size;
    char buf[BUFF_SIZE];
    
    size = 0;
    
    while(1)
    {
        ret=read(fd, buf, BUFF_SIZE);
        if(ret > 0)
        {
            size += ret;
        }
        else
        {
            break;
        }
    }
    return size;
}