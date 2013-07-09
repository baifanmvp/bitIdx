#include "bIdxThrPool.h"
#include <unistd.h>

void* bIdxThrPool_worker(void* arg)
{
    bIdxThrData* pThrData = arg;
    if(!pThrData || pThrData->id != pthread_self() )
    {
        return NULL;
    }
    pThrData->flag = BIDXTHR_IDLE;

    while(1)
    {
        pthread_cond_wait(&pThrData->ct, &pThrData->mt);
       
        pThrData->running(pThrData->inRes);
        
        pThrData->flag = BIDXTHR_IDLE;
        
    }
    return NULL;
}


bIdxThrPool* bIdxThrPool_init(size_t thrCnt)
{
    bIdxThrPool* lp_pool = (bIdxThrPool*) malloc(sizeof(bIdxThrPool));
    lp_pool->thrCnt = thrCnt;
    lp_pool->thrData = (bIdxThrData*)malloc(sizeof(bIdxThrData) * lp_pool->thrCnt);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    while(thrCnt--)
    {
        lp_pool->thrData[thrCnt].inRes = NULL;
        pthread_mutex_init(&lp_pool->thrData[thrCnt].mt, NULL);
        pthread_cond_init(&lp_pool->thrData[thrCnt].ct, NULL);
        
        pthread_create(&lp_pool->thrData[thrCnt].id, &attr,
                       bIdxThrPool_worker, lp_pool->thrData + thrCnt);
    }
    
    pthread_attr_destroy(&attr);

    return lp_pool;
    
}

bbool bIdxThrPool_working(bIdxThrPool* pThrPool, void* inRes, bIdxFnWorker running)
{
    if(!pThrPool || !inRes)
    {
        return FALSE;
    }
    bindex_t n_idx = 0;
    while(1)
    {
        //避免lock多次调用 提升性能
        if(pThrPool->thrData[n_idx].flag == BIDXTHR_IDLE)
        {
            if(pthread_mutex_trylock( &(pThrPool->thrData[n_idx].mt) ) == 0)
            {
                if(pThrPool->thrData[n_idx].flag == BIDXTHR_BUSY)
                {
                    pthread_mutex_unlock( &(pThrPool->thrData[n_idx].mt));
                }
                else
                {
                    pThrPool->thrData[n_idx].inRes = inRes;
                    pThrPool->thrData[n_idx].running = running;
                    
                    pthread_cond_signal(&pThrPool->thrData[n_idx].ct);
                    printf("    **************************  bIdxServer_processing_sk  ************************[%lu]*** \n", pThrPool->thrData[n_idx].id);
                    
                    pThrPool->thrData[n_idx].flag = BIDXTHR_BUSY;
                    
                    pthread_mutex_unlock( &(pThrPool->thrData[n_idx].mt));
                    return TRUE;
                }
            }
        }   
        if(++n_idx == pThrPool->thrCnt)
        {
            n_idx = 0;
            usleep(5000);
        }
    }
    
}
