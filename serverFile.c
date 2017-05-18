#include "serverFile.h"

void hashCalculate(SHA256_CTX *ctx,RecievedDataInfo *RDI)
{
    RDI->fileDescriptor = open(RDI->pathFile, O_RDWR | O_CREAT | O_APPEND, 0644);
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
        DIR* isdir = opendir(RDI->fileMeta.fileName);
        if (isdir)   // When file name is same as directory name
        {
            printError("File name is same as directory name in CWD");
            RDI->type=META;
        }
        else
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
            strcat(RDI->pathFile,tmpFile);
            if((RDI->fileDescriptor = open( tmpFile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1) // tmp file is already existed
            {
                printError("There are existed canceled file.");
                hashCalculate(ctx,RDI);
                
            }
            else if((RDI->fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
            {
                //TO DO
                //When file is already existed
                strcat(RDI->pathFile,pathFile);
                printError("There are existed file.");
                hashCalculate(ctx,RDI);
                read(RDI->fileDescriptor,RDI->buffer,RDI->fileMeta.size - (BLOCK_SIZE)*RDI->fileSequence);
                RDI->fileSequence = -1;
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
        char tmpFile[256];
        strcat(tmpFile,RDI->pathFile);
        RDI->pathFile[strlen(RDI->pathFile)-4]='\0';
        rename(tmpFile,RDI->pathFile);
        printDelete("change INTE");
        RDI->type = INTE;
        sha256_final(ctx, RDI->servHash);
        close(RDI->fileDescriptor);
        printNotice("end load data");
    }
}

void verifyFile(RecievedDataInfo *RDI)
{
    // Argu List : error ,  verified, ignore
    if(strcmp(RDI->buffer,"error")==0) // When file is corrupted (tmp? orign?)
    {
        close(RDI->fileDescriptor);
        remove(RDI->pathFile);
        open(RDI->pathFile, O_RDWR | O_CREAT | O_TRUNC , 0644);
        RDI->type=DATA;
    }
    else if(strcmp(RDI->buffer,"verified")==0) // When file keep going to download (with tmp file)
    {    
        lseek(RDI->fileDescriptor,RDI->fileSequence*BLOCK_SIZE,SEEK_SET);
        RDI->currentSize = RDI->fileSequence * BLOCK_SIZE;
        RDI->type=DATA;    
    }
    else if(strcmp(RDI->buffer,"ignore")==0) // When file is already existed and ignored
    {   
        close(RDI->fileDescriptor);
        RDI->type=META; 
    }    
}