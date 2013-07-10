#include "bIdxHash.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
bIdxHash* bIdxHash_new()
{
    bIdxHash* p_idx_hash = (bIdxHash*)malloc(sizeof(bIdxHash));
    p_idx_hash->allHash = g_hash_table_new(g_str_hash, g_str_equal);
    p_idx_hash->preHash = g_hash_table_new(g_str_hash, g_str_equal);
    return p_idx_hash;
}
bbool bIdxHash_delete(bIdxHash* pIdxHash)
{
    if(!pIdxHash)
    {
        return FALSE;
    }
    
    GList* list_key = g_hash_table_get_keys(pIdxHash->preHash);
    GList* list_value = g_hash_table_get_values(pIdxHash->preHash);
    if (list_key != NULL)
    {
        GList* list_key_iter = g_list_first(list_key);
        do {
            free(list_key_iter->data);
        } while ((list_key_iter = g_list_next(list_key_iter)) != NULL);
        g_list_free(list_key);
    }

    if (list_value != NULL)
    {
        GList* list_value_iter = g_list_first(list_value);
        do {
            bPreVal* p_pre_val = (bPreVal*)list_value_iter->data;
            while(p_pre_val)
            {
                bPreVal* p_pre_val_tmp = p_pre_val;                
                p_pre_val = p_pre_val->next;
                free(p_pre_val_tmp->suffix);
                free(p_pre_val_tmp);
            }
            
        } while ((list_value_iter = g_list_next(list_value_iter)) != NULL);
        g_list_free(list_value);
    }

/////////////////////////////////////////////////////////////////////////////////////
    list_key = g_hash_table_get_keys(pIdxHash->allHash);
    list_value = g_hash_table_get_values(pIdxHash->allHash);
    if (list_key != NULL)
    {
        GList* list_key_iter = g_list_first(list_key);
        do {
            free(list_key_iter->data);
        } while ((list_key_iter = g_list_next(list_key_iter)) != NULL);
        g_list_free(list_key);
    }

    if (list_value != NULL)
    {
        GList* list_value_iter = g_list_first(list_value);
        do {
            bAllVal* p_all_val = (bAllVal*)list_value_iter->data;
            bIdxArray_delete(p_all_val->data);//free bIdxArray
            free(p_all_val);
        } while ((list_value_iter = g_list_next(list_value_iter)) != NULL);
        g_list_free(list_value);
    }

////////////////////////////////////////////////////////////////////////////////
    g_hash_table_destroy(pIdxHash->allHash);
    g_hash_table_destroy(pIdxHash->preHash);
    
    free(pIdxHash);
    return TRUE;
}

bbool bIdxHash_blocks_set_null(bIdxHash* pIdxHash)
{
    if(!pIdxHash)
    {
        return FALSE;
    }
    
    GList* list_value ;

/////////////////////////////////////////////////////////////////////////////////////
    list_value = g_hash_table_get_values(pIdxHash->allHash);

    if (list_value != NULL)
    {
        GList* list_value_iter = g_list_first(list_value);
        do {
            bAllVal* p_all_val = (bAllVal*)list_value_iter->data;
	    memset(p_all_val->data->array, 0, sizeof(bIdxArray*) * BIDXARRAY_BLOCK_CNT);
        } while ((list_value_iter = g_list_next(list_value_iter)) != NULL);
        g_list_free(list_value);
    }

////////////////////////////////////////////////////////////////////////////////
    return TRUE;
}

bbool bIdxHash_insert(bIdxHash* pIdxHash, char* prefix, char* suffix, bIdxArray* data)
{
    
    bPreVal* p_pre_val = bIdxHash_lookup_pre(pIdxHash, prefix);
    bPreVal* p_new_val = (bPreVal*)malloc(sizeof(bPreVal));
    p_new_val->suffix = strdup(suffix);
    p_new_val->data = data;
    p_new_val->next = p_pre_val;
         
    if(p_pre_val)
    {
        p_new_val->prefix = p_pre_val->prefix;
        g_hash_table_insert(pIdxHash->preHash, prefix, p_new_val);            
    }
    else
    {
        p_new_val->prefix = strdup(prefix);
        g_hash_table_insert(pIdxHash->preHash, p_new_val->prefix, p_new_val);    
    }
        
    bAllVal* p_all_val = (bAllVal*)malloc(sizeof(bAllVal));
    char* lp_all_key = NULL;
    dup_all_string(prefix, suffix, lp_all_key);

    data->allKey = lp_all_key;
    p_all_val->data = data;
    g_hash_table_insert(pIdxHash->allHash, lp_all_key, p_all_val);
    
    return TRUE;
    
}


bAllVal* bIdxHash_lookup_all(bIdxHash* pIdxHash, char* all)
{
    if(!pIdxHash || !all)
    {
        return NULL;
    }
    
    return g_hash_table_lookup(pIdxHash->allHash, all);
}

bPreVal* bIdxHash_lookup_pre(bIdxHash* pIdxHash, char* prefix)
{
    if(!pIdxHash || !prefix)
    {
        return NULL;
    }
    return g_hash_table_lookup(pIdxHash->preHash, prefix);
}




bIdxArray*  bIdxHash_remove(bIdxHash* pIdxHash, char* prefix, char* suffix )
{
    if(!pIdxHash || !prefix || !suffix)
    {
        return NULL;
    }
    
    char* lp_all_key = NULL;
    dup_all_string(prefix, suffix, lp_all_key);
    bIdxArray* lp_array = NULL;
    
    bAllVal* lp_all_val = bIdxHash_lookup_all(pIdxHash, lp_all_key);
    if(lp_all_val)
    {
       gboolean ret = g_hash_table_remove(pIdxHash->allHash, lp_all_key);
        lp_array = lp_all_val->data;
        free(lp_all_val);
    }
    else
    {
        return NULL;
    }
    
    bPreVal* lp_pre_val = bIdxHash_lookup_pre(pIdxHash, prefix);
    bPreVal* lp_last_preVal = NULL;
    while(lp_pre_val)
    {
        if(strcmp(lp_pre_val->suffix, suffix) == 0)break;
        lp_last_preVal = lp_pre_val;
        lp_pre_val = lp_pre_val->next;
    }
    if(!lp_pre_val)
    {
        printf("bIdxHash_lookup_pre !=  bIdxHash_lookup_all ! [%s][%d]\n", __FILE__, __LINE__);
        exit(-1);
    }

    if(lp_pre_val->data != lp_array)
    {
        printf("pre_array !=  all_array ! [%s][%d]\n", __FILE__, __LINE__);
        exit(-1);
    }

    if(lp_last_preVal)
    {
        lp_last_preVal->next = lp_pre_val->next;
    }
    else
    {
        if(lp_pre_val->next)
        {
            g_hash_table_insert(pIdxHash->preHash, prefix, lp_pre_val->next);
        }
        else
        {
           gboolean ret = g_hash_table_remove(pIdxHash->preHash, prefix);
            free(lp_pre_val->prefix);            
        }
        
        free(lp_pre_val->suffix);
        free(lp_pre_val);
    }
    free(lp_all_key);

    return lp_array;
}
