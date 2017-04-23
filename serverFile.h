#ifndef __SERVER_FILE_H__
#define __SERVER_FILE_H__

#include "file.h"
#include <stdio.h>         // puts()
#include <string.h>        // strlen()
#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()
#include <sys/types.h>
#include <sys/stat.h>

void createFile(char *fileBuffer,char *fileName, int fileSize);
int writeFile(int fileDescriptor,char *fileBuffer,char *fileName, int fileSize);
char *writeError();

#define DBL_INT_ADD(a,b,c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

typedef struct SHA256_CTX SHA256_CTX;

void sha256_transform(SHA256_CTX *ctx, unsigned char data[]);
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, unsigned char data[], unsigned int len);
void sha256_final(SHA256_CTX *ctx, unsigned char hash[]);
void calculate_sha256(unsigned char data[], unsigned int len,unsigned char hash[]);

#endif
