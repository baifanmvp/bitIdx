#include <stdio.h>

#include "../src/bIdxBasicOp.h"
int main()
{
    bIdxBasOp* lp_idx_op = bIdxBasOp_new("./opt.test");
    int cnt = 0;
    while(1)
    {
        char key[1024];
        sprintf(key, "baifan%d", cnt);
        bIdxBasOp_add_array(lp_idx_op, "name", key);
        bIdxArray* lp_array = bIdxBasOp_lookup_array(lp_idx_op, "name", key);
        bIdxBasRes* lp_res = NULL;
        BIDXIDRES_INIT(lp_res, 4);
        BIDXIDRES_SETID(lp_res, 0, 1);
        BIDXIDRES_SETID(lp_res, 1, 2+BIDXBLOCK_ID_CNT);
        BIDXIDRES_SETID(lp_res, 2, 3+BIDXBLOCK_ID_CNT*15);
        BIDXIDRES_SETID(lp_res, 3, BIDXBLOCK_ID_CNT*2);
        
        bIdxBasOp_modify_atoi(lp_idx_op, lp_array,  lp_res);
        printf("key : [%s]\n", key);
        cnt++;
    }


    bIdxBasOp_delete(lp_idx_op);
    return 0;

}
