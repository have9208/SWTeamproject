#ifndef __SERVER_FILE_H__
#define __SERVER_FILE_H__

void createFile(char *fileBuffer,char *fileName, int fileSize);
int writeFile(char *fileBuffer,char *fileName, int fileSize);
char *writeError();

#endif
