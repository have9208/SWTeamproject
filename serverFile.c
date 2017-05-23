#include "serverFile.h"

void checkFile(SHA256_CTX *ctx,RecievedDataInfo *RDI)
{
    char pathFile[256] = "data/";
    char tmpFile[256] = "tmp/";
    char mkdirCmd[256] = "mkdir -p ";
    printf("HASH INIT_checkFile\n");
    sha256_init(ctx);
    strcpy(RDI->servHash, "");
    strncat(pathFile,RDI->fileMeta.parent,strlen(RDI->fileMeta.parent));

    if(RDI->fileMeta.type==DIR_TYPE)
    {
        strncat(mkdirCmd,tmpFile,strlen(tmpFile)); // Make tmp directory
        system(mkdirCmd);

        strcpy(mkdirCmd,"mkdir -p ");
        strncat(mkdirCmd,pathFile,strlen(pathFile));
        strcat(mkdirCmd,"/");
        strncat(mkdirCmd, RDI->fileMeta.fileName, strlen(RDI->fileMeta.fileName));
        printAdd(mkdirCmd);      
        if(system(mkdirCmd)) //When directory name is same as file name
        {
            printError("Directory name is same as file name in CWD"); // MKDIR_ERR
            RDI->error = 1;
        }
        else
        {
            RDI->error = 0;
            printDelete("change META");
        }     
        RDI->type=META;
    }
    else if(RDI->fileMeta.type==FILE_TYPE)
    {   
        DIR* isdir = opendir(RDI->pathFile);
        if (isdir)   // When file name is same as directory name
        {
            printError("File name is same as directory name in CWD"); // CREATE_ERR
            RDI->error = 1;
            RDI->type=META;
        }
        else
        {
            RDI->error = 0;
            printDelete("change DATA");
            RDI->type=DATA;
            strcat(pathFile,"/");
            strncat(pathFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
            strncat(tmpFile,RDI->fileMeta.fileName,strlen(RDI->fileMeta.fileName));
            strcpy(RDI->pathFile,pathFile);
            strcpy(RDI->tmpFile,tmpFile);
            
            if ( 0 == access( RDI->pathFile, F_OK))
            {
                printError("There are existed file.");
                RDI->fileDescriptor = open(RDI->pathFile, O_RDWR | O_CREAT | O_APPEND, 0644);
                RDI->fileSequence = (int)lseek(RDI->fileDescriptor,0,SEEK_END);
                int offset = RDI->fileSequence / BLOCK_SIZE;
                printf("File Name ==> %s\n",RDI->pathFile);
                printf("File Sequence ==> %d\n",RDI->fileSequence);
                printf("File size ==>%d\n",RDI->size);
                lseek(RDI->fileDescriptor,0,SEEK_SET);
                for(int i=0;i<offset;i++)
                {
                    read(RDI->fileDescriptor,RDI->buffer,BLOCK_SIZE);
                    //printf("---> %d\n",i);
                    sha256_update(ctx, RDI->buffer, BLOCK_SIZE);
                }
                if(RDI->size % BLOCK_SIZE != 0)
                {
                    //printf("size ---> %d \n",RDI->fileMeta.size % (BLOCK_SIZE));
                    read(RDI->fileDescriptor,RDI->buffer,RDI->fileMeta.size % (BLOCK_SIZE));
                    //printf("read data ===> %s \n",RDI->buffer);
                    sha256_update(ctx, RDI->buffer, RDI->fileMeta.size % (BLOCK_SIZE));
                }             
                sha256_final(ctx, RDI->servHash);
                sha256_init(ctx);  
                RDI->fileSequence = -1;
                RDI->type = CHK;
                printHash(RDI->servHash);
            }
            else if ((RDI->fileDescriptor = open( RDI->tmpFile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
            {
                //printf("Tmp file path : %s\n",tmpFile);
                printError("There are existed canceled file.");
                RDI->fileDescriptor = open(RDI->tmpFile, O_RDWR | O_CREAT | O_APPEND, 0644);
                RDI->fileSequence = (int)lseek(RDI->fileDescriptor,0,SEEK_END);
                int offset = RDI->fileSequence / BLOCK_SIZE;
                lseek(RDI->fileDescriptor,0,SEEK_SET);
                for(int i=0;i<offset;i++)
                {
                    read(RDI->fileDescriptor,RDI->buffer,BLOCK_SIZE);
                    sha256_update(ctx, RDI->buffer, BLOCK_SIZE);
                }
                if(offset==0)
                {
                    read(RDI->fileDescriptor,RDI->buffer,RDI->fileSequence);
                    sha256_update(ctx, RDI->buffer, RDI->fileSequence);
                } 
                RDI->type = CHK;
                sha256_final(ctx, RDI->servHash);
                sha256_init(ctx);  
                printHash(RDI->servHash);
            }
            
        }        
    }
}

void writeFile(SHA256_CTX *ctx,RecievedDataInfo *RDI)
{
    write( RDI->fileDescriptor, RDI->buffer, RDI->size);

    printf("Write Buffer =======> %02x\n",RDI->buffer[0]);
    sha256_update(ctx, RDI->buffer, RDI->size); 
    printf("RDI->size : %d\n",RDI->size);
    printf("currentSize : %d\n",RDI->currentSize);
    printf("fileSize : %d\n",RDI->fileMeta.size);
    printf("name : %s\n",RDI->fileMeta.fileName);
    if( RDI->currentSize >= RDI->fileMeta.size )
    {
        rename(RDI->tmpFile,RDI->pathFile);
        printDelete("change INTE");
        RDI->type = INTE;
        sha256_final(ctx, RDI->servHash);
        //close(RDI->fileDescriptor);
        printNotice("end load data");
    }
}

void verifyFile(RecievedDataInfo *RDI)
{
    // Argu List : error ,  verified, ignore
    //printf("buffer = %d\n",RDI->buffer[0]);
    if(RDI->buffer[0] == REWRITE ) // When file is corrupted ( file overwrite, rewrite )
    {
        printf("REWRITE\n");
        close(RDI->fileDescriptor);
        remove(RDI->tmpFile);
        RDI->fileDescriptor = open(RDI->tmpFile, O_RDWR | O_CREAT , 0644);
        RDI->type=DATA;
    }
    else if(RDI->buffer[0] == APPEND) // When file keep going to download (with tmp file)
    {    
        int offset = RDI->fileSequence / BLOCK_SIZE;
        printf("APPEND\n");
        lseek(RDI->fileDescriptor,RDI->fileSequence,SEEK_SET);
        RDI->currentSize = RDI->fileSequence;
        RDI->type=DATA;    
    }
    else if(RDI->buffer[0] == IGNORE) // When file is already existed and ignored
    {   
        printf("IGNORE\n");
        close(RDI->fileDescriptor);
        RDI->type=META; 
    }    
}