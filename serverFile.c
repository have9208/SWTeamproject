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
        //printError("File is already existed!");
    }
}

int main()
{
    unsigned char data[256]= "testtest";
    unsigned char hash[64] = "";
    unsigned int len = 12;
    
    char test1[1024] = "1234";
    char test2[1024] = "5678";
    
    SHA256_CTX ctx;
    sha256_init(&ctx);

    sha256_update(&ctx,test1,strlen(test1));
    sha256_update(&ctx,test1,strlen(test2));

    sha256_final(&ctx,hash);
    printf("hash : %s\n",hash);
    printf("hex : ");
         
    for(int i=0;i<strlen(hash);i++)
    {
        printf("%02X",hash[i]);
    }
    printf("\n");
    return 0;
}