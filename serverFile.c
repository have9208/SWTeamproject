#include "serverFile.h"

#include <stdio.h>         // puts()
#include <string.h>        // strlen()
#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()

void createFile(char *fileBuffer,char *fileName, int fileSize)
{
    if(!writeFile(fileBuffer,fileName,fileSize))
    {
        writeError();
    }
}

int writeFile(char *fileBuffer,char *fileName, int fileSize)
{
    int  fileDescriptor;
    if ( 0 < ( fileDescriptor = open( fileName, O_WRONLY | O_CREAT | O_EXCL, 0644)))
    {
        write( fileDescriptor, fileBuffer, fileSize);
        close(fileDescriptor);
        return fileDescriptor;
    }
    else
    {
        return 0;
    }
}

char *writeError()
{
    char *errorMessage = "[!] File Write Error";
    printf( "%s\n",errorMessage);
    return errorMessage;
}
