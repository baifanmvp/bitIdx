#ifndef BIDXER_H_
#define BIDXER_H_
#include "bIdxLock.h"
#include "bIdxBasicOp.h"
#include "lib/cjson.h"
#include "eqlClient.h"

typedef struct st_bIdxer
{
    bIdxBasOp* pBasOp;
    bIdxLock* pLock;
    char* eqlIp;
    unsigned short eqlPort;
}bIdxer;

bIdxer* bIdxer_new(char* bitPath, char* eqlIp, unsigned short eqlPort);

char* bIdxer_query(bIdxer* pbIdxer, char* query);

char* bIdxer_query_addtag(bIdxer* pbIdxer, cjson* jsoner);

char* bIdxer_query_rmtag(bIdxer* pbIdxer, cjson* jsoner);

char* bIdxer_query_qryid(bIdxer* pbIdxer, cjson* jsoner);

char* bIdxer_query_utagid(bIdxer* pbIdxer, cjson* jsoner);

char* bIdxer_query_utagcnd(bIdxer* pbIdxer, cjson* jsoner);

char* bIdxer_query_qrycnd(bIdxer* pbIdxer, cjson* jsoner);

bbool bIdxer_delete(bIdxer* pbIdxer);


#endif
