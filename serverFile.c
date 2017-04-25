#include "serverFile.h"

void checkFile(RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    char mkdirCmd[256] = "mkdir -p ";
    if (strcmp(RDI->fileMeta.parent,""))
    {
        strcat(pathFile,"/");
    }
    
    //TO DO
    //Directory Temp Path
    strncat(pathFile,RDI->fileMeta.parent,strlen(RDI->fileMeta.parent));
    
    if(RDI->fileMeta.type==DIR_TYPE)
    {
        strncat(mkdirCmd,pathFile,strlen(pathFile));
        system(mkdirCmd);
        RDI->type=META;
    }
    else if(RDI->fileMeta.type==FILE_TYPE)
    {
        RDI->type=DATA;
        strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
        RDI->fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644);
        if(RDI->fileDescriptor == -1)
        {
            printError("File is already existed");
        }
    }
    
}

void writeFile(SHA256_CTX *ctx,RecievedDataInfo *RDI)
{
    if(RDI->currentSize==0)
    {
        sha256_init(ctx);
        strcpy(RDI->servHash, "");
         printf("00000000\n");
    }
    printf("11111111\n");
    write( RDI->fileDescriptor, RDI->buffer, RDI->size);
    printf("fffffff\n");
    printf("buffer size : %d\n",RDI->size);
    printf("buffer : %s\n",RDI->buffer);
    sha256_update(ctx,RDI->buffer, RDI->size);
    
    RDI->currentSize += RDI->size;
    if(RDI->type == INTE)
    {
         printf("333333333\n");
        sha256_final(ctx, RDI->servHash);
         printf("4444444444\n");
        printNotice("end load file");
        close(RDI->fileDescriptor);
    }
}

void deleteFile(RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    //TO DO
    //Directory Temp Path
    strncat(pathFile,RDI->fileMeta.parent,strlen(RDI->fileMeta.parent));
	strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
    printError("Hash Error : recieved file is deleted");
    remove(pathFile);
}