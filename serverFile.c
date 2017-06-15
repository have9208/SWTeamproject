#include "serverFile.h"

void checkFile(SHA256_CTX *ctx,RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    char tmpFile[256] = "tmp/";
    char mkdirCmd[256] = "mkdir -p ";
    sha256_init(ctx);
    strcpy(RDI->servHash, "");
    strncat(pathFile,RDI->fileMeta.parent,strlen(RDI->fileMeta.parent));

    if(RDI->fileMeta.type==DIR_TYPE)
    {
        strncat(mkdirCmd,tmpFile,strlen(tmpFile)); // Make tmp directory
        system(mkdirCmd);
        strcpy(mkdirCmd,"mkdir -p ");
        strncat(mkdirCmd,pathFile,strlen(pathFile));
        strcat(mkdirCmd,"/");
        strncat(mkdirCmd, RDI->fileMeta.fileName, strlen(RDI->fileMeta.fileName));   
        if(system(mkdirCmd)) //When directory name is same as file name
        {
            printError("Directory name is same as file name in CWD"); // MKDIR_ERR
            RDI->error = 1;
        }
        else
        {
            RDI->error = 0;
            printDelete("change META");
        }     
        RDI->type=META;
    }
    else if(RDI->fileMeta.type==FILE_TYPE)
    {   
        DIR* isdir = opendir(RDI->pathFile);
        if (isdir)   // When file name is same as directory name
        {
            printError("File name is same as directory name in CWD"); // CREATE_ERR
            RDI->error = 1;
            RDI->type=META;
        }
        else
        {
            RDI->error = 0;
            printDelete("change DATA");
            RDI->type=DATA;
            strcat(pathFile,"/");
            strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
            strncat(tmpFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
            strcpy(RDI->pathFile,pathFile);
            strcpy(RDI->tmpFile,tmpFile);
            
            if ( 0 == access( RDI->pathFile, F_OK))
            {
                printError("There are existed file.");
                RDI->fileDescriptor = open(RDI->pathFile, O_RDWR | O_CREAT | O_APPEND, 0644);
                RDI->fileSize = (int)lseek(RDI->fileDescriptor,0,SEEK_END);
                int offset = RDI->fileSize / BLOCK_SIZE;
                
                lseek(RDI->fileDescriptor,0,SEEK_SET);
                for(int i=0;i<offset;i++)
                {
                    read(RDI->fileDescriptor,RDI->buffer,BLOCK_SIZE);
                    sha256_update(ctx, RDI->buffer, BLOCK_SIZE);
                }
                if(RDI->size % BLOCK_SIZE != 0)
                {
                    read(RDI->fileDescriptor,RDI->buffer,RDI->fileMeta.size % (BLOCK_SIZE));
                    sha256_update(ctx, RDI->buffer, RDI->fileMeta.size % (BLOCK_SIZE));
                }             
                sha256_final(ctx, RDI->servHash);
                sha256_init(ctx);  
                RDI->fileSize = -1;
                RDI->type = CHK;
                printHash(RDI->servHash);
            }
            else if ((RDI->fileDescriptor = open( RDI->tmpFile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
            {
                printError("There are existed canceled file.");
                RDI->fileDescriptor = open(RDI->tmpFile, O_RDWR | O_CREAT | O_APPEND, 0644);
                RDI->fileSize = (int)lseek(RDI->fileDescriptor,0,SEEK_END);
                int offset = RDI->fileSize / BLOCK_SIZE;
                lseek(RDI->fileDescriptor,0,SEEK_SET);
                for(int i=0;i<offset;i++)
                {
                    read(RDI->fileDescriptor,RDI->buffer,BLOCK_SIZE);
                    sha256_update(ctx, RDI->buffer, BLOCK_SIZE);
                }
                if(offset==0)
                {
                    read(RDI->fileDescriptor,RDI->buffer,RDI->fileSize);
                    sha256_update(ctx, RDI->buffer, RDI->fileSize);
                } 
                RDI->type = CHK;
                sha256_final(ctx, RDI->servHash);
                sha256_init(ctx);  
                printHash(RDI->servHash);
            }
            
        }        
    }
}

void writeFile(SHA256_CTX *ctx,RecievedDataInfo *RDI)
{
    write( RDI->fileDescriptor, RDI->buffer, RDI->size);
    sha256_update(ctx, RDI->buffer, RDI->size); 
    if( RDI->currentSize >= RDI->fileMeta.size )
    {
        rename(RDI->tmpFile,RDI->pathFile);
        printDelete("change INTE");
        RDI->type = INTE;
        sha256_final(ctx, RDI->servHash);
        printNotice("end load data");       
    }
}
char checkHash(RecievedDataInfo *RDI)
{
    char boolean = (char)(memcmp(RDI->servHash, RDI->cliHash, HASH_SIZE) == 0);
    RDI->type=META;
    if(boolean == 0)
    {
        printNotice("integrity fail.");
        close(RDI->fileDescriptor);
        remove(RDI->pathFile);
        return 0;
    }
    else
    {
        printNotice("integrity success.");
        close(RDI->fileDescriptor); 
        return 1;
    }
}



void verifyFile(RecievedDataInfo *RDI)
{
    // Argu List : error ,  verified, ignore
    if(RDI->buffer[0] == REWRITE ) // When file is corrupted ( file overwrite, rewrite )
    {
        printf("REWRITE\n");
        close(RDI->fileDescriptor);
        remove(RDI->tmpFile);
        RDI->fileDescriptor = open(RDI->tmpFile, O_RDWR | O_CREAT , 0644);
        RDI->type=DATA;
    }
    else if(RDI->buffer[0] == APPEND) // When file keep going to download (with tmp file)
    {    
        int offset = RDI->fileSize / BLOCK_SIZE;
        printf("APPEND\n");
        lseek(RDI->fileDescriptor,RDI->fileSize,SEEK_SET);
        RDI->currentSize = RDI->fileSize;
        RDI->type=DATA;    
    }
    else if(RDI->buffer[0] == IGNORE) // When file is already existed and ignored
    {   
        printf("IGNORE\n");
        close(RDI->fileDescriptor);
        RDI->type=META; 
    }    
}

void deleteFile(RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    char cmd[256] = "rm -rf ";
    strncat(pathFile,RDI->fileMeta.parent,strlen(RDI->fileMeta.parent));
    strcat(pathFile,"/");
    strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
    strncat(cmd,pathFile,strlen(pathFile));
    system(cmd);
}

void getList(RecievedDataInfo *RDI)
{
    DIR *dir_info;
    struct dirent *dir_entry;
    dir_info = opendir( "data/");
    strcpy(RDI->buffer, "");
    if ( NULL != dir_info)
    {
        while( dir_entry   = readdir( dir_info))
        {
            if(!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, ".."))
            {
            }
            else
            {
                strncat(RDI->buffer,dir_entry->d_name,strlen(dir_entry->d_name));
                strcat(RDI->buffer," ");
                printf( "%s\n", dir_entry->d_name);
            }      
        }
        RDI->size = strlen(RDI->buffer);
        closedir( dir_info);
    }
}