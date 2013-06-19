#ifndef BIDX_BASICOP_H
#define BIDX_BASICOP_H
#include "bIdxFile.h"
#include "bIdxArray.h"


typedef struct st_bIdxfreeNode
{
    bIdxBlock* node;
    struct st_bIdxfreeNode* next;
}bIdxfreeNode;

#define BIDX_SPACE_UNIT   (BIDXBLOCK_SIZE)

#define BIDX_EXT_SIZE     (1073741824)                 // 1G

#define BIDX_EXT_UNIT_CNT (BIDX_EXT_SIZE/BIDX_SPACE_UNIT)  // 8096

struct st_bIdxBasOp
{
    bIdxFile* fp;
    bIdxHash* haIdx;
    bIdxfreeNode* free;
    
}bIdxBasOp;


bIdxBasOp* bIdxBasOp_new(char* path)
{
    bIdxFile* p_idx_file = bIdxFile_open(path);
    
    if(!p_idx_file)
    {
        printf("bIdxFile_open false ! [%s][%d]\n", __FILE__, __LINE__);
        return NULL;
    }
    bIdxBasOp* p_idx_basop = (bIdxBasOp*)malloc(sizeof(bIdxBasOp));
    p_idx_basop->fp = p_idx_file;
    p_idx_basop->free = NULL;
    p_idx_basop->haIdx = bIdxHash_new();
    
    if(!p_idx_file->msize)
    {
        bIdxBasOp_ext(p_idx_basop);
    }
    else
    {
        bIdxBasOp_load(p_idx_basop);
    }
    return p_idx_basop;
}


bool bIdxBasOp_delete(bIdxBasOp* pbIdxBasOp)
{
    if(!pbIdxBasOp)
    {
        return FALSE;
    }
    bIdxFile_close(pbIdxBasOp->fp);
    bIdxHash_delete(pbIdxBasOp->haIdx);
    while(pbIdxBasOp->free)
    {
        bIdxfreeNode* lp_tmp_node = pbIdxBasOp->free;
        pbIdxBasOp->free = pbIdxBasOp->free->next;
        free(lp_tmp_node);
    }

    return TRUE;

}






bool bIdxBasOp_load(bIdxBasOp* pbIdxBasOp)
{
    if(!pbIdxBasOp)
    {
        return FALSE;
    }
    
    bIdxFile* p_idx_file = pbIdxBasOp->fp;
    addr* p_start_addr = bIdxFile_get_map_addr(p_idx_file);
    size_t n_block_sz = bIdxFile_get_size(p_idx_file);
    
    size_t n_unit_cnt = n_block_sz / BIDX_SPACE_UNIT;
    size_t i = 0;
    while(i != n_unit_cnt)
    {
        bIdxBlock* p_block = p_start_addr + i * BIDX_SPACE_UNIT;
        if (p_block->flag == BIDXBLOCK_INVALID)
        {
            bIdxfreeNode* p_node = (bIdxfreeNode*)malloc(sizeof(bIdxfreeNode));
            p_node->node = p_block;
            p_node->next = pIdxSpace->free;
            pbIdxBasOp->free = p_node;
        }
        else if (p_block->flag == BIDXBLOCK_VALID)
        {
            
            char* lp_all_key = NULL;
            dup_all_string(p_block->org, p_block->tag, lp_all_key);

            bAllVal* lp_all_val = bIdxHash_lookup_all(pbIdxBasOp->haIdx, lp_all_key);
            free(lp_all_key)
            if(lp_all_val)
            {
                bIdxArray* lp_idx_array = (bIdxArray*)(lp_all_val->data);
                if(lp_idx_array->array[p_block->startIdx])
                {
                    printf("lp_idx_array->array[p_block->startIdx]  is not null ! [%s][%d]\n", __FILE__, __LINE__);
                    exit(-1);
                }
                
                lp_idx_array->array[p_block->startIdx] = p_block;
            }
            else
            {
                //create new array, because not find  org.tag
                bIdxArray* lp_idx_array = (bIdxArray*)malloc(sizeof(bIdxArray));
                memset(lp_idx_array, 0, sizeof(bIdxArray));
                lp_idx_array->array[p_block->startIdx] = p_block;
                bIdxHash_insert(pbIdxBasOp->haIdx, p_block->org, p_block->tag, lp_idx_array);
            }
        }
        i++;
    }    
}

bool bIdxBasOp_ext(bIdxBasOp* pbIdxBasOp)
{
    if(!pbIdxBasOp)
    {
        return FALSE;
    }
    
    bIdxFile* p_idx_file = pbIdxBasOp->fp;
    size_t n_src_msize = p_idx_file->msize;
    bIdxFile_append(p_idx_file, BIDX_EXT_SIZE);
        
    addr* p_start_addr = p_idx_file->mem + n_src_msize;
    size_t n_unit_cnt = BIDX_EXT_UNIT_CNT;
    size_t i = 0;
    while(i != n_unit_cnt)
    {
        bIdxBlock* p_block = p_start_addr + i * BIDX_SPACE_UNIT;
        p_block->flag = BIDXBLOCK_INVALID;
            
        bIdxfreeNode* p_node = (bIdxfreeNode*)malloc(sizeof(bIdxfreeNode));
        p_node->node = p_block;
        p_node->next = pIdxSpace->free;
        pbIdxBasOp->free = p_node;
        
        i++;
    }
    return TRUE;
}

//array to array
bIdxArray* bIdxBasOp_searchOr_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp || !srcArr)
        return NULL;

    if(!pDestArr)
        return bIdxArray_dup(pSrcArr);

    index_t idx = 0;
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(!pDestArr->array[idx] && pSrcArr->array[idx])
        {
            //load a block if OR op (malloc)
            bIdxBlock* p_new_block =  (bIdxBlock*)malloc(BIDXBLOCK_SIZE);
            memcpy(p_new_block, pSrcArr->array[idx], BIDXBLOCK_SIZE);
            bIdxArray_load_block(pDestArr, p_new_block, idx);
        }
        else
        {
            bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_OR);
        }
        
        idx ++;
        
    }

    return pDestArr;
}



bIdxArray* bIdxBasOp_searchAnd_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp || !srcArr)
        return NULL;

    if(!pDestArr)
        return bIdxArray_dup(pSrcArr);

    index_t idx = 0;
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(pDestArr->array[idx] && !pSrcArr->array[idx])
        {
            //unload a block if AND op (malloc)
            bIdxBlock* p_unload_block = bIdxArray_unload_block(pDestArr, idx);
            free(p_unload_block);
        }
        else
        {
            bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_AND);
        }
        
        
        idx ++;
        
    }

    return pDestArr;
}





bIdxArray* bIdxBasOp_modify_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp || !srcArr || !pDestArr)
        return NULL;


    char* org = NULL;
    char* tag = NULL;
    index_t idx = 0;
    char* lp_all_key = strdup(pDestArr->allKey);
    
    split_all_string(org, tag, lp_all_key);
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(!pDestArr->array[idx] && pSrcArr->array[idx])
        {
            //load a block if OR op (file mmap)
            bIdxfreeNode* lp_free_node = pbIdxBasOp->free;
            pbIdxBasOp->free = pbIdxBasOp->free->next;

            
            bIdxBlock* p_new_block =  (bIdxBlock*)lp_free_node->node;
            bIdxBlock_init(org, tag, idx, 0, (byte*)p_new_block);
            memcpy(p_new_block + 1, pSrcArr->array[idx] + 1, BIDXBLOCK_ID_SIZE);
            bIdxArray_load_block(pDestArr, p_new_block, idx);

            free(lp_free_node);
        }
        else
        {
            bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_OR);
        }
        
        idx ++;
        
    }

    free(lp_all_key);
    return pDestArr;
}

//if pSrcArr is NULL ,then remove all block of pDestArr
bIdxArray* bIdxBasOp_remove_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp ||  !pDestArr)
        return NULL;


    index_t idx = 0;
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_NOT_AND);
        
        idx ++;
        
    }

    free(lp_all_key);
    return pDestArr;
}

/////////////////////////////////////////////



//array to idResult

bIdxArray* bIdxBasOp_searchOr_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp || !srcArr)
        return NULL;

    if(!pDestArr)
        return bIdxArray_dup(pSrcArr);

    index_t idx = 0;
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(!pDestArr->array[idx] && pSrcArr->array[idx])
        {
            //load a block if OR op (malloc)
            bIdxBlock* p_new_block =  (bIdxBlock*)malloc(BIDXBLOCK_SIZE);
            memcpy(p_new_block, pSrcArr->array[idx], BIDXBLOCK_SIZE);
            bIdxArray_load_block(pDestArr, p_new_block, idx);
        }
        else
        {
            bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_OR);
        }
        
        idx ++;
        
    }

    return pDestArr;
}



bIdxArray* bIdxBasOp_searchAnd_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxBasRes* pBasRes)
{
    if(!pbIdxBasOp || !srcArr)
        return NULL;

    if(!pDestArr)
        return bIdxArray_dup(pSrcArr);

    index_t idx = 0;
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(pDestArr->array[idx] && !pSrcArr->array[idx])
        {
            //unload a block if AND op (malloc)
            bIdxBlock* p_unload_block = bIdxArray_unload_block(pDestArr, idx);
            free(p_unload_block);
        }
        else
        {
            bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_AND);
        }
        
        
        idx ++;
        
    }

    return pDestArr;
}





bIdxArray* bIdxBasOp_modify_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp || !srcArr || !pDestArr)
        return NULL;


    char* org = NULL;
    char* tag = NULL;
    index_t idx = 0;
    char* lp_all_key = strdup(pDestArr->allKey);
    
    split_all_string(org, tag, lp_all_key);
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(!pDestArr->array[idx] && pSrcArr->array[idx])
        {
            //load a block if OR op (file mmap)
            bIdxfreeNode* lp_free_node = pbIdxBasOp->free;
            pbIdxBasOp->free = pbIdxBasOp->free->next;

            
            bIdxBlock* p_new_block =  (bIdxBlock*)lp_free_node->node;
            bIdxBlock_init(org, tag, idx, 0, (byte*)p_new_block);
            memcpy(p_new_block + 1, pSrcArr->array[idx] + 1, BIDXBLOCK_ID_SIZE);
            bIdxArray_load_block(pDestArr, p_new_block, idx);

            free(lp_free_node);
        }
        else
        {
            bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_OR);
        }
        
        idx ++;
        
    }

    free(lp_all_key);
    return pDestArr;
}

//if pSrcArr is NULL ,then remove all block of pDestArr
bIdxArray* bIdxBasOp_remove_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp ||  !pDestArr)
        return NULL;


    index_t idx = 0;
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_NOT_AND);
        
        idx ++;
        
    }

    free(lp_all_key);
    return pDestArr;
}




#endif

