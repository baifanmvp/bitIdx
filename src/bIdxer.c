#include "bIdxer.h"


static type_t bIdxer_get_needFields(bIdxer* pbIdxer);
    
static char* bIdxer_get_labelTag(bIdxer* pbIdxer, bIdxArray** pLoopArrays, bid_t id, char* label);

static char* bIdxer_get_otherTag(bIdxer* pbIdxer, char* org , char* tag, bid_t id, char* tagbuf);

static bIdxArray* bIdxer_get_array_byMatchFields(bIdxer* pbIdxer);

static cjson* bIdxer_get_tag_byId(bIdxer* pbIdxer, bIdxBasRes *pIdRes);

static bIdxBasRes* bIdxer_get_ids_byEql(bIdxer* pbIdxer, char* query, char* orderby);








bIdxer* bIdxer_new(char* bitPath, char* ip, int port)
{
    bIdxer* p_bidxer = (bIdxer*)malloc(sizeof(bIdxer));
    
    p_bidxer->pBasOp = bIdxBasOp_new(bitPath);
    p_bidxer->jsoner = cjson_new("{}");
    p_bidxer->peql = eqlClient_new(EQL_CLIENT_INET, ip, port);
    return p_bidxer;
    
}
bbool bIdxer_delete(bIdxer* pbIdxer)
{
    if(pbIdxer)
    {
        bIdxBasOp_delete(pbIdxer->pBasOp);
        cjson_delete(pbIdxer->jsoner);
        eqlClient_delete(pbIdxer->peql);
        
        free(pbIdxer);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


char* bIdxer_query(bIdxer* pbIdxer, char* query)
{
    if(!pbIdxer || !query)
    {
        return NULL;
    }
    cjson* jsoner = pbIdxer->jsoner;
    cjson_parse(jsoner, query);
    char* lp_op = jsoner->obj(jsoner, BIDX_JSONKEY_op)->obj_s(jsoner);
    char* lp_res_str = NULL;

    if (strcmp(lp_op, BIDX_JSONVAL_ADDTAG) == 0)
    {
        lp_res_str = bIdxer_query_addtag(pbIdxer);  
    }
    else if (strcmp(lp_op, BIDX_JSONVAL_RMTAG) == 0)
    {
        lp_res_str = bIdxer_query_rmtag(pbIdxer);  
    }
    else if (strcmp(lp_op, BIDX_JSONVAL_QRYID) == 0)
    {
        lp_res_str = bIdxer_query_qryid(pbIdxer);
    }
    else if (strcmp(lp_op, BIDX_JSONVAL_QRYCND) == 0)
    {
        lp_res_str = bIdxer_query_qrycnd(pbIdxer);
    }
    else if (strcmp(lp_op, BIDX_JSONVAL_UTAGID) == 0)
    {
        lp_res_str = bIdxer_query_utagid(pbIdxer);  
    }
    else if (strcmp(lp_op, BIDX_JSONVAL_UTAGCND) == 0)
    {
        lp_res_str = bIdxer_query_utagcnd(pbIdxer);
    }
    return lp_res_str; 

    
}


char* bIdxer_query_addtag(bIdxer* pbIdxer)
{
    cjson* jsoner = pbIdxer->jsoner;

     char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);
    char* lp_pre_tag = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_fieldId)->obj_s(jsoner);
    size_t n_tag_cnt = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_tagId)->ar_sz(jsoner);
    cjson* p_tags_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_tagId);                          

    size_t idx = 0;

    int code = 0;
    cjson* lp_res_jsoner = cjson_new("{}");
    lp_res_jsoner->sstr(lp_res_jsoner, BIDX_JSONKEY_op, BIDX_JSONVAL_ADDTAG);
    
    cjson* lp_untag_jsoner = cjson_new("[]");
    
    while(idx < n_tag_cnt)
    {
        cjson* p_tags_tmp = p_tags_json->ref(p_tags_json);    
        char* lp_suf_tag = p_tags_tmp->obj_i(p_tags_tmp, idx)->obj_s(p_tags_tmp);
        char* lp_all_tag = NULL;
        
        dup_all_string(lp_pre_tag, lp_suf_tag, lp_all_tag);
        
        if(!bIdxBasOp_add_array(pbIdxer->pBasOp, org, lp_all_tag))
        {
            code = -1;
            cjson* lp_tag_jsoner = cjson_new("{}");
            lp_tag_jsoner->sstr(lp_tag_jsoner, lp_pre_tag, lp_suf_tag);
            
            lp_untag_jsoner->sobj_i(lp_untag_jsoner, 99999, lp_tag_jsoner);
            //  cjson_delete(lp_tag_jsoner);
        }

        free(lp_suf_tag);
        free(lp_all_tag);
        cjson_delete(p_tags_tmp);
        idx++;
    }
    lp_res_jsoner->snum(lp_res_jsoner, BIDX_JSONKEY_code, code);

    if(code == -1)
    {
        lp_res_jsoner->sobj(lp_res_jsoner, BIDX_JSONKEY_failTag, lp_untag_jsoner);
    }
    else
    {
        cjson_delete(lp_untag_jsoner);
        
    }

    
    char* lp_ret_str = lp_res_jsoner->obj_s(lp_res_jsoner);
    cjson_delete(lp_res_jsoner);
    free(lp_pre_tag);
    free(org);
    return lp_ret_str;
}




char* bIdxer_query_rmtag(bIdxer* pbIdxer)
{
    cjson* jsoner = pbIdxer->jsoner;

    char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);
    char* lp_pre_tag = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_fieldId)->obj_s(jsoner);
    size_t n_tag_cnt = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_tagId)->ar_sz(jsoner);
    cjson* p_tags_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_tagId);                          

    size_t idx = 0;

    int code = 0;
    cjson* lp_res_jsoner = cjson_new("{}");
    lp_res_jsoner->sstr(lp_res_jsoner, BIDX_JSONKEY_op, BIDX_JSONVAL_RMTAG);
    
    cjson* lp_untag_jsoner = cjson_new("[]");
    
    while(idx < n_tag_cnt)
    {
        cjson* p_tags_tmp = p_tags_json->ref(p_tags_json);    
        char* lp_suf_tag = p_tags_tmp->obj_i(p_tags_tmp, idx)->obj_s(p_tags_tmp);
        char* lp_all_tag = NULL;
        
        dup_all_string(lp_pre_tag, lp_suf_tag, lp_all_tag);
        
        if(!bIdxBasOp_rm_array(pbIdxer->pBasOp, org, lp_all_tag))
        {
            code = -1;
            cjson* lp_tag_jsoner = cjson_new("{}");
            lp_tag_jsoner->sstr(lp_tag_jsoner, lp_pre_tag, lp_suf_tag);
            
            lp_untag_jsoner->sobj_i(lp_untag_jsoner, 99999, lp_tag_jsoner);
        }

        free(lp_suf_tag);
        free(lp_all_tag);
        cjson_delete(p_tags_tmp);
        idx++;
    }
    lp_res_jsoner->snum(lp_res_jsoner, BIDX_JSONKEY_code, code);

    if(code == -1)
    {
        lp_res_jsoner->sobj(lp_res_jsoner, BIDX_JSONKEY_failTag, lp_untag_jsoner);
    }
    else
    {
        cjson_delete(lp_untag_jsoner);
    }
    
    char* lp_ret_str = lp_res_jsoner->obj_s(lp_res_jsoner);
    cjson_delete(lp_res_jsoner);
    free(lp_pre_tag);
    free(org);
    return lp_ret_str;
}


char* bIdxer_query_qryid(bIdxer* pbIdxer)
{
    cjson* jsoner = pbIdxer->jsoner;

    char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);
    size_t idx = 0;

    int code = 0;
    cjson* lp_res_json = cjson_new("{}");
    lp_res_json->sstr(lp_res_json, BIDX_JSONKEY_op, BIDX_JSONVAL_QRYID);
    lp_res_json->snum(lp_res_json, BIDX_JSONKEY_code, 0);
    
    bIdxArray** lp_org_arrays = bIdxBasOp_lookupOrg_array(pbIdxer->pBasOp, org);


//get id
    cjson* p_ids_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_docId)->ref(jsoner); 
    size_t n_ids_cnt = p_ids_json->ar_sz(p_ids_json);
    bIdxBasRes *p_id_res = NULL;
    size_t n_real_cnt = 0;
    BIDXIDRES_INIT(p_id_res, n_ids_cnt);
    while(idx < n_ids_cnt)
    {
        cjson* p_ids_tmp = p_ids_json->ref(p_ids_json);    
        char* lp_id = p_ids_tmp->obj_i(p_ids_tmp, idx)->obj_s(p_ids_tmp);
        bid_t id = atoi(lp_id);

        bIdxArray** lp_loop_arrays = lp_org_arrays;
        while(*lp_loop_arrays)
        {
            bbyte val;
            bIdxArray_get_bitVal(*lp_loop_arrays, id, val);
            if(val)
            {
                BIDXIDRES_SETID(p_id_res, n_real_cnt, id );
                n_real_cnt++;
                break;
            }
            (lp_loop_arrays)++;
        }
        cjson_delete(p_ids_tmp);
        free(lp_id);
        idx++;
    }
    BIDXIDRES_CNT(p_id_res) = n_real_cnt;


//get idTag info
    cjson* lp_record_json = bIdxer_get_tag_byId(pbIdxer, p_id_res);
    lp_record_json->snum(lp_record_json, BIDX_JSONKEY_total, BIDXIDRES_CNT(p_id_res));
    
    lp_res_json->sobj(lp_res_json, BIDX_JSONKEY_data, lp_record_json);
    lp_res_json->snum(lp_res_json, BIDX_JSONKEY_code, code);
    
    char* lp_ret_str = lp_res_json->obj_s(lp_res_json);
    
    cjson_delete(p_ids_json);
    cjson_delete(lp_res_json);
    free(lp_org_arrays);
    free(org);
    return lp_ret_str;
}

char* bIdxer_query_qrycnd(bIdxer* pbIdxer)
{
    cjson* jsoner = pbIdxer->jsoner;
    cjson* lp_res_json = cjson_new("{}");
    int code = 0;
    
    lp_res_json->sstr(lp_res_json, BIDX_JSONKEY_op, BIDX_JSONVAL_UTAGCND);

    char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);
    
    

    //通过搜索条件得到idRes
    
    bIdxArray* lp_id_array = bIdxer_get_array_byMatchFields(pbIdxer);
    bIdxBasRes* lp_id_res = NULL;
    
    char* lp_offset_str = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_offset)->obj_s(jsoner);
    char* lp_cnt_str = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_maxLen)->obj_s(jsoner);
    size_t n_id_start = atoi(lp_offset_str);
    size_t n_id_cnt = atoi(lp_cnt_str);
    
    free(lp_offset_str);
    free(lp_cnt_str);
    
        
    char* lp_eql_query = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_matchEql)->obj_s(jsoner);
    if(strlen(lp_eql_query))
    {
        char* lp_eql_orderby = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_orderEql)->obj_s(jsoner);
        bIdxBasRes* lp_eql_res = bIdxer_get_ids_byEql(pbIdxer, lp_eql_query, lp_eql_orderby);

        lp_id_res =  bIdx_resAndArray(lp_eql_res, lp_id_array, n_id_start, n_id_cnt);

        
        free(lp_eql_orderby);
        BIDXIDRES_DESTORY(lp_eql_res);
    }
    else
    {
        lp_id_res =  bIdxArray_get_idResult(lp_id_array, n_id_start, n_id_cnt);
    }
    
    free(lp_eql_query);
    
//get idTag info
    cjson* lp_record_json = bIdxer_get_tag_byId(pbIdxer, lp_id_res);    
    lp_res_json->sobj(lp_res_json, BIDX_JSONKEY_data, lp_record_json);

    lp_res_json->snum(lp_res_json, BIDX_JSONKEY_code, code);

    char* lp_ret_str = lp_res_json->obj_s(lp_res_json);

    
    bIdxArray_delete(lp_id_array);
    BIDXIDRES_DESTORY(lp_id_res);
    cjson_delete(lp_res_json);
    free(org);
    return lp_ret_str;
}


char* bIdxer_query_utagid(bIdxer* pbIdxer)
{
    cjson* jsoner = pbIdxer->jsoner;
    cjson* lp_res_json = cjson_new("{}");
    lp_res_json->sstr(lp_res_json, BIDX_JSONKEY_op, BIDX_JSONVAL_UTAGID);    
    int code = 0;

    
    char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);
    size_t idx;
    
    
//get id
    cjson* p_ids_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_docId)->ref(jsoner); 
    size_t n_ids_cnt = p_ids_json->ar_sz(p_ids_json);
    bIdxBasRes *p_id_res = NULL;
    BIDXIDRES_INIT(p_id_res, n_ids_cnt);
    idx = 0;
    while(idx < n_ids_cnt)
    {
        cjson* p_ids_tmp = p_ids_json->ref(p_ids_json);    
        char* lp_id = p_ids_tmp->obj_i(p_ids_tmp, idx)->obj_s(p_ids_tmp);
        bid_t id = atoi(lp_id);
        BIDXIDRES_SETID(p_id_res, idx, id );
        
        cjson_delete(p_ids_tmp);
        free(lp_id);
        idx++;
    }
    

//add tag
    char* lp_addfield = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_addField)->obj(jsoner, BIDX_JSONKEY_fieldId)->obj_s(jsoner);
    cjson* lp_addTag_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_addField)->obj(jsoner, BIDX_JSONKEY_tagId)->ref(jsoner);
    size_t n_addTag_cnt = lp_addTag_json->ar_sz(lp_addTag_json);
    idx = 0;
    while(idx < n_addTag_cnt)
    {
        cjson* lp_tmp_json = lp_addTag_json->ref(lp_addTag_json);
        char* lp_suf_tag = lp_tmp_json->obj_i(lp_tmp_json, idx)->obj_s(lp_tmp_json);
        char* lp_all_tag = NULL;
        
        dup_all_string(lp_addfield, lp_suf_tag, lp_all_tag);

        bIdxArray* lp_array = bIdxBasOp_lookup_array(pbIdxer->pBasOp, org, lp_all_tag);
        if(lp_array)
        {
            bIdxBasOp_modify_atoi(pbIdxer->pBasOp, lp_array,  p_id_res);
        }
        else
        {
            code = -1;
        }
        
        free(lp_suf_tag);
        free(lp_all_tag);
        idx++;
    }

    
    //rm tag
    char* lp_rmfield = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_rmField)->obj(jsoner, BIDX_JSONKEY_fieldId)->obj_s(jsoner);
    cjson* lp_rmTag_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_rmField)->obj(jsoner, BIDX_JSONKEY_tagId)->ref(jsoner);
    size_t n_rmTag_cnt = lp_rmTag_json->ar_sz(lp_rmTag_json);
    idx = 0;
    while(idx < n_rmTag_cnt)
    {
        cjson* lp_tmp_json = lp_rmTag_json->ref(lp_rmTag_json);
        char* lp_suf_tag = lp_tmp_json->obj_i(lp_tmp_json, idx)->obj_s(lp_tmp_json);
        char* lp_all_tag = NULL;
        
        dup_all_string(lp_rmfield, lp_suf_tag, lp_all_tag);

        bIdxArray* lp_array = bIdxBasOp_lookup_array(pbIdxer->pBasOp, org, lp_all_tag);
        if(lp_array)
        {
            bIdxBasOp_remove_atoi(pbIdxer->pBasOp, lp_array,  p_id_res);
        }
        else
        {
            code = -1;
        }
        
        free(lp_suf_tag);
        free(lp_all_tag);
        idx++;
    }

    lp_res_json->snum(lp_res_json, BIDX_JSONKEY_code, code);
    char* lp_ret_str = lp_res_json->obj_s(lp_res_json);
    
    cjson_delete(lp_res_json);
    return lp_ret_str;
}









char* bIdxer_query_utagcnd(bIdxer* pbIdxer)
{
    cjson* jsoner = pbIdxer->jsoner;
    cjson* lp_res_json = cjson_new("{}");
    int code = 0;
    size_t idx;
    
    lp_res_json->sstr(lp_res_json, BIDX_JSONKEY_op, BIDX_JSONVAL_UTAGCND);

    char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);
    
    

    
    
    bIdxArray* lp_id_array = bIdxer_get_array_byMatchFields(pbIdxer);

    char* lp_eql_query = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_matchEql)->obj_s(jsoner);
    
    if(strlen(lp_eql_query))
    {
        bIdxBasRes* lp_eql_res = bIdxer_get_ids_byEql(pbIdxer, lp_eql_query, NULL);
        bIdxArray* lp_eql_array = bIdxIdRes_get_idxArray(lp_eql_res, 0, BIDXIDRES_CNT(lp_eql_res));

        lp_id_array = bIdxBasOp_searchAnd_atoa(pbIdxer->pBasOp, lp_id_array,  lp_eql_array);
        
        BIDXIDRES_DESTORY(lp_eql_res);
        bIdxArray_delete(lp_eql_array);
    }
    
    free(lp_eql_query);
    

//add tag
    char* lp_addfield = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_addField)->obj(jsoner, BIDX_JSONKEY_fieldId)->obj_s(jsoner);
    cjson* lp_addTag_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_addField)->obj(jsoner, BIDX_JSONKEY_tagId)->ref(jsoner);
    size_t n_addTag_cnt = lp_addTag_json->ar_sz(lp_addTag_json);
    idx = 0;
    while(idx < n_addTag_cnt)
    {
        cjson* lp_tmp_json = lp_addTag_json->ref(lp_addTag_json);
        char* lp_suf_tag = lp_tmp_json->obj_i(lp_tmp_json, idx)->obj_s(lp_tmp_json);
        char* lp_all_tag = NULL;
        
        dup_all_string(lp_addfield, lp_suf_tag, lp_all_tag);

        bIdxArray* lp_array = bIdxBasOp_lookup_array(pbIdxer->pBasOp, org, lp_all_tag);
        if(lp_array)
        {
            bIdxBasOp_modify_atoa(pbIdxer->pBasOp, lp_array,  lp_id_array);
        }
        else
        {
            code = -1;
        }
        
        free(lp_suf_tag);
        free(lp_all_tag);
        idx++;
    }

    
    //rm tag
    char* lp_rmfield = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_rmField)->obj(jsoner, BIDX_JSONKEY_fieldId)->obj_s(jsoner);
    cjson* lp_rmTag_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_rmField)->obj(jsoner, BIDX_JSONKEY_tagId)->ref(jsoner);
    size_t n_rmTag_cnt = lp_rmTag_json->ar_sz(lp_rmTag_json);
    idx = 0;
    while(idx < n_rmTag_cnt)
    {
        cjson* lp_tmp_json = lp_rmTag_json->ref(lp_rmTag_json);
        char* lp_suf_tag = lp_tmp_json->obj_i(lp_tmp_json, idx)->obj_s(lp_tmp_json);
        char* lp_all_tag = NULL;
        
        dup_all_string(lp_rmfield, lp_suf_tag, lp_all_tag);

        bIdxArray* lp_array = bIdxBasOp_lookup_array(pbIdxer->pBasOp, org, lp_all_tag);
        if(lp_array)
        {
            bIdxBasOp_remove_atoa(pbIdxer->pBasOp, lp_array,  lp_id_array);
        }
        else
        {
            code = -1;
        }
        
        free(lp_suf_tag);
        free(lp_all_tag);
        idx++;
    }

    lp_res_json->snum(lp_res_json, BIDX_JSONKEY_code, code);
    char* lp_ret_str = lp_res_json->obj_s(lp_res_json);
    
    cjson_delete(lp_res_json);
    return lp_ret_str;
}
















///status function///////////////////////////////////////////

type_t bIdxer_get_needFields(bIdxer* pbIdxer)
{
    cjson* jsoner = pbIdxer->jsoner;
    type_t n_tag_type = 0;
    char* p_needDoc = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_needDoc)->obj_s(jsoner);
    
    if(p_needDoc && strcmp(p_needDoc, "true") == 0)
    {
        char* p_needFields = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_needFields)->obj_s(jsoner);
        if(strstr(p_needFields, "LABEL"))
        {
            n_tag_type |= BIDX_TAGTYPE_LABEL;
        }
        if(strstr(p_needFields, "READ"))
        {
            n_tag_type |= BIDX_TAGTYPE_READ;
        }
        if(strstr(p_needFields, "STAR"))
        {
            n_tag_type |= BIDX_TAGTYPE_STAR;
        }
        if(strstr(p_needFields, "IMP"))
        {
            n_tag_type |= BIDX_TAGTYPE_IMP;
        }
        free(p_needFields);
    }
    free(p_needDoc);
    return n_tag_type;
}
char* bIdxer_get_labelTag(bIdxer* pbIdxer, bIdxArray** pLoopArrays, bid_t id, char* label)
{
    char* p_iter_label = label;

    while(*pLoopArrays)
    {

        char* p_tag = strchr(((*pLoopArrays))->allKey, '.') + 1;

        size_t n_tag_len = strlen(p_tag);
        
        if( (p_iter_label + n_tag_len) < (label + 4096 -2)
            && n_tag_len > strlen(BIDX_JSONKEY_LABEL)
            && strncmp(p_tag, BIDX_JSONKEY_LABEL, 5) == 0)
        {
            bbyte val;
            bIdxArray_get_bitVal((*pLoopArrays), id, val);
            if(val)
            {
                char* p_real_tag  = p_tag;
                memcpy(p_iter_label, p_real_tag + 6, n_tag_len - 6);
                p_iter_label += n_tag_len - 6;
                *p_iter_label = ',';
                p_iter_label ++;
            }
        }    
        pLoopArrays++;
    }
    
    return p_iter_label;

}

char* bIdxer_get_otherTag(bIdxer* pbIdxer, char* org , char* tag, bid_t id, char* tagbuf)
{
    bIdxArray* lp_array = bIdxBasOp_lookup_array(pbIdxer->pBasOp, org, tag);
    bbyte val ;
    char* p_iter_tag = tagbuf;
    if(lp_array)
    {
        bIdxArray_get_bitVal(lp_array, id, val);
        if(val)
        {
            char* p_real_tag  = strchr((lp_array)->allKey, '.') + 1;
            char* p_suf_tag  = strchr(p_real_tag, '.') + 1;
            memcpy(p_iter_tag, p_suf_tag, 1);
            p_iter_tag ++;
            *p_iter_tag = ',';
            p_iter_tag ++;
        }
    }
    return  p_iter_tag;
    
}

bIdxArray* bIdxer_get_array_byMatchFields(bIdxer* pbIdxer)
{
    if(!pbIdxer)
    {
        return NULL;
    }
    cjson* jsoner = pbIdxer->jsoner;
    
    cjson* lp_matchTag_json = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_matchFields)->ref(jsoner);
    size_t n_matchTag_cnt = lp_matchTag_json->ar_sz(lp_matchTag_json);
    char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);

    flag_t first_flag = TRUE;
//get id
    bIdxArray* lp_id_array = bIdxArray_new();
    bindex_t idx = 0;
    while(idx < n_matchTag_cnt)
    {

        cjson* p_pre_js_tmp = lp_matchTag_json->ref(lp_matchTag_json);
        char* lp_pre_tag = p_pre_js_tmp->obj(p_pre_js_tmp, BIDX_JSONKEY_fieldId)->obj_s(p_pre_js_tmp);

        cjson* p_suf_js_tmp = lp_matchTag_json->ref(lp_matchTag_json);
        p_suf_js_tmp=p_suf_js_tmp->obj(p_suf_js_tmp, BIDX_JSONKEY_tagId);

        size_t n_suf_cnt = p_suf_js_tmp->ar_sz(p_suf_js_tmp);
        size_t jdx = 0;
        while(jdx < n_suf_cnt)
        {
            cjson* p_jdx_tmp = p_suf_js_tmp->ref(p_suf_js_tmp);
            char* lp_suf_tag = p_jdx_tmp->obj_i(p_jdx_tmp, jdx)->obj_s(p_jdx_tmp);
            char* lp_all_tag = NULL;
            dup_all_string(lp_pre_tag, lp_suf_tag, lp_all_tag);

            //因为lp_id_array初始化的时候所有位置bit（NULL）都是不存在的，所以如果直接做and，
            //则获取不到一个结果，所以第一次要先做个or，相当于复制第一个lp_src_array里的数据
            if(first_flag)
                lp_id_array = bIdxBasOp_searchOr_atoa(pbIdxer->pBasOp, lp_id_array,  bIdxBasOp_lookup_array(pbIdxer->pBasOp, org, lp_all_tag));
            else
                lp_id_array = bIdxBasOp_searchAnd_atoa(pbIdxer->pBasOp, lp_id_array,  bIdxBasOp_lookup_array(pbIdxer->pBasOp, org, lp_all_tag));
            
            first_flag = FALSE;
            cjson_delete(p_jdx_tmp);
            free(lp_suf_tag);
            free(lp_all_tag);
            jdx++;
        }
        
        cjson_delete(p_pre_js_tmp);
        cjson_delete(p_suf_js_tmp);
        free(lp_pre_tag);        
        idx++;
    }
    free(org);
    return lp_id_array;
}


cjson* bIdxer_get_tag_byId(bIdxer* pbIdxer, bIdxBasRes *pIdRes)
{
    if(!pbIdxer || !pIdRes)
    {
        return NULL;
    }
    cjson* jsoner = pbIdxer->jsoner;
    
        //get tag info
    char* org = jsoner->rs(jsoner)->obj(jsoner, BIDX_JSONKEY_org)->obj_s(jsoner);
    
    type_t n_tag_type = bIdxer_get_needFields(pbIdxer);
    size_t n_ids_cnt = BIDXIDRES_CNT(pIdRes);
    
    bIdxArray** lp_org_arrays = bIdxBasOp_lookupOrg_array(pbIdxer->pBasOp, org);
    
    
    cjson* lp_record_json = cjson_new("{}");
    char label[4096];
    char read[64];
    char star[64];
    char imp[64];
    
    cjson* lp_tags_json = cjson_new("[]");

    bindex_t idx = 0;
    while(idx < n_ids_cnt)
    {
        cjson* lp_id_json = cjson_new("{}");
        bid_t id = BIDXIDRES_GETID(pIdRes, idx);
        lp_id_json->snum(lp_id_json, "doc_id", id);
        char* p_iter_label = label;
        char* p_iter_read = read;
        char* p_iter_star = star;
        char* p_iter_imp = imp;

        if (n_tag_type & BIDX_TAGTYPE_LABEL)
        {
            p_iter_label = bIdxer_get_labelTag(pbIdxer, lp_org_arrays, id, p_iter_label);
        }
            
        if (n_tag_type & BIDX_TAGTYPE_READ)
        {
            
            p_iter_read = bIdxer_get_otherTag(pbIdxer, org , "READ.0", id, p_iter_read);
            p_iter_read = bIdxer_get_otherTag(pbIdxer, org , "READ.1", id, p_iter_read);            
        }
        
        if (n_tag_type & BIDX_TAGTYPE_STAR)
        {
            p_iter_star = bIdxer_get_otherTag(pbIdxer, org , "STAR.0", id, p_iter_star);
            p_iter_star = bIdxer_get_otherTag(pbIdxer, org , "STAR.1", id, p_iter_star);            
        }
        
        if (n_tag_type & BIDX_TAGTYPE_IMP)
        {
            p_iter_imp = bIdxer_get_otherTag(pbIdxer, org , "IMP.0", id, p_iter_imp);
            p_iter_imp = bIdxer_get_otherTag(pbIdxer, org , "IMP.1", id, p_iter_imp);            
        }
        *p_iter_label = '\0';
        *p_iter_read = '\0';
        *p_iter_star = '\0';
        *p_iter_imp = '\0';
        lp_id_json->sstr(lp_id_json, "LABEL", label);
        lp_id_json->sstr(lp_id_json, "READ", read);
        lp_id_json->sstr(lp_id_json, "STAR", star);
        lp_id_json->sstr(lp_id_json, "IMP", imp);
        lp_tags_json->sobj_i(lp_tags_json, lp_tags_json->ar_sz(lp_tags_json), lp_id_json);

//        cjson_delete(lp_id_json);
        
        idx++;
    }
    lp_record_json->sobj(lp_record_json, BIDX_JSONKEY_record, lp_tags_json);
    return lp_record_json;

}


bIdxBasRes* bIdxer_get_ids_byEql(bIdxer* pbIdxer, char* query, char* orderby)
{
    return NULL;
}
