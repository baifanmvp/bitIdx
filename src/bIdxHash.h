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



bIdxHash* bIdxHash_new();

bbool bIdxHash_delete(bIdxHash* pIdxHash);

bbool bIdxHash_insert(bIdxHash* pIdxHash, char* prefix, char* suffix, bIdxArray* data);

bAllVal* bIdxHash_lookup_all(bIdxHash* pIdxHash, char* all);

bPreVal* bIdxHash_lookup_pre(bIdxHash* pIdxHash, char* prefix);



#endif
