#include "serverFile.h"

int checkFile(char *fileBuffer,char *fileName, int fileSize)
{
    int fileDescriptor;
    mkdir("./data/", 0777);
	char pathFile[256] = "./data/";
	strncat(pathFile,fileName,strlen(fileName));

    if ( 0 < ( fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644)))
    {
        return fileDescriptor;
    }
    else
    {
        return fileDescriptor;
    }
}

int writeFile(int fileDescriptor,char *fileBuffer,char *fileName, int fileSize)
{
	mkdir("./data/", 0777);
	char pathFile[256] = "./data/";
	strncat(pathFile,fileName,strlen(fileName));

    if(fileDescriptor)
    {
        write( fileDescriptor, fileBuffer, fileSize);
    }
    else
    {
        printError("File is already existed!");
    }
}