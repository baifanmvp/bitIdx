#ifndef BIDX_THRPOOL_H_
#define BIDX_THRPOOL_H_
#include "bIdxDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
typedef  void thrOutRes;
typedef  void thrInRes;
typedef void *(*bIdxFnWorker) (void *);
#define BIDXTHR_BUSY 0
#define BIDXTHR_IDLE 1
typedef struct _bIdxThrData
{
    pthread_mutex_t mt;
    pthread_cond_t ct;
    void* inRes;
    bIdxFnWorker running;
    pthread_t id;
    bflag_t flag;
}bIdxThrData;

typedef struct _bIdxThrPool
{
    bIdxThrData* thrData;
    size_t thrCnt;
}bIdxThrPool;

void* bIdxThrPool_worker(void* arg);


bIdxThrPool* bIdxThrPool_init(size_t thrCnt);

bbool bIdxThrPool_working(bIdxThrPool* pThrPool, void* inRes, bIdxFnWorker running);

#endif
