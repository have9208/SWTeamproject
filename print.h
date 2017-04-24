#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdio.h>
#include <stdlib.h>

void printNotice(char *msg);
void printError(char *msg);
void printAdd(char *msg);
void printDelete(char *msg);
void printClientHelp(char* name);

void printSpeedByte(struct timeval start, struct timeval end, int size);

#endif /* PRINT_H */