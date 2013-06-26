#include "bIdxBasicOp.h"

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


bbool bIdxBasOp_delete(bIdxBasOp* pbIdxBasOp)
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

bIdxBlock* bIdxBasOp_get_free_idxBlock(bIdxBasOp* pbIdxBasOp)
{
    if(!pbIdxBasOp)
    {
        return NULL;
    }
    if(!pbIdxBasOp->free)
    {
        bIdxBasOp_reload(pbIdxBasOp);

    }
    bIdxfreeNode* lp_free_node = pbIdxBasOp->free;
    pbIdxBasOp->free = pbIdxBasOp->free->next;
    bIdxBlock* p_new_block =  (bIdxBlock*)lp_free_node->node;
    free(lp_free_node);
    return p_new_block;
    
}


bbool bIdxBasOp_reload(bIdxBasOp* pbIdxBasOp)
{
    if(!pbIdxBasOp)
    {
        return FALSE;
    }
//clean mem
    bIdxHash_delete(pbIdxBasOp->haIdx);
    pbIdxBasOp->haIdx = bIdxHash_new();
    
    while(pbIdxBasOp->free)
    {
        bIdxfreeNode* lp_tmp_node = pbIdxBasOp->free;
        pbIdxBasOp->free = pbIdxBasOp->free->next;
        free(lp_tmp_node);
    }

    //file append and remmap
    bIdxFile* p_idx_file = pbIdxBasOp->fp;
    bIdxFile_append(p_idx_file, BIDX_EXT_SIZE);
        
    //reload bitBlock
    bIdxBasOp_load(pbIdxBasOp);
    return TRUE;
}



bbool bIdxBasOp_load(bIdxBasOp* pbIdxBasOp)
{
    if(!pbIdxBasOp)
    {
        return FALSE;
    }
    
    bIdxFile* p_idx_file = pbIdxBasOp->fp;
    baddr* p_start_addr = bIdxFile_get_map_addr(p_idx_file);
    size_t n_block_sz = bIdxFile_get_size(p_idx_file);
    
    size_t n_unit_cnt = n_block_sz / BIDX_SPACE_UNIT;
    size_t i = 0;
    while(i != n_unit_cnt)
    {
        bIdxBlock* p_block = (bIdxBlock*)(p_start_addr + i * BIDX_SPACE_UNIT);
        if (p_block->flag == BIDXBLOCK_INVALID)
        {
            bIdxfreeNode* p_node = (bIdxfreeNode*)malloc(sizeof(bIdxfreeNode));
            p_node->node = p_block;
            p_node->next = pbIdxBasOp->free;
            pbIdxBasOp->free = p_node;
        }
        else if (p_block->flag == BIDXBLOCK_VALID)
        {
            
            char* lp_all_key = NULL;
            dup_all_string(p_block->org, p_block->tag, lp_all_key);

            bAllVal* lp_all_val = bIdxHash_lookup_all(pbIdxBasOp->haIdx, lp_all_key);
            free(lp_all_key);
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

bbool bIdxBasOp_ext(bIdxBasOp* pbIdxBasOp)
{
    if(!pbIdxBasOp)
    {
        return FALSE;
    }
    
    bIdxFile* p_idx_file = pbIdxBasOp->fp;
    size_t n_src_msize = p_idx_file->msize;
    bIdxFile_append(p_idx_file, BIDX_EXT_SIZE);
        
    baddr* p_start_addr = p_idx_file->mem + n_src_msize;
    size_t n_unit_cnt = BIDX_EXT_UNIT_CNT;
    size_t i = 0;
    while(i != n_unit_cnt)
    {
        bIdxBlock* p_block = (bIdxBlock*)(p_start_addr + i * BIDX_SPACE_UNIT);
        p_block->flag = BIDXBLOCK_INVALID;
            
        bIdxfreeNode* p_node = (bIdxfreeNode*)malloc(sizeof(bIdxfreeNode));
        p_node->node = p_block;
        p_node->next = pbIdxBasOp->free;
        pbIdxBasOp->free = p_node;
        
        i++;
    }
    return TRUE;
}

//array to array
bIdxArray* bIdxBasOp_searchOr_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp || !pSrcArr)
        return NULL;

    if(!pDestArr)
        return bIdxArray_dup(pSrcArr);

    bindex_t idx = 0;
    
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
    if(!pbIdxBasOp || !pSrcArr)
        return NULL;

    if(!pDestArr)
        return bIdxArray_dup(pSrcArr);

    bindex_t idx = 0;
    
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
    if(!pbIdxBasOp || !pSrcArr || !pDestArr)
        return NULL;


    char* org = NULL;
    char* tag = NULL;
    bindex_t idx = 0;
    char* lp_all_key = strdup(pDestArr->allKey);
    
    split_all_string(org, tag, lp_all_key);
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        if(!pDestArr->array[idx] && pSrcArr->array[idx])
        {
            //load a block if OR op (file mmap)
            
            bIdxBlock* p_new_block =  bIdxBasOp_get_free_idxBlock(pbIdxBasOp);
            bIdxBlock_init(org, tag, idx, 0, (bbyte*)p_new_block);
            memcpy(p_new_block + 1, pSrcArr->array[idx] + 1, BIDXBLOCK_ID_SIZE);
            bIdxArray_load_block(pDestArr, p_new_block, idx);
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


    bindex_t idx = 0;
    
    while(idx < BIDXARRAY_BLOCK_CNT)
    {
        bIdxArray_block_op(pDestArr,  pSrcArr, idx,  BIT_OP_NOT_AND);
        
        idx ++;
        
    }

    return pDestArr;
}

/////////////////////////////////////////////



//array to idResult

bIdxArray* bIdxBasOp_searchOr_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr)
{
    if(!pbIdxBasOp || !pSrcArr)
        return NULL;

    if(!pDestArr)
        return bIdxArray_dup(pSrcArr);

    bindex_t idx = 0;
    
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

bIdxBasRes* bIdxBasOp_searchAnd_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxBasRes* pBasRes)
{
    if(!pbIdxBasOp || !pDestArr || !pBasRes)
        return NULL;

    bIdxBasRes* lp_and_res = (bIdxBasRes*)malloc(sizeof(bIdxBasRes));
    lp_and_res->ids = (bid_t*)malloc(sizeof(bid_t) * pBasRes->cnt);
    lp_and_res->cnt = 0;
    
    bindex_t idx = 0;
    
    while(idx < pBasRes->cnt)
    {
        bbyte val = 0;
        if(pBasRes->ids[idx] < BIDXARRAY_ID_CNT)
            bIdxArray_get_bitVal(pDestArr, pBasRes->ids[idx], val);

        if(val)
            lp_and_res->ids[lp_and_res->cnt++] = pBasRes->ids[idx];
        idx++;
    }

    return lp_and_res;
}





bbool bIdxBasOp_modify_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxBasRes* pBasRes)
{
    if(!pbIdxBasOp || !pDestArr || !pBasRes)
        return FALSE;


    char* org = NULL;
    char* tag = NULL;
    bindex_t idx = 0;
    char* lp_all_key = strdup(pDestArr->allKey);
    
    split_all_string(org, tag, lp_all_key);

    
    while(idx < pBasRes->cnt)
    {
        if(pBasRes->ids[idx] < BIDXARRAY_ID_CNT)
        {
            if(!pDestArr->array[pBasRes->ids[idx] / BIDXBLOCK_ID_CNT])
            {
                bIdxBlock* p_new_block =  bIdxBasOp_get_free_idxBlock(pbIdxBasOp);
                bIdxBlock_init(org, tag, idx, 0, (bbyte*)p_new_block);
   
                bIdxArray_load_block(pDestArr, p_new_block, pBasRes->ids[idx] / BIDXBLOCK_ID_CNT);
                
            }

            bIdxArray_set_bitVal(pDestArr, pBasRes->ids[idx], 1);
        }
        idx++;
    }
    free(lp_all_key);
    return TRUE;
}
    

//if pSrcArr is NULL ,then remove all block of pDestArr

bbool bIdxBasOp_remove_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxBasRes* pBasRes)
{
    if(!pbIdxBasOp ||  !pDestArr || !pBasRes)
        return FALSE;


    bindex_t idx = 0;
    
    while(idx < pBasRes->cnt)
    {
        if(pBasRes->ids[idx] < BIDXARRAY_ID_CNT)
        {
            bIdxArray_set_bitVal(pDestArr, pBasRes->ids[idx], 0);
        }
        idx++;
    }
    return TRUE;    
}

bIdxArray** bIdxBasOp_lookupOrg_array(bIdxBasOp* pbIdxBasOp, char* prefix )
{
    bIdxArray** lplp_array = (bIdxArray**)malloc(sizeof(bIdxArray*) * BIDX_ORG_LIMIT);
    bPreVal* lp_pre_val = bIdxHash_lookup_pre(pbIdxBasOp->haIdx, prefix);
    bindex_t idx = 0;
    while(lp_pre_val)
    {
        lplp_array[idx++] = lp_pre_val->data;
        lp_pre_val = lp_pre_val->next;
    }
    lplp_array[idx] = NULL;
    return lplp_array;
    
}
bIdxArray* bIdxBasOp_lookup_array(bIdxBasOp* pbIdxBasOp, char* prefix, char* suffix)
{
    char* lp_all_key = NULL;
    dup_all_string(prefix, suffix, lp_all_key);

    bAllVal* lp_all_val = bIdxHash_lookup_all(pbIdxBasOp->haIdx, lp_all_key);
    free(lp_all_key);
    
    if(lp_all_val)
        return lp_all_val->data;
    else
        return NULL;
}


bbool bIdxBasOp_add_array(bIdxBasOp* pbIdxBasOp, char* prefix, char* suffix)
{

    char* lp_all_key = NULL;
    dup_all_string(prefix, suffix, lp_all_key);

    bAllVal* lp_all_val = bIdxHash_lookup_all(pbIdxBasOp->haIdx, lp_all_key);
    free(lp_all_key);
    
    if(!lp_all_val)
    {
        bIdxArray* lp_new_array = bIdxArray_new();
        bIdxHash_insert(pbIdxBasOp->haIdx, prefix, suffix, lp_new_array);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bbool bIdxBasOp_rm_array(bIdxBasOp* pbIdxBasOp, char* prefix, char* suffix)
{

    char* lp_all_key = NULL;
    dup_all_string(prefix, suffix, lp_all_key);

    bAllVal* lp_all_val = bIdxHash_lookup_all(pbIdxBasOp->haIdx, lp_all_key);
    free(lp_all_key);
    
    if(lp_all_val)
    {
        bIdxArray* lp_del_array = (bIdxArray*)(lp_all_val->data);
        bindex_t idx = 0;
        //loop del block 
        while(idx < BIDXARRAY_BLOCK_CNT)
        {
            bIdxBlock* lp_del_block = lp_del_array->array[idx];
            if(lp_del_block)
            {
                lp_del_block->flag = BIDXBLOCK_INVALID;
                bIdxfreeNode* p_node = (bIdxfreeNode*)malloc(sizeof(bIdxfreeNode));
                p_node->node = lp_del_block;
                p_node->next = pbIdxBasOp->free;
                pbIdxBasOp->free = p_node;
            }
            idx++;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

