#include "bIdxArray.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
bIdxArray* bIdxArray_new()
{
    bIdxArray* lp_array = (bIdxArray*)malloc(sizeof(bIdxArray));
    memset(lp_array, 0, sizeof(bIdxArray));
    return lp_array;
}

bbool bIdxArray_delete(bIdxArray* pArray)
{
    if(!pArray)
    {
        return FALSE;
    }
    free(pArray);
    return TRUE;
}


bbool bIdxArray_load_block(bIdxArray* pArray, bIdxBlock* pBlock, bindex_t idx)
{
    if(!pArray || !pBlock)
    {
        return FALSE;
    }
    if(idx >= BIDXARRAY_BLOCK_CNT)
    {
        printf("idx is too big [%s][%d]\n", __FILE__, __LINE__);
        return FALSE;
    }
    if(pArray->array[idx] != NULL)
    {
        printf("pArray->array[idx] != NULL [%s][%d]\n", __FILE__, __LINE__);
        return FALSE;
    }
    pArray->array[idx] = pBlock;
    return TRUE;
}

bIdxBlock* bIdxArray_unload_block(bIdxArray* pArray, bindex_t idx)
{
    if(!pArray)
    {
        return NULL;
    }
    if(idx >= BIDXARRAY_BLOCK_CNT)
    {
        printf("idx is too big [%s][%d]\n", __FILE__, __LINE__);
        return NULL;
    }
    if(pArray->array[idx] == NULL)
    {
        printf("pArray->array[idx] != NULL [%s][%d]\n", __FILE__, __LINE__);
        return NULL;
    }
    
    bIdxBlock* pBlock = pArray->array[idx];
    pArray->array[idx] = NULL;
    
    return pBlock;
}





bbool bIdxArray_block_op(bIdxArray* pDest, bIdxArray* pSrc, bindex_t idx, int op)
{
    
    if(pDest->array[idx] && pSrc->array[idx])
    {
        bIdxBit_op((sbit*)(pDest->array[idx] + 1), (sbit*)(pSrc->array[idx] + 1), BIDXBLOCK_ID_SIZE, op);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
} 






bIdxArray* bIdxArray_dup(bIdxArray* pArray)
{
    if(!pArray)
    {
        return NULL;
    }

    bIdxArray* p_new_array = bIdxArray_new();
    bindex_t idx = 0;
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(pArray->array[idx] != NULL)
        {
            p_new_array->array[idx] = (bIdxBlock*)malloc(BIDXBLOCK_SIZE);
            memcpy(p_new_array->array[idx], pArray->array[idx], BIDXBLOCK_SIZE);
        }
        
        idx ++;
        
    }
    
    return p_new_array;
}


bIdxBlock* bIdxBlock_init(char* org, char* tag, unsigned short startIdx, bbyte bitVal, bbyte* memblock)
{
    bIdxBlock* p_idx_block = (bIdxBlock*)memblock;
    
    p_idx_block->flag = BIDXBLOCK_VALID;
    p_idx_block->startIdx = startIdx;
    
    if(org)
    {
        memcpy(p_idx_block->org, org, strlen(org) + 1);
    }
    else
    {
        p_idx_block->org[0] = '\0';
    }
    
    if(tag)
    {
        memcpy(p_idx_block->tag, tag, strlen(tag) + 1);
    }
    else
    {
        p_idx_block->tag[0] = '\0';
    }
    
    if(bitVal)
        memset(p_idx_block + 1, 0xff, BIDXBLOCK_ID_SIZE);
    else
        memset(p_idx_block + 1, 0x0, BIDXBLOCK_ID_SIZE);
    
    return p_idx_block;
}

bIdxBasRes* bIdxArray_get_idResult(bIdxArray* arr, size_t off, size_t cnt)
{
    if(!arr)
    {
        return NULL;
    }
    
    if(BIDXARRAY_ID_CNT <= off)
    {
        printf("BIDXARRAY_ID_CNT <= off [%s][%d]\n", __FILE__, __LINE__);
         return NULL;
    }

    
    bIdxBasRes* p_res = (bIdxBasRes*)malloc(sizeof(bIdxBasRes));
    p_res->ids = (bid_t*)malloc(sizeof(bid_t) * cnt);
    p_res->cnt = 0;
    
    bindex_t n_block_idx = off / BIDXBLOCK_ID_CNT;
    bindex_t n_bit_idx = off % BIDXBLOCK_ID_CNT;
    bindex_t i = n_bit_idx; // start pos

    while(n_block_idx < BIDXARRAY_BLOCK_CNT)
    {
        bIdxBlock* p_block = arr->array[n_block_idx];
        if(p_block)
        {

            baddr* p_addr = (baddr*)(p_block + 1);
            while(i != BIDXBLOCK_ID_CNT)
            {
                bbyte val = 0;
		bIdxBit_get_val(p_addr, i, val);
val = 0;
                if(val)
                {
                    p_res->ids[p_res->cnt++] = i + n_block_idx * BIDXBLOCK_ID_CNT;
                    if(p_res->cnt == cnt)
                    {
                        goto get_all_res;
                    }
                }
                
                i++;
            }
            
            i = 0; //set 0
        }
        n_block_idx ++;
    }
    
get_all_res :
    return p_res;
}




bIdxArray* bIdxIdRes_get_idxArray(bIdxBasRes* res, size_t off, size_t cnt)
{
    bIdxArray* lp_array = bIdxArray_new();
    bindex_t idx = off;
    bindex_t n_end = (cnt + off) < BIDXIDRES_CNT(res)? cnt + off : BIDXIDRES_CNT(res);
    
    while(idx < n_end)
    {
        bid_t id = BIDXIDRES_GETID(res, idx);
        
        if(!lp_array->array[id/BIDXBLOCK_ID_CNT])
        {
            bIdxBlock* p_new_block =  (bIdxBlock*)malloc(BIDXBLOCK_SIZE);
            bIdxBlock_init("", "", id/BIDXBLOCK_ID_CNT, 0, (bbyte*)p_new_block);
            bIdxArray_load_block(lp_array, p_new_block, id/BIDXBLOCK_ID_CNT);
        }

        bIdxArray_set_bitVal(lp_array, id, 1);
        
        idx++;
    }
    
    return lp_array;
        
}

//2个结构做and操作，生成一个idres，id按照老的idres里的id排序
bIdxBasRes* bIdx_resAndArray(bIdxBasRes* res, bIdxArray* arr, size_t off, size_t cnt)
{
    bIdxBasRes* lp_ret_res = NULL;
    BIDXIDRES_INIT(lp_ret_res, cnt);
    BIDXIDRES_CNT(lp_ret_res)=0;
    
    bindex_t idx = 0;
    size_t n_hit_cnt = 0;
    while(idx < BIDXIDRES_CNT(res))
    {
        bid_t id = BIDXIDRES_GETID(res, idx);
        bbyte val;
        bIdxArray_get_bitVal(arr, id, val);
        
        if(val)
        {
            if(n_hit_cnt >= off)
            {
                BIDXIDRES_SETID(lp_ret_res, BIDXIDRES_CNT(lp_ret_res), id);
                BIDXIDRES_CNT(lp_ret_res)++;
                
                if(BIDXIDRES_CNT(lp_ret_res) == cnt)break;
            }
            n_hit_cnt++;
        }
        
        idx++;
    }
    return lp_ret_res;
}
