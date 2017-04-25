#include "serverFile.h"

void checkFile(RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    char mkdirCmd[256] = "mkdir -p ";
    int test =0;
    //TO DO
    //Directory Temp Path
    char directoryPath[30] = "/test/test2/";
    strncat(pathFile,directoryPath,strlen(directoryPath));
    
    if(test==0)
    {
        strncat(mkdirCmd,pathFile,strlen(pathFile));
        system(mkdirCmd);
        RDI->type=META;
    }
    else if(test==1)
    {
        strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
        RDI->fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644);
        if(RDI->fileDescriptor == -1)
        {
            printError("File is already existed");
        }
    }
    
}

void writeFile(RecievedDataInfo *RDI)
{
    if(RDI->currentSize==0)
    {
        sha256_init(&RDI->ctx);
        strcpy(RDI->servHash, "");
    }
    write( RDI->fileDescriptor, RDI->buffer, RDI->size);
    sha256_update(&RDI->ctx,RDI->buffer, RDI->size);
    RDI->currentSize += size;
    if(RDI->type == INTE)
    {
        sha256_final(&RDI->ctx, RDI->servHash);
        printNotice("end load file");
        close(RDI->fileDescriptor);
    }
}

void deleteFile(RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    //TO DO
    //Directory Temp Path
    char directoryPath[30] = "/test/test2/";
    strncat(pathFile,directoryPath,strlen(directoryPath));
	strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
    printError("Hash Error : recieved file is deleted");
    remove(pathFile);
}