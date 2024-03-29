#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include "sha256.h"

void printNotice(char *msg);
void printError(char *msg);
void printAdd(char *msg);
void printDelete(char *msg);
void printClientHelp(char* name);

void printSpeedByte(struct timeval start, struct timeval end, int size, int nowsize, int maxsize);

int getConsoleWidth();
void clearLine();
void printHash(char* hash);

#endif /* PRINT_H */