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

bbool bIdxHash_insert(bIdxHash* pIdxHash, char* prefix, char* suffix, bIdxArray* data)
{
    
    bPreVal* p_pre_val = bIdxHash_lookup_pre(pIdxHash, prefix);
    bPreVal* p_new_val = (bPreVal*)malloc(sizeof(bPreVal));
    p_new_val->suffix = strdup(suffix);
    p_new_val->data = data;
    p_new_val->next = p_pre_val;
         
    if(p_pre_val)
    {
        g_hash_table_insert(pIdxHash->preHash, prefix, p_new_val);            
    }
    else
    {
        g_hash_table_insert(pIdxHash->preHash, strdup(prefix), p_new_val);    
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



