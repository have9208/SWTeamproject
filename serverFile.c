#include "serverFile.h"

void checkFile(SHA256_CTX *ctx,RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    char mkdirCmd[256] = "mkdir -p ";
    sha256_init(ctx);
    strcpy(RDI->servHash, "");
    //TO DO
    //Directory Temp Path
    strncat(pathFile,RDI->fileMeta.parent,strlen(RDI->fileMeta.parent));
    
    if(RDI->fileMeta.type==DIR_TYPE)
    {
        strncat(mkdirCmd,pathFile,strlen(pathFile));
        strcat(mkdirCmd,"/");
        strncat(mkdirCmd, RDI->fileMeta.fileName, strlen(RDI->fileMeta.fileName));
        printAdd(mkdirCmd);
        system(mkdirCmd);
        printDelete("change META");
        RDI->type=META;
    }
    else if(RDI->fileMeta.type==FILE_TYPE)
    {
        char tmpExtension[12] = ".tmp";
        char tmpFile[256];
        printDelete("change DATA");
        RDI->type=DATA;
        strcat(pathFile,"/");
        strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
        strcat(RDI->pathFile,pathFile);

        strcpy(tmpFile,pathFile);
        strncat(tmpFile,tmpExtension,strlen(tmpExtension));
        printAdd(tmpFile);
        strcat(RDI->tmpFile,tmpFile);
        RDI->fileDescriptor = open( tmpFile, O_WRONLY | O_CREAT | O_EXCL, 0644);
        if((RDI->fileDescriptor = open( tmpFile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1) // tmp file is already existed
        {
            printError("There are existed canceled file.");
            RDI->fileDescriptor = open(RDI->tmpFile, O_RDWR | O_CREAT | O_APPEND, 0644);
            RDI->fileSequence = (int)lseek(RDI->fileDescriptor,0,SEEK_END) / BLOCK_SIZE;
            lseek(RDI->fileDescriptor,0,SEEK_SET);
            for(int i=0;i<RDI->fileSequence;i++)
            {
                read(RDI->fileDescriptor,RDI->buffer,BLOCK_SIZE);
                sha256_update(ctx, RDI->buffer, BLOCK_SIZE);
            }
            sha256_final(ctx, RDI->servHash);
            sha256_init(ctx);  
            RDI->type = CHK;
        }
        else if((RDI->fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
        {
            //TO DO
            //When file is already existed
            printError("There are existed file.");
            //close(RDI->fileDescriptor);

            RDI->type = CHK;
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
        close(RDI->fileDescriptor);
        printNotice("end load data");
    }
}

void verifyFile(RecievedDataInfo *RDI)
{
    if(strcmp(RDI->buffer,"error")==0)
    {
        lseek(RDI->fileDescriptor,0,SEEK_SET);
        RDI->type=DATA;
    }
    else if(strcmp(RDI->buffer,"overwrite")==0)
    {
        RDI->fileDescriptor = open( RDI->pathFile, O_WRONLY | O_CREAT | O_TRUNC , 0644);
        RDI->type=DATA;
    }
    else if(strcmp(RDI->buffer,"verified")==0)
    {
        lseek(RDI->fileDescriptor,RDI->fileSequence*BLOCK_SIZE,SEEK_SET);
        RDI->currentSize = RDI->fileSequence * BLOCK_SIZE;
        RDI->type=DATA;
    }
    else if(strcmp(RDI->buffer,"ignore")==0)
    {
        RDI->type=INTE;
    }    
}