#ifndef BIDX_HASH_H
#define BIDX_HASH_H

#include "bIdxDef.h"
#include "bIdxArray.h"
#include <glib.h>
typedef void (*bFnFreeVal) (void*);
typedef struct st_bPreVal
{
    bIdxArray* data;
    char* suffix;
    struct st_bPreVal *next;
}bPreVal;

typedef struct st_bAllVal
{
    bIdxArray* data;
}bAllVal;

typedef struct
{
    GHashTable* allHash;
    GHashTable* preHash;
}bIdxHash;
#define ALLSTR_SIGN "." 
#define dup_all_string(pre, suf, all)                   \
    do                                                  \
    {                                                   \
        int n_pre_len = strlen(pre);                    \
        int n_suf_len = strlen(suf);                    \
        int n_all_len = n_pre_len + n_suf_len + 2;      \
        all = (char*)malloc(n_all_len);                 \
        memcpy(all, pre, n_pre_len);                    \
        memcpy((all) + n_pre_len, ".", 1);              \
        memcpy((all) + n_pre_len + 1, suf, n_suf_len);  \
        (all)[n_all_len - 1] = '\0';                    \
    }while(FALSE)



#define split_all_string(pre, suf, all)                 \
    do                                                  \
    {                                                   \
        bbyte* p = strchr(all, '.');                     \
        pre = all;                                      \
        suf = p + 1;                                    \
        *p = '\0';                                      \
    }while(FALSE)



bIdxHash* bIdxHash_new();

bbool bIdxHash_delete(bIdxHash* pIdxHash);

bbool bIdxHash_insert(bIdxHash* pIdxHash, char* prefix, char* suffix, bIdxArray* data);

bAllVal* bIdxHash_lookup_all(bIdxHash* pIdxHash, char* all);

bPreVal* bIdxHash_lookup_pre(bIdxHash* pIdxHash, char* prefix);



#endif
