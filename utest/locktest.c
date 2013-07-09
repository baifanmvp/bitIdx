#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "../src/bIdxLock.h"
#include <stdlib.h>
  #include <unistd.h>


#define _THREAD_CNT 8
bIdxLock* g_lock = NULL;

void* thr_run(void* arg)
{
    int id = *(int*)arg;
    int count = 0;
    sleep(1);
    printf("id : %d start !\n", id);
    while(1)
    {
        useconds_t usec = (1+rand()%10)*1000*50;
    int rmd = rand()%4;
        if(rmd == 3)
        {
            bIdxLock_x_lock(g_lock);
            usleep(usec);
            bIdxLock_x_unlock(g_lock);
            printf("X :id : %lu, cnt : %u \n", pthread_self(), count++);
        }
        else
        {
            bIdxLock_s_lock(g_lock);
            usleep(usec/4);
            bIdxLock_s_unlock(g_lock);
            printf("S :id : %lu, cnt : %u\n", pthread_self(), count++);

        }
    }
}
int g_id[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
int main()
{
    srand(time(0));
    
    g_lock = bIdxLock_new();
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int i = 0;
    while(i < _THREAD_CNT)
    {
        pthread_t tid = 0;
        pthread_create(&tid, &attr, thr_run, g_id+i);

        i++;
    }

    sleep(10000);
    
    return 0;
}
