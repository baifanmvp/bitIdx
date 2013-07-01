#include "bIdxFile.h"
#define _LARGE_FILE64_SOURCE
bIdxFile* bIdxFile_open(char* path)
{
    struct stat f_info;
    int fd = 0;
    
    if(access(path, F_OK) == 0)
    {
        if( (fd =open(path, O_RDWR|O_LARGEFILE) ) < 0 )
        {
            perror("打开文件失败!!!");
            exit(-1);
        }
        
        if(fstat(fd, &f_info) < 0)
        {
            perror("获取文件信息失败!!");
            exit(-1);
        }

        if(f_info.st_size < BIDX_FHEAD_SIZE)
        {
            perror("文件size is error!!");
            exit(-1);
        }

        bIdxFileHead st_head = {0};
        read(fd, &st_head, sizeof(st_head));
        if(st_head.magic != BIDX_FILE_MAGIC)
        {
            printf("magic num  is error!!");
            exit(-1);
        }
    }
    else
    {
        if( (fd =open(path, O_RDWR|O_CREAT, S_IRWXU) ) < 0 )
        {
            perror("打开文件失败!!!");
            exit(-1);
        }

        bIdxFileHead st_head = {0};
        st_head.magic = BIDX_FILE_MAGIC;
        write(fd, &st_head, sizeof(st_head));
        
        void* lp_file_buf = malloc(BIDX_FHEAD_SIZE - sizeof(st_head));
        write(fd, lp_file_buf, BIDX_FHEAD_SIZE - sizeof(st_head));
        free(lp_file_buf);
        
        
        if(fstat(fd, &f_info) < 0)
        {
            perror("获取文件信息失败!!");
            exit(-1);
        }
        
    }

    bIdxFile* pf = (bIdxFile*)malloc(sizeof(bIdxFile));
    pf->fd = fd;
    pf->fsize = f_info.st_size;
    pf->msize = pf->fsize - BIDX_FHEAD_SIZE;
    if(pf->msize)
    {
        pf->mem = (baddr*)mmap64(0, pf->fsize,
                              PROT_READ|PROT_WRITE, MAP_SHARED,
                              pf->fd, 0);
        
        if(  pf->mem ==  MAP_FAILED )
        {
            perror("文件映射不成功!!");
            exit(-1);
        }
    }
    else
    {
        pf->mem = 0;
    }

    pf->head = (bIdxFileHead*)mmap(0, sizeof(bIdxFileHead),
                              PROT_READ|PROT_WRITE, MAP_SHARED|MAP_LOCKED,
                              pf->fd, 0);
        
    return pf;
    
}

bbool bIdxFile_append(bIdxFile* bFile, size_t fsize)
{
    if(!bFile || !fsize)
    {
        printf("bFile is NULL  or fsize is 0\n");
        return FALSE;
    }
    if(-1 == munmap(bFile->mem, bFile->fsize))
    {
         printf("文件close映射不成功!!\n");
         exit(-1);
    }

    if(-1 == munmap(bFile->head, sizeof(bIdxFileHead)))
    {
         printf("文件close映射不成功!!\n");
         exit(-1);
    }


///////////////////////////////////////////////////////////
    lseek(bFile->fd, 0, SEEK_END);

    size_t n_append_modsize = fsize % BIDX_APPEND_MAXUNIT;
    size_t n_append_unit_cnt = fsize / BIDX_APPEND_MAXUNIT;

    if(n_append_unit_cnt)
    {
        void* lp_file_buf = malloc(BIDX_APPEND_MAXUNIT);
        memset(lp_file_buf, 0, BIDX_APPEND_MAXUNIT);

        while (n_append_unit_cnt)
        {
            write(bFile->fd, lp_file_buf, BIDX_APPEND_MAXUNIT);
            n_append_unit_cnt--;
        }
        free(lp_file_buf);
    }
    
    void* lp_file_buf = malloc(n_append_modsize);
    memset(lp_file_buf, 0, n_append_modsize);
    write(bFile->fd, lp_file_buf, n_append_modsize);
    free(lp_file_buf);

    ///////////////////////////////////////////////////////////

    
    bFile->fsize += fsize;
    bFile->msize = bFile->fsize - BIDX_FHEAD_SIZE;
    if(bFile->msize)
    {
        bFile->mem = (baddr*)mmap64(0, bFile->fsize,
                              PROT_READ|PROT_WRITE, MAP_SHARED,
                              bFile->fd, 0);
        
        if(bFile->mem ==  MAP_FAILED )
        {
            perror("文件映射不成功!!");
            exit(-1);
        }
    }
    else
    {
        bFile->mem = 0;
    }        
    return TRUE;
}

int bIdxFile_get_fd(bIdxFile* bFile)
{
    if(bFile)
        return bFile->fd;
    return 0;
}

size_t bIdxFile_get_size(bIdxFile* bFile)
{
    if(bFile)
        return bFile->msize;
    return 0;
    
}

baddr* bIdxFile_get_map_addr(bIdxFile* bFile)
{
    if(bFile && bFile->mem)
        return bFile->mem + BIDX_FHEAD_SIZE;
    return 0;
       
}

bIdxFileHead* bIdxFile_get_head(bIdxFile* bFile)
{
    if(bFile && bFile->head)
        return bFile->head;
    return 0;
       
}


bbool bIdxFile_close(bIdxFile* bFile)
{
    if(!bFile)
    {
        printf("bFile is NULL\n");
        return FALSE;
    }

    if(-1 == munmap(bFile->mem, bFile->fsize))
    {
         printf("文件close映射不成功!!");
         exit(-1);
    }
    
    close(bFile->fd);
    free(bFile);
    return TRUE;
}
