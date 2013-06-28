#include "bIdxThrPool.h"

void* bIdxThrPool_worker(void* arg)
{
    bIdxThrData* pThrData = arg;
    if(!pThrData || pThrData->id != pthread_self() )
    {
        return NULL;
    }
    while(1)
    {
        pthread_cond_wait(&pThrData->ct, &pThrData->mt);

        pThrData->running(pThrData->inRes);
        
        pThrData->flag = BIDXTHR_IDLE;
        
        pthread_mutex_unlock(&pThrData->mt);

    }
    return NULL;
}


bIdxThrPool* bIdxThrPool_init(size_t thrCnt)
{
    bIdxThrPool* lp_pool = (bIdxThrPool*) malloc(sizeof(bIdxThrPool));
    lp_pool->thrCnt = thrCnt;
    lp_pool->thrData = (bIdxThrData*)malloc(sizeof(bIdxThrData));

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
        
        if(pthread_mutex_trylock( &(pThrPool->thrData[n_idx].mt) ) == 0)
        {
            if(pThrPool->thrData[n_idx].flag = BIDXTHR_IDLE)
            {
                pThrPool->thrData[n_idx].flag = BIDXTHR_BUSY;
                break;
            }
            else
            {
                pthread_mutex_unlock( &(pThrPool->thrData[n_idx].mt));
            }
        }
            
        if(++n_idx == pThrPool->thrCnt)
        {
            n_idx = 0;
            usleep(5000);
        }
    }
    pThrPool->thrData[n_idx].inRes = inRes;
    pThrPool->thrData[n_idx].running = running;
    pthread_cond_signal(&pThrPool->thrData[n_idx].ct);
    pthread_mutex_unlock( &(pThrPool->thrData[n_idx].mt));
    return TRUE;
    
}
