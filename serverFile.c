#include "serverFile.h"

#include <stdio.h>         // puts()
#include <string.h>        // strlen()
#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()
#include <sys/types.h>
#include <sys/stat.h>


void createFile(char *fileBuffer,char *fileName, int fileSize)
{
    if(!writeFile(fileBuffer,fileName,fileSize))
    {
        //writeError();
    }
}

int writeFile(char *fileBuffer,char *fileName, int fileSize)
{
	mkdir("./data/", 0777);
	char pathFile[256] = "./data/";
	strncat(pathFile,fileName,strlen(fileName));
    int  fileDescriptor;
	
    if ( 0 < ( fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_EXCL, 0644)))
    {
        write( fileDescriptor, fileBuffer, fileSize);
        close(fileDescriptor);
        return fileDescriptor;
    }
    else
    {
		fileDescriptor = open( pathFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        write( fileDescriptor, fileBuffer, fileSize);
        close(fileDescriptor);
        return fileDescriptor;
    }
}

char *writeError()
{
    char *errorMessage = "[!] File is already exist";
    printf( "%s\n",errorMessage);
    return errorMessage;
}
