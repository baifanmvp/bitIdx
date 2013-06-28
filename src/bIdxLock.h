#ifndef BIDX_LOCK_H_
#define BIDX_LOCK_H_
#include<pthread.h>
#include"bIdxDef.h"
#define BIDX_S_LOCK 1
#define BIDX_X_LOCK 1109
typedef short int lktype_t;
typedef struct _bIdxLock
{
    pthread_mutex_t mt;
    pthread_cond_t ct;
    lktype_t ty;
    
}bIdxLock;

bIdxLock* bIdxLock_new();

bbool bIdxLock_w_lock(bIdxLock* plock);

bbool bIdxLock_s_lock(bIdxLock* plock);

bbool bIdxLock_s_unlock(bIdxLock* plock);

bbool bIdxLock_w_unlock(bIdxLock* plock);
#endif
