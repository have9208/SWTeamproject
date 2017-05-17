#include "clientFile.h"

unsigned char* getHash(int *fd,int fileSize)
{
    SHA256_CTX ctx;
    unsigned char *hash;
    
    hash =(unsigned char*)malloc(sizeof(unsigned char)*HASH_SIZE);
    
    ctx = getHashCtx(fd,ctx,fileSize);
    sha256_final(&ctx,hash);
    
    return hash;
}

char* fileSequenceChk(int *fd,int sequence,int fileSize,bool lastFile)
{
    char *file;
    int offset = 0;
    
    if(lastFile == true)
    {
        offset = sequence * BLOCK_SIZE;
    }
    else
    {
        offset = sequence * fileSize;
    }
    
    file = (char*)malloc(sizeof(char)*BLOCK_SIZE);
    memset(file,0,sizeof(char)*BLOCK_SIZE);
    
    lseek(*fd,offset,SEEK_SET);
    
    if(read(*fd, file, BLOCK_SIZE) == -1)
    {
        printError("read() error \n");
        close(*fd);
        return -1;
    }
    
    return file;
}

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

int getDirectoryFileCount(char *dirName)
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
    int count = 0, i = 0;

    MetaDir *dir;


    if (!isDir(dirName))
    {
        printError("디렉토리가 아님");
        return 0;
    }

    dir = (MetaDir*)malloc(sizeof(MetaDir));

    count = getDirectoryFileCount(dirName);
    strcpy(dir->path, dirName);
    dir->files = (DataFile*)malloc(sizeof(DataFile) * count);
    dir->childs = count;
        
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

SHA256_CTX getHashCtx(int *fd,SHA256_CTX ctx,int fileSize)
{
    int ret,size, block = BUFF_SIZE;
    
    size = 0;
    
    lseek(*fd,0,SEEK_SET);
    sha256_init(&ctx);
    char buf[BUFF_SIZE];
    
    while(1)
    {
        if (size + BUFF_SIZE > fileSize)
        {
            block = fileSize % BUFF_SIZE;
        }
        
        ret = read(*fd, buf, block);
        size += block;
        
        if(ret > 0)
        {
            sha256_update(&ctx,buf,ret);
        }
        else
        {
            break;
        }
        
        if (size >= fileSize) 
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
        return -1;
    }
    
    return fd;
}