#ifndef __SERVER_FILE_H__
#define __SERVER_FILE_H__

void createFile(char *FileBuffer,char *FileName, int FileSize);
int writeFile(char *FileBuffer,char *FileName, int FileSize);
char *writeError();

#endif
