#include "clientFile.h"

bool isDir(char *fileName)
{
    struct stat file;
    stat(fileName, &file);
    
    if(S_ISREG(file.st_mode))
    {
        return false;
    }
    else if(S_ISDIR(file.st_mode))
    {
        return true;
    }
    else
    {
        return false;
    }
}

int getDirectoryLength(char *dirName)
{
    DIR* dp;
    struct dirent*  dirp;
    struct stat     fstat;
    char orgPath[MAX_FILE_NAME_LENGTH] = "";
    int count = 0;

    if (!isDir(dirName))
    {
        printError("디렉토리가 아님");
        return 0;
    }
        
    dp = opendir(dirName);
    if (!dp)
    {
        printError("open direcgory error");
        exit(1);
    }
 
    getcwd(orgPath, MAX_FILE_NAME_LENGTH);
    chdir(dirName);
    
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

        count++;
    }

    closedir(dp);
    chdir(orgPath);
    
    return count;
}

MetaDir* listDirectory(char* dirName)
{
    DIR* dp;
    struct dirent*  dirp;
    struct stat     fstat;
    DataFile *fileBuf;
    char orgPath[MAX_FILE_NAME_LENGTH] = "";
    int c = 0, i = 0;

    MetaDir *dir;


    if (!isDir(dirName))
    {
        printError("디렉토리가 아님");
        return 0;
    }

    dir = (MetaDir*)malloc(sizeof(MetaDir));

    c = getDirectoryLength(dirName);
    strcpy(dir->path, dirName);
    dir->files = (DataFile*)malloc(sizeof(DataFile) * c);
    dir->childs = c;
        
    dp = opendir(dirName);
    if (!dp)
    {
        printError("open direcgory error");
        exit(1);
    }
 
    getcwd(orgPath, MAX_FILE_NAME_LENGTH);
    chdir(dirName);

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

        strcpy(dir->files[i].fileName, dirp->d_name);
        i++;
    }
    
    closedir(dp);
    chdir(orgPath);
    
    return dir;
}

void closeDataFile(DataFile* data)
{
    free(data->file);
}

void closeDirectory(MetaDir* dir)
{
    free(dir->files);
    free(dir);
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