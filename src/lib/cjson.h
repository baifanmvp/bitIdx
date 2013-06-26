
#ifndef cjson__H_
#define cjson__H_

#include "cJSON.h"
#define JSON_TYPE_BOOL     1
#define JSON_TYPE_NUM      2
#define JSON_TYPE_STRING   3
#define JSON_TYPE_ARRAY    4
#define JSON_OBJECT_MASTER  1
#define JSON_OBJECT_REF 0
typedef struct st_cjson cjson;
typedef  int cbool;
typedef cbool (*fn_str) (cjson* this, const char*);
typedef cjson* (*fn_objson) (cjson* this, const char*);
typedef cjson* (*fn_objson_idx) (cjson* this, int idx);

typedef cjson* (*fn_json_reset) (cjson*);

typedef cbool (*fn_set_objson) (cjson* this, const char*, cjson* obj);

typedef cbool (*fn_set_strjson) (cjson* this, const char*, char* str);

typedef cbool (*fn_set_numjson) (cjson* this, const char*, int num);

typedef cbool (*fn_set_booljson) (cjson* this, const char*, cbool b);

typedef cbool (*fn_set_nulljson) (cjson* this, const char*);



typedef cbool (*fn_set_objson_i) (cjson* this, int idx, cjson* obj);

typedef cbool (*fn_set_strjson_i) (cjson* this, int idx, char* str);

typedef cbool (*fn_set_numjson_i) (cjson* this, int idx, int num);

typedef cbool (*fn_set_booljson_i) (cjson* this, int idx, cbool b);

typedef cbool (*fn_set_nulljson_i) (cjson* this, int idx );


typedef char* (*fn_objson_str) (cjson* this);

typedef size_t (*fn_objson_arrsize) (cjson* this);

typedef cjson* (*fn_cjson_ref)(cjson* this);

struct st_cjson
{
    
    cJSON* root;
    cJSON* child;
    unsigned short ty;
    fn_str parse;
    fn_cjson_ref ref;
    
    fn_objson obj;
    fn_objson_idx obj_i;
    fn_json_reset rs;
    
    fn_set_objson sobj;
    fn_set_strjson sstr;
    fn_set_numjson snum;
    fn_set_booljson sbool;
    fn_set_nulljson snull;

    fn_set_objson_i sobj_i;
    fn_set_strjson_i sstr_i;
    fn_set_numjson_i snum_i;
    fn_set_booljson_i sbool_i;
    fn_set_nulljson_i snull_i;

    
    fn_objson_str obj_s;
    fn_objson_arrsize ar_sz;
};

#define CJSON_MEMBER_POS(n) ( ((cjson*)(0))->(n) )

cjson* cjson_new(const char* strJson);

int cjson_delete(cjson* pJson);

cjson* cjson_reset(cjson* pJson);

cbool cjson_parse(cjson* pJson, const char* strJson);

cjson* cjson_ref(cjson* pJson);

cjson* cjson_obj(cjson* pJson, const char* key);

cjson* cjson_obj_idx(cjson* pJson, int idx);



cbool cjson_set_obj(cjson* pJson, const char* name, cjson* obj);

cbool cjson_set_str(cjson* pJson, const char* name, char* str);


cbool cjson_set_num(cjson* pJson, const char* name, int num);

cbool cjson_set_bool(cjson* pJson, const char* name, cbool b);

cbool cjson_set_null(cjson* pJson, const char* name);


cbool cjson_set_obj_idx(cjson* pJson, int idx, cjson* obj);

cbool cjson_set_str_idx(cjson* pJson, int idx, char* str);


cbool cjson_set_num_idx(cjson* pJson, int idx, int num);

cbool cjson_set_bool_idx(cjson* pJson, int idx, cbool b);

cbool cjson_set_null_idx(cjson* pJson, int idx );


size_t cjson_array_size(cjson* pJson);

char* cjson_obj_str(cjson* pJson);

#endif /* SPDJSON_H_ */
