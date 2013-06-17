#ifndef BIDX_HASH_H
#define BIDX_HASH_H

#include "bIdxDef.h"
#include <glib.h>
typedef struct st_bPreVal
{
    void* data;
    char* suffix;
    struct st_bPreVal *next;
}bPreVal;

typedef struct st_bAllVal
{
    void* data;
}bAllVal;

typedef struct
{
    GHashTable* allHash;
    GHashTable* preHash;
}bIdxHash;

#define dup_all_string(pre, suf, all)                   \
    do                                                  \
    {                                                   \
        int n_pre_len = strlen(pre);                    \
        int n_suf_len = strlen(suf);                    \
        int n_all_len = n_pre_len + n_suf_len + 2;      \
        all = (char*)malloc(n_all_len);                 \
        memcpy(all, pre, n_pre_len);                    \
        memcpy(all + n_pre_len, ".", 1);                \
        memcpy(all + n_pre_len + 1, suf, n_suf_len);    \
        all[n_all_len - 1] = '\0';                      \
    }while(FALSE)


bIdxHash* bIdxHash_new();

bool bIdxHash_delete(bIdxHash* pIdxHash);

bool bIdxHash_insert(bIdxHash* pIdxHash, char* prefix, char* suffix, void* data);

bAllVal* bIdxHash_lookup_all(bIdxHash* pIdxHash, char* all);

bPreVal* bIdxHash_lookup_pre(bIdxHash* pIdxHash, char* prefix);



#endif
