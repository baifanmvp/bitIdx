#ifndef BIDX_BASICOP_H

#define BIDX_BASICOP_H
#include "bIdxFile.h"
#include "bIdxArray.h"
#include "bIdxHash.h"

#define BIDX_SPACE_UNIT   (BIDXBLOCK_SIZE)

#define BIDX_EXT_SIZE     (1073741824)                 // 1G

#define BIDX_EXT_UNIT_CNT (BIDX_EXT_SIZE/BIDX_SPACE_UNIT)  // 8096

typedef struct st_bIdxfreeNode
{
    bIdxBlock* node;
    struct st_bIdxfreeNode* next;
}bIdxfreeNode;


typedef struct st_bIdxBasOp
{
    bIdxFile* fp;
    bIdxHash* haIdx;
    bIdxfreeNode* free;
    
}bIdxBasOp;


bIdxBasOp* bIdxBasOp_new(char* path);


bbool bIdxBasOp_delete(bIdxBasOp* pbIdxBasOp);

bIdxBlock* bIdxBasOp_get_free_idxBlock(bIdxBasOp* pbIdxBasOp);


bbool bIdxBasOp_reload(bIdxBasOp* pbIdxBasOp);


bbool bIdxBasOp_load(bIdxBasOp* pbIdxBasOp);

bbool bIdxBasOp_ext(bIdxBasOp* pbIdxBasOp);

//array to array
bIdxArray* bIdxBasOp_searchOr_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr);


bIdxArray* bIdxBasOp_searchAnd_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr);


bIdxArray* bIdxBasOp_modify_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxArray* pSrcArr);

//if pSrcArr is NULL ,then remove all block of pDestArr
bIdxArray* bIdxBasOp_remove_atoa(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxArray* pSrcArr);

/////////////////////////////////////////////



//array to idResult

bIdxArray* bIdxBasOp_searchOr_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxArray* pSrcArr);

bIdxBasRes* bIdxBasOp_searchAnd_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxBasRes* pBasRes);


bbool bIdxBasOp_modify_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr,  bIdxBasRes* pBasRes);

//if pSrcArr is NULL ,then remove all block of pDestArr

bbool bIdxBasOp_remove_atoi(bIdxBasOp* pbIdxBasOp, bIdxArray* pDestArr, bIdxBasRes* pBasRes);


bIdxArray* bIdxBasOp_lookup_array(bIdxBasOp* pbIdxBasOp, char* prefix, char* suffix);


bbool bIdxBasOp_add_array(bIdxBasOp* pbIdxBasOp, char* prefix, char* suffix);
bbool bIdxBasOp_rm_array(bIdxBasOp* pbIdxBasOp, char* prefix, char* suffix);


#endif

