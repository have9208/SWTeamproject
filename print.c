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

void printClientHelp(char* name)
{
    printf("Usage: %s\n", name);
    puts("  -l: Listenr server IP");
    puts("  -f: File name and directory name");
    puts("  -p: Sending protocol (default: UDP)");
    puts("      UDP or TCP select");
    
    exit(1);
}

void printSpeedByte(struct timeval start, struct timeval end, int size)
{
    long long byte;
    char unit = '\0';
    int rest = 0;

    double p;

    byte = end.tv_usec - start.tv_usec;
    byte += ((end.tv_sec - start.tv_sec) * 1000000);
    byte = size * (1000000 / byte);

    if (byte >= 1024)
    {
        rest = byte % 1024;
        byte /= 1024;
        unit = 'K';
    }

    if (byte >= 1024)
    {
        rest = byte % 1024;
        byte /= 1024;
        unit = 'M';
    }

    p = byte + ((double)rest / 1024);

    printf("%.1f%cB/s         \r", p, unit);
}