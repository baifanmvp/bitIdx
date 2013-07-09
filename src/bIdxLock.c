#include "bIdxLock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

bIdxLock* bIdxLock_new()
{
    bIdxLock* lp_lock = (bIdxLock*) malloc(sizeof(bIdxLock));
    pthread_mutex_init(&lp_lock->mt, NULL);
    pthread_cond_init(&lp_lock->ct, NULL);
    lp_lock->ty = BIDX_X_LOCK;
    return lp_lock;
}


bbool bIdxLock_delete(bIdxLock* plock)
{
    pthread_mutex_destroy(&plock->mt);
    pthread_cond_destroy(&plock->ct);
    free(plock);
    return TRUE;
}

bbool bIdxLock_x_lock(bIdxLock* plock)
{

    pthread_mutex_lock(&plock->mt);
    
wlock_start:
    
    if (plock->ty == BIDX_X_LOCK)
    {
        printf("X--- no lock, OK !![%lu]\n", pthread_self());

        plock->ty -= BIDX_X_LOCK;
        pthread_mutex_unlock(&plock->mt);

        return  TRUE;
    }
    if(0 < plock->ty && plock->ty < BIDX_X_LOCK)
    {
        plock->ty -= BIDX_X_LOCK;
        while(plock->ty < 0)
        {
            printf("X--- s lock, wait !! [%lu]\n", pthread_self());
            pthread_cond_wait(&plock->ct, &plock->mt);
            printf("X--- s unlock, OK !! [%lu] \n", pthread_self());
        }
        pthread_mutex_unlock(&plock->mt);
        return TRUE;
    }
    
    while(plock->ty <= 0)
    {
        printf("X--- x lock, wait !! [%lu]\n",pthread_self());
        pthread_cond_wait(&plock->ct, &plock->mt);
    }
    
    goto  wlock_start;
    

}


bbool bIdxLock_s_lock(bIdxLock* plock)
{

    pthread_mutex_lock(&plock->mt);
    
slock_start:

    if(1 < plock->ty && plock->ty <= BIDX_X_LOCK)
    {
        printf("S--- s lock, OK !![%lu]\n", pthread_self());
        plock->ty -= BIDX_S_LOCK;
        pthread_mutex_unlock(&plock->mt);

        return  TRUE;
    }

    while(plock->ty <= 1)
    {
        printf("S--- x lock, wait !! [%lu]\n", pthread_self());
        pthread_cond_wait(&plock->ct, &plock->mt);
    }
    
    goto  slock_start;
 
}



bbool bIdxLock_s_unlock(bIdxLock* plock)
{

    pthread_mutex_lock(&plock->mt);


    plock->ty += BIDX_S_LOCK;
    
    pthread_cond_broadcast(&plock->ct);
    
    pthread_mutex_unlock(&plock->mt);
    return  TRUE;

}

bbool bIdxLock_x_unlock(bIdxLock* plock)
{

    pthread_mutex_lock(&plock->mt);


    plock->ty += BIDX_X_LOCK;
    
    pthread_cond_broadcast(&plock->ct);
    
    pthread_mutex_unlock(&plock->mt);
    return  TRUE;

}
