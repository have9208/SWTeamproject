#include "serverFile.h"

int checkFile(char *directoryPath,char *fileBuffer,char *fileName)
{
    int fileDescriptor;
    char pathFile[256] = "data/";
    char mkdirCmd[256] ="mkdir -p ";
    strncat(pathFile,directoryPath,strlen(directoryPath));
    strncat(mkdirCmd,directoryPath,strlen(directoryPath));
    system(mkdirCmd);
	strncat(pathFile,fileName,strlen(fileName));
    fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if(fileDescriptor == -1)
    {
        printError("File is already existed");
    }
    return fileDescriptor;
}

int writeFile(RecievedDataInfo *RDI,int fileDescriptor,char *fileBuffer,char *fileName, int size)
{
    if(RDI->currentSize==0)
    {
        sha256_init(&RDI->ctx);
        strcpy(RDI->servHash, "");
    }
    write( fileDescriptor, fileBuffer, size);
    sha256_update(&RDI->ctx,fileBuffer, size);
    RDI->currentSize += size;
    if(RDI->fileMeta.size <= RDI->currentSize)
    {
        sha256_final(&RDI->ctx, RDI->servHash);
        printNotice("end load file");
        return 0;
    }
    return 1;
}