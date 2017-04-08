#include "serverFile.h"

#include <stdio.h>         // puts()
#include <string.h>        // strlen()
#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()

void createFile(char *FileBuffer,char *FileName, int FileSize)
{
    if(!writeFile(FileBuffer,FileName,FileSize))
    {
        writeError();
    }
}

int writeFile(char *FileBuffer,char *FileName, int FileSize)
{
    int  FileDescriptor;
    if ( 0 < ( FileDescriptor = open( FileName, O_WRONLY | O_CREAT | O_EXCL, 0644)))
    {
        write( FileDescriptor, FileBuffer, FileSize);
        close(FileDescriptor);
        return FileDescriptor;
    }
    else
    {
        return 0;
    }
}

char *writeError()
{
    char *ErrorMessage = "[!] File Write Error";
    printf( "%s\n",ErrorMessage);
    return ErrorMessage;
}
