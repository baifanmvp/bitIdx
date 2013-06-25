#include "cjson.h"
#include <stdio.h>
#include <string.h>

cjson* cjson_new(const char* strJson)
{
    cjson* p_json = (cjson*)malloc(sizeof(cjson));
    p_json->ty = JSON_OBJECT_MASTER;
    p_json->root = cJSON_Parse(strJson);
    p_json->child = p_json->root;
    p_json->parse = cjson_parse;
    p_json->obj = cjson_obj;
    p_json->obj_i = cjson_obj_idx;
    p_json->obj_s = cjson_obj_str;

    p_json->sobj = cjson_set_obj;
    p_json->sstr = cjson_set_str;
    p_json->snum = cjson_set_num;
    p_json->sbool = cjson_set_bool;
    p_json->snull = cjson_set_null;

    p_json->sobj_i = cjson_set_obj_idx;
    p_json->sstr_i = cjson_set_str_idx;
    p_json->snum_i = cjson_set_num_idx;
    p_json->sbool_i = cjson_set_bool_idx;
    p_json->snull_i = cjson_set_null_idx;
    p_json->rs = cjson_reset;
    p_json->ar_sz = cjson_array_size;
    return p_json;
}



int cjson_delete(cjson* pJson)
{
    if(pJson)
    {
        
        if(pJson->ty == JSON_OBJECT_MASTER && pJson->root)
        {
            cJSON_Delete(pJson->root);
        }
        free(pJson);
        return 1;
    }
    else
    {
        return 0;
    }
}



cjson* cjson_ref(cjson* pJson)
{
    cjson* p_json_ref =  (cjson*)malloc(sizeof(cjson));
    memcpy(p_json_ref, pJson, sizeof(cjson));
    pJson->ty = JSON_OBJECT_REF;
    return p_json_ref;
}

cjson* cjson_reset(cjson* pJson)
{
    if(pJson)
    {
        pJson->child = pJson->root;
        
        return pJson;
    }
    else
    {
        return pJson;
    }
}



bbool cjson_parse(cjson* pJson, const char* strJson)
{
    if(pJson)
    {
        pJson->child = NULL;
        
        if(pJson->root)
        {
            cJSON_Delete(pJson->root);
        }
        pJson->root = cJSON_Parse(strJson);
    }
    return 1;
}

cjson* cjson_obj(cjson* pJson, const char* key)
{
    if(pJson)
    {
        if (pJson->child)
            pJson->child = cJSON_GetObjectItem(pJson->child, key);

        
        return pJson;
    }
    else
    {
        return NULL;
    }
}





cjson* cjson_obj_idx(cjson* pJson, int idx)
{
    if(pJson)
    {
        if(pJson->child)
            pJson->child = cJSON_GetArrayItem(pJson->child, idx);
        return pJson;
    }
    else
    {
        return NULL;
    }
}



bbool cjson_set_obj(cjson* pJson, const char* name, cjson* obj)
{
    if(!pJson || !name || !obj)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;
    cJSON* lp_src_json = obj->child;

    if(!lp_dest_json || lp_dest_json->type != cJSON_Object)
    {
        return 0;
    }
    
    
    if(cJSON_GetObjectItem(lp_dest_json, name)!= NULL)
    {
        cJSON_ReplaceItemInObject(lp_dest_json, name, lp_src_json);
    }
    else
    {
        cJSON_AddItemToObject(lp_dest_json, name, lp_src_json);
    }
    free(obj);
    return 1;
}



bbool cjson_set_str(cjson* pJson, const char* name, char* str)
{
    if(!pJson || !name || !str)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;
    
    if(!lp_dest_json || lp_dest_json->type != cJSON_Object)
    {
        return 0;
    }
    
    if(cJSON_GetObjectItem(lp_dest_json, name)!= NULL)
    {
        cJSON_ReplaceItemInObject(lp_dest_json, name, cJSON_CreateString(str));
    }
    else
    {
        cJSON_AddItemToObject(lp_dest_json, name, cJSON_CreateString(str));
    }
    
    return 1;
}





bbool cjson_set_num(cjson* pJson, const char* name, int num)
{
    if(!pJson || !name)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;
    
    if(!lp_dest_json || lp_dest_json->type != cJSON_Object)
    {
        return 0;
    }
    
    
    if(cJSON_GetObjectItem(lp_dest_json, name)!= NULL)
    {
        cJSON_ReplaceItemInObject(lp_dest_json, name, cJSON_CreateNumber(num));
    }
    else
    {
        cJSON_AddItemToObject(lp_dest_json, name, cJSON_CreateNumber(num));
    }
    
    return 1;
}

bbool cjson_set_bool(cjson* pJson, const char* name, bbool b)
{
    if(!pJson || !name)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;
    
    if(!lp_dest_json ||lp_dest_json->type != cJSON_Object)
    {
        return 0;
    }
    

    if(cJSON_GetObjectItem(lp_dest_json, name)!= NULL)
    {
        cJSON_ReplaceItemInObject(lp_dest_json, name,  (b ? cJSON_CreateTrue() : cJSON_CreateFalse()) );
    }
    else
    {
        cJSON_AddItemToObject(lp_dest_json, name, (b ? cJSON_CreateTrue() : cJSON_CreateFalse()) );
    }
    
    return 1;
}



bbool cjson_set_null(cjson* pJson, const char* name)
{
    if(!pJson || !name)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;
    
    if(!lp_dest_json ||lp_dest_json->type != cJSON_Object)
    {
        return 0;
    }
    
    if(cJSON_GetObjectItem(lp_dest_json, name)!= NULL)
    {
        cJSON_ReplaceItemInObject(lp_dest_json, name,  cJSON_CreateNull() );
    }
    else
    {
        cJSON_AddItemToObject(lp_dest_json, name, cJSON_CreateNull() );
    }
    
    return 1;
}


//////////////////////////////////
//////////////////////////////////
//////////////////////////////////





bbool cjson_set_obj_idx(cjson* pJson, int idx, cjson* obj)
{
    if(!pJson || !obj)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;
    cJSON* lp_src_json = obj->child;

    
    if(!lp_dest_json ||lp_dest_json->type != cJSON_Array)
    {
        return 0;
    }

    if(cJSON_GetArrayItem(lp_dest_json, idx)!= NULL)
    {
        cJSON_ReplaceItemInArray(lp_dest_json, idx, lp_src_json);
    }
    else
    {
        cJSON_AddItemToArray(lp_dest_json, lp_src_json);
    }
    
    free(obj);
    return 1;
}



bbool cjson_set_str_idx(cjson* pJson, int idx, char* str)
{
    if(!pJson || !str)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;

    if(!lp_dest_json ||lp_dest_json->type != cJSON_Array)
    {
        return 0;
    }
    
    if(cJSON_GetArrayItem(lp_dest_json, idx)!= NULL)
    {
        cJSON_ReplaceItemInArray(lp_dest_json, idx, cJSON_CreateString(str));
    }
    else
    {
        cJSON_AddItemToArray(lp_dest_json, cJSON_CreateString(str));
    }
    
    return 1;
}





bbool cjson_set_num_idx(cjson* pJson, int idx, int num)
{
    if(!pJson)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;

    if(!lp_dest_json || lp_dest_json->type != cJSON_Array)
    {
        return 0;
    }
    
    if(cJSON_GetArrayItem(lp_dest_json, idx)!= NULL)
    {
        cJSON_ReplaceItemInArray(lp_dest_json, idx, cJSON_CreateNumber(num));
    }
    else
    {
        cJSON_AddItemToArray(lp_dest_json, cJSON_CreateNumber(num));
    }

    
    return 1;
}

bbool cjson_set_bool_idx(cjson* pJson, int idx, bbool b)
{
    if(!pJson)
    {
        return 0;
    }
    cJSON* lp_dest_json = pJson->child;

    if(!lp_dest_json || lp_dest_json->type != cJSON_Array)
    {
        return 0;
    }

    if(cJSON_GetArrayItem(lp_dest_json, idx)!= NULL)
    {
        cJSON_ReplaceItemInArray(lp_dest_json, idx, (b ? cJSON_CreateTrue() : cJSON_CreateFalse()) );
    }
    else
    {
        cJSON_AddItemToArray(lp_dest_json, (b ? cJSON_CreateTrue() : cJSON_CreateFalse()) );
    }

    
    return 1;
}



bbool cjson_set_null_idx(cjson* pJson, int idx)
{
    if(!pJson)
    {
        return 0;
    }
    
    cJSON* lp_dest_json = pJson->child;
    
    if(!lp_dest_json || lp_dest_json->type != cJSON_Array)
    {
        return 0;
    }

    if(cJSON_GetArrayItem(lp_dest_json, idx)!= NULL)
    {
        cJSON_ReplaceItemInArray(lp_dest_json, idx, cJSON_CreateNull() );
    }
    else
    {
        cJSON_AddItemToArray(lp_dest_json, cJSON_CreateNull() );
    }
    
    
    return 1;
}






size_t cjson_array_size(cjson* pJson)
{
    if(pJson==NULL || pJson->child == NULL)
    {
        return 0;
    }
    
    cJSON* lp_json_ret = pJson->child;
    if(lp_json_ret->type != cJSON_Array)
    {
        return 0;
    }
    return cJSON_GetArraySize(lp_json_ret);
}


char* cjson_obj_str(cjson* pJson)
{

    if(pJson==NULL || pJson->child == NULL)
    {
        //throw runtime_error("json字符串格式错误");
        return calloc(1,1);
    }
    char* lp_str_ret = NULL;
    cJSON* lp_json_ret = pJson->child;
    switch (lp_json_ret->type) {
    case cJSON_False:
        lp_str_ret = strdup("false");
        break;
    case cJSON_True:
        lp_str_ret = strdup("true");
        break;
    case cJSON_NULL:
        lp_str_ret =  strdup("null");
        break;
    case cJSON_Number:   
        lp_str_ret = cJSON_Print(lp_json_ret);
        break;
    case cJSON_String:
        lp_str_ret = strdup(lp_json_ret->valuestring);
        break;
    case cJSON_Array:
    case cJSON_Object:
        lp_str_ret = cJSON_Print(lp_json_ret);
        break;
    }

    return lp_str_ret;

    
}
