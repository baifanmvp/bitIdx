#ifndef BIDX_ARRAY_H
#define BIDX_ARRAY_H
#include "bIdxBit.h"

#define BIDXBLOCK_VALID   (0x0001)
#define BIDXBLOCK_INVALID (0x0000)

#pragma pack(push)
#pragma pack(4)
//128 byte
typedef struct st_bIdxBlock
{
    bflag_t flag;
    unsigned short startIdx;
    char org[32];
    char tag[32];
    char free[60];
}bIdxBlock;
#pragma pack(pop)


#define BIDXBLOCK_SIZE (1024*128)

#define BIDXBLOCK_ID_SIZE (BIDXBLOCK_SIZE - sizeof(bIdxBlock)) // sizeof(bIdxBlock) == 128 byte

#define BIDXBLOCK_ID_CNT  (BIDXBLOCK_ID_SIZE*8)

#define BIDXARRAY_BLOCK_CNT  (128/8)
#define BIDXARRAY_ID_CNT  (BIDXARRAY_BLOCK_CNT * BIDXBLOCK_ID_CNT)




typedef struct st_bIdxArray
{
    char* allKey; //ref allkey of hashtable 
    bIdxBlock* array[BIDXARRAY_BLOCK_CNT];
}bIdxArray;


typedef struct st_bIdxIdRes
{
    bid_t* ids;
    size_t cnt;
}bIdxBasRes;


#define BIDXARRAY_POS_BLOCK(arr, pos) ((arr)->array[(pos)/BIDXBLOCK_ID_CNT]) 

#define bIdxArray_set_bitVal(arr, pos, val)                             \
    do                                                                  \
    {                                                                   \
        if((arr)->array[(pos) / BIDXBLOCK_ID_CNT])                      \
            bIdxBit_set_val( ((sbit*)((arr)->array[(pos) / BIDXBLOCK_ID_CNT]) + 1), \
                             (pos) % BIDXBLOCK_ID_CNT, val);            \
    }while(FALSE);



#define bIdxArray_get_bitVal(arr, pos, val)                             \
    do                                                                  \
    {                                                                   \
        if((arr)->array[(pos) / BIDXBLOCK_ID_CNT])                      \
            bIdxBit_get_val( ((sbit*)((arr)->array[(pos) / BIDXBLOCK_ID_CNT]) + 1), \
                             (pos) % BIDXBLOCK_ID_CNT, val);            \
    }while(FALSE);


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

 


bIdxArray* bIdxArray_new();

bbool bIdxArray_delete(bIdxArray* pArray);

bbool bIdxArray_load_block(bIdxArray* pArray, bIdxBlock* pBlock, bindex_t idx);

bIdxBlock* bIdxArray_unload_block(bIdxArray* pArray, bindex_t idx);

bbool bIdxArray_block_op(bIdxArray* pDest, bIdxArray* pSrc, bindex_t idx, int op);

bIdxArray* bIdxArray_dup(bIdxArray* pArray);

bIdxBlock* bIdxBlock_init(char* org, char* tag, unsigned short startIdx, bbyte bitVal, bbyte* memblock);

bIdxBasRes* bIdxArray_get_idResult(bIdxArray* arr, size_t off, size_t cnt);

#endif
