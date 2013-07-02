#include <stdio.h>

#include "../src/bIdxBasicOp.h"
int main()
{
    unlink("./opt.test");
    int n_all_cnt = 300;
    bIdxBasOp* lp_idx_op = bIdxBasOp_new("./opt.test");
    int cnt = 0;
    bIdxBasRes* lp_res = NULL;
    BIDXIDRES_INIT(lp_res, 4);
    
    BIDXIDRES_SETID(lp_res, 0, 1);
    BIDXIDRES_SETID(lp_res, 1, 2+BIDXBLOCK_ID_CNT);
    BIDXIDRES_SETID(lp_res, 2, 3+BIDXBLOCK_ID_CNT*15);
    BIDXIDRES_SETID(lp_res, 3, BIDXBLOCK_ID_CNT*2);

    while(1)
    {
        char key[1024];
        sprintf(key, "baifan%d", cnt);
        bIdxBasOp_add_array(lp_idx_op, "name", key);
        bIdxArray* lp_array = bIdxBasOp_lookup_array(lp_idx_op, "name", key);
	bIdxBasOp_modify_atoi(lp_idx_op, lp_array,  lp_res);
	//        bIdxBasOp_remove_atoi(lp_idx_op, lp_array,  lp_res);
        printf("key : [%s]\n", key);
        cnt++;
	if(cnt == n_all_cnt)break;
    }

    BIDXIDRES_DESTORY(lp_res);
    while(1)
      {
	struct timeval tstart ,tend;
	gettimeofday(&tstart, 0);
	cnt = 0;
    char key1[1024];
    char key2[1024];
	while(1)
	  {
          sprintf(key1, "baifan%d", rand()%n_all_cnt);
          sprintf(key2, "baifan%d", rand()%n_all_cnt);
          bIdxArray* lp_array_1 = bIdxBasOp_lookup_array(lp_idx_op, "name", key1);
          
          bIdxArray* lp_array_2 = bIdxBasOp_lookup_array(lp_idx_op, "name", key2);

        
	    printf("----------key1 : [%s]------------- \n", key1);
	    printf("----------key2 : [%s]------------- \n", key2);

        bIdxBasOp_remove_atoa(lp_idx_op, lp_array_1, lp_array_2);
	    cnt++;
        printf("----------end------------- \n\n\n", key1);

	    if(cnt == n_all_cnt)break;
	  }
	gettimeofday(&tend, 0);
	printf("time : %f\n", (double)(tend.tv_usec - tstart.tv_usec)/1000000  + (double)(tend.tv_sec - tstart.tv_sec) ) ;
	getchar();
      }

    bIdxBasOp_delete(lp_idx_op);
    return 0;

}



int main_atoi()
{
    int n_all_cnt = 300;
    bIdxBasOp* lp_idx_op = bIdxBasOp_new("./opt.test");
    int cnt = 0;
    bIdxBasRes* lp_res = NULL;
    BIDXIDRES_INIT(lp_res, 4);
    
    BIDXIDRES_SETID(lp_res, 0, 1);
    BIDXIDRES_SETID(lp_res, 1, 2+BIDXBLOCK_ID_CNT);
    BIDXIDRES_SETID(lp_res, 2, 3+BIDXBLOCK_ID_CNT*15);
    BIDXIDRES_SETID(lp_res, 3, BIDXBLOCK_ID_CNT*2);

    while(1)
    {
        char key[1024];
        sprintf(key, "baifan%d", cnt);
        bIdxBasOp_add_array(lp_idx_op, "name", key);
        bIdxArray* lp_array = bIdxBasOp_lookup_array(lp_idx_op, "name", key);
	bIdxBasOp_modify_atoi(lp_idx_op, lp_array,  lp_res);
	//        bIdxBasOp_remove_atoi(lp_idx_op, lp_array,  lp_res);
        printf("key : [%s]\n", key);
        cnt++;
	if(cnt == n_all_cnt)break;
    }

    BIDXIDRES_DESTORY(lp_res);
    while(1)
      {
	struct timeval tstart ,tend;
	gettimeofday(&tstart, 0);
	cnt = 0;
	while(1)
	  {
	    char key[1024];
	    sprintf(key, "baifan%d", cnt);
	    bIdxArray* lp_array = bIdxBasOp_lookup_array(lp_idx_op, "name", key);
	    
	    bIdxBasRes* lp_res = bIdxArray_get_idResult(lp_array, 0, 10);

	    printf("----------key : [%s]------------- \n", key);
	    int i = 0;
	     while(i < BIDXIDRES_CNT(lp_res))
	      {
		printf("%d\n", BIDXIDRES_GETID(lp_res, i));
		i++;
	      }
	    BIDXIDRES_DESTORY(lp_res);
	    
	    cnt++;
	    if(cnt == n_all_cnt)break;
	  }
	gettimeofday(&tend, 0);
	printf("time : %f\n", (double)(tend.tv_usec - tstart.tv_usec)/1000000  + (double)(tend.tv_sec - tstart.tv_sec) ) ;
	getchar();
      }

    bIdxBasOp_delete(lp_idx_op);
    return 0;

}
