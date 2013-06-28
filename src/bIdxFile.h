#ifndef  BIDX_FILE_H
#define  BIDX_FILE_H

#include "bIdxDef.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>




#pragma pack(push)
#pragma pack(4)

typedef struct
{
    size_t maxbit;
    int magic;
}bIdxFileHead;
#pragma pack(pop)

typedef struct
{
    int fd;
    size_t fsize;
    
    baddr* mem;
    size_t msize;
    bIdxFileHead* head;
    
}bIdxFile;

#define bIdxFile_set_maxbit(pf, pos) do{if((pf)->maxbit < (pos))(pf)->maxbit =(pos); }while(FALSE);

#define bIdxFile_get_maxbit(pf) ((pf)->maxbit)

#define BIDX_FHEAD_SIZE 4096
#define BIDX_FILE_MAGIC 741596357
#define BIDX_APPEND_MAXUNIT (1024*1024*10) //10 MB

bIdxFile* bIdxFile_open(char* path);

bbool bIdxFile_append(bIdxFile* bFile, size_t fsize);

int bIdxFile_get_fd(bIdxFile* bFile);

size_t bIdxFile_get_size(bIdxFile* bFile);

baddr* bIdxFile_get_map_addr(bIdxFile* bFile);

bIdxFileHead* bIdxFile_get_head(bIdxFile* bFile);

bbool bIdxFile_close(bIdxFile* bFile);



#endif 
