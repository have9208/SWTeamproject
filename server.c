#include <stdio.h>
#include <string.h>
#include "server.h"

char *buffer[] = {
    "hello world\n","new World","struct SocketInfo sockInfo;\n"
};

int main(int argc, char *argv[])
{
    /*
    struct SocketInfo sockInfo;
    char *buf;
    int size;
    
    serverSocket(&sockInfo);

    while(receive(&sockInfo, buf, &size))
    {
        //do something with file function(buf, size)
    }
    */

    char *buf; 
	char fileName = "test.txt";
    int size;
    printf("filename: %s\n",fileName);
    for(int i=0; i<3; i++)
    {
        printf(buffer[i]);
        size = strlen(buffer[i]);
        writeFile(buffer[i], fileName, size);
    }

    return 0;
}
