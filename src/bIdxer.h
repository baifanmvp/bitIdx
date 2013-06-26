#ifndef BIDXER_H_
#define BIDXER_H_
#include "bIdxBasicOp.h"
#include "lib/cjson.h"
#include "eqlClient.h"

typedef struct st_bIdxer
{
    cjson* jsoner;
    bIdxBasOp* pBasOp;
    eqlClient* peql;
}bIdxer;

bIdxer* bIdxer_new(char* bitPath, char* ip, int port);

char* bIdxer_query(bIdxer* pbIdxer, char* query);

char* bIdxer_query_addtag(bIdxer* pbIdxer);

char* bIdxer_query_rmtag(bIdxer* pbIdxer);

char* bIdxer_query_qryid(bIdxer* pbIdxer);

char* bIdxer_query_utagid(bIdxer* pbIdxer);

char* bIdxer_query_utagcnd(bIdxer* pbIdxer);

char* bIdxer_query_qrycnd(bIdxer* pbIdxer);

bbool bIdxer_delete(bIdxer* pbIdxer);


#endif
