#include "print.h"

void printNotice(char *msg)
{
    printf("[*] ");
    printf("%s\n", msg);
}

void printError(char *msg)
{
    printf("\x1B[31m[!]\x1B[0m ");
    printf("%s\n", msg);
}

void printAdd(char *msg)
{
    printf("\x1B[32m[+]\x1B[0m ");
    printf("%s\n", msg);
}

void printDelete(char *msg)
{
    printf("\x1B[33m[-]\x1B[0m ");
    printf("%s\n", msg);
}