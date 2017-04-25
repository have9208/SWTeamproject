#include "serverFile.h"

int checkFile(char *directoryPath,char *fileBuffer,char *fileName, int fileSize)
{
    int fileDescriptor;
    char pathFile[256] = "data/";
    char mkdirCmd ="mkdir -p ";
    strncat(pathFile,directoryPath,strlen(directoryPath));
    strncat(mkdirCmd,directoryPath,strlen(directoryPath));
    system(mkdirCmd);
	strncat(pathFile,fileName,strlen(fileName));
    fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644)))
    if(fileDescriptor == -1)
    {
        //printError("File is already existed");
    }
    return fileDescriptor;
}

void writeFile(ReceivedDataInfo *RDI,int fileDescriptor,char *fileBuffer,char *fileName, int size)
{
    write( fileDescriptor, fileBuffer, size);
    sha256_update(RDI.ctx,buffer, size);
    RDI.currentSize += size;
}