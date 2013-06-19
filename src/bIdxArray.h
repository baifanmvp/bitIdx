#ifndef BIDX_ARRAY_H
#define BIDX_ARRAY_H
#include "bIdxBit.h"

#define BIDXBLOCK_VALID   (0x0001)
#define BIDXBLOCK_INVALID (0x0000)

#pragma pack(push)
#pragma pack(128)
//128 byte
typedef struct st_bIdxBlock
{
    flag_t flag;
    unsigned short startIdx;
    char org[32];
    char tag[32];
    char free[1];
}bIdxBlock;
#pragma pack(pop)


#define BIDXBLOCK_SIZE (1024*128)

#define BIDXBLOCK_ID_SIZE (BIDXBLOCK_SIZE - sizeof(bIdxBlock)) // sizeof(bIdxBlock) == 128 byte

#define BIDXBLOCK_ID_CNT  (BIDXBLOCK_ID_SIZE*8)

#define BIDXARRAY_BLOCK_CNT  (128/8)
#define BIDXARRAY_ID_CNT  (BIDXARRAY_BLOCK_CNT * BIDXBLOCK_ID_CNT)

#define BIDXARRAY_POS_BLOCK(arr, pos) ((arr)->array[(pos)/BIDXBLOCK_ID_CNT]) 

#define bIdxArray_set_bitVal(arr, pos, val)                             \
    bIdxBit_set_val( ((arr)->array[(pos) / BIDXBLOCK_ID_CNT]) + 1, (pos) % BIDXBLOCK_ID_CNT, val)


#define bIdxArray_get_bitVal(arr, pos, val)                             \
    bIdxBit_get_val( ((arr)->array[(pos) / BIDXBLOCK_ID_CNT]) + 1, (pos) % BIDXBLOCK_ID_CNT, val)


#define BIDXARRAY_POS_SET_VAL(arr, pos, val)                        \
    do                                                              \
    {                                                               \
        bIdxBlock* pBlock = BIDXARRAY_POS_BLOCK(arr, pos);          \
        bIdxBit_set_val(pBlock + 1, (pos)%BIDXBLOCK_ID_CNT, (val)); \
    }while(FALSE);

#define BIDXARRAY_POS_GET_VAL(arr, pos, val)                        \
    do                                                              \
    {                                                               \
        bIdxBlock* pBlock = BIDXARRAY_POS_BLOCK(arr, pos);          \
        bIdxBit_get_val(pBlock + 1, (pos)%BIDXBLOCK_ID_CNT, (val)); \
    }while(FALSE);

 


typedef struct st_bIdxArray
{
    char* allkey; //ref allkey of hashtable 
    bIdxBlock* array[BIDXARRAY_BLOCK_CNT];
}bIdxArray;


struct st_bIdxIdRes
{
    bid_t* id;
    size_t cnt;
}bIdxBasRes;

bIdxBasRes* bIdxArray_get_idResult(bIdxArray* arr, size_t off, size_t cnt)
{
    if(!array)
    {
        return NULL;
    }
    
    if(BIDXARRAY_ID_CNT <= off)
    {
        printf("BIDXARRAY_ID_CNT <= off [%s][%d]\n", __FILE__, __LINE__);
         return NULL;
    }

    
    bIdxBasRes* p_res = (bIdxBasRes*)malloc(sizeof(bIdxBasRes));
    p_res->id = (did_t*)malloc(sizeof(did_t) * cnt);
    p_res->cnt = 0;
    
    index_t n_block_idx = off / BIDXBLOCK_ID_CNT;
    index_t n_bit_idx = off % BIDXBLOCK_ID_CNT;
    index_t i = n_bit_idx; // start pos
    
    while(n_block_idx < BIDXARRAY_BLOCK_CNT)
    {
        bIdxBlock* p_block = arr->array[n_block_idx];
        if(p_block)
        {
            addr* p_addr = (addr*)(p_block + 1);
            while(i != BIDXBLOCK_ID_CNT)
            {
                byte val;
                bIdxBit_get_val(p_addr, i, val);
                if(val)
                {
                    p_res->id[p_res->cnt++] = i + n_block_idx * BIDXBLOCK_ID_CNT;
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



bIdxArray* bIdxArray_new()
{
    bIdxArray* lp_array = (bIdxArray*)malloc(sizeof(bIdxArray));
    memset(lp_array, 0, sizeof(bIdxArray));
    return lp_array;
}

bool bIdxArray_delete(bIdxArray* pArray)
{
    if(!pArray)
    {
        return FALSE;
    }
    free(pArray);
    return TRUE;
}


bool bIdxArray_load_block(bIdxArray* pArray, bIdxBlock* pBlock, index_t idx)
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

bIdxBlock* bIdxArray_unload_block(bIdxArray* pArray, index_t idx)
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





bool bIdxArray_block_op(bIdxArray* pDest, bIdxArray* pSrc, index_t idx, int op)
{
    
    if(pDest->array[n_loop_cnt] && pSrc->array[n_loop_cnt])
    {
        bIdxBit_op(pDest->array[n_loop_cnt] + 1, pSrc->array[n_loop_cnt] + 1, BIDXBLOCK_ID_SIZE, op);
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
    index_t idx = 0;
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


bIdxBlock* bIdxBlock_init(char* org, char* tag, unsigned short startIdx, byte bitVal, byte* memblock)
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



#endif
