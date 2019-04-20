/* XMRig
* A simple tool for control cpu cost between busy and idle time.
* Build commond: gcc cpu.c -lpthread -lrt -o testcpu
 */



#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>  
#include <string.h>  
#include <stdio.h>  
#include <errno.h> 
#define __USE_GNU       // ==>For fixing CPU_ZERO,CPU_SET build error
#include <sched.h>      // ==>This header must be placed before <pthread.h>
#include <pthread.h>


typedef long long int int64;

const int NUM_THREADS = 8;      // total count of cpu core
const int cpu_busy_time = 30;   // busy time in sec
const int cpu_idle_time = 30;   // idle time in sec

enum
{
    CPU_BUSY_TIME=0,
    CPU_IDLE_TIME,
    CPU_MAX_TIME
};

int64 GetTickCountInMs(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64 sec = now.tv_sec;
    int64 nsec = now.tv_nsec;
    return sec*1000 + nsec/1000000;
}

static int set_cpu(int i)  
{  
    cpu_set_t mask;  
    CPU_ZERO(&mask);  
    CPU_SET(i,&mask);  
  
    //printf("thread id=%lu, i= %d\n", pthread_self(), i);  
    if(-1 == pthread_setaffinity_np(pthread_self() ,sizeof(mask),&mask))  
    {  
        return -1;  
    }  
    return 0;  
} 

void* cpu_cost(void *args)
{
    const int64 time_info[CPU_MAX_TIME] = {cpu_busy_time*1000,cpu_idle_time*1000};
    int64 startTime = 0;
    int cpu_core_num = *((int *)args);

    //printf("cpu id =%d\n", cpu_core_num);

    set_cpu(cpu_core_num);
    while (1)
    {
        startTime = GetTickCountInMs();
        while((GetTickCountInMs() - startTime) <= time_info[CPU_BUSY_TIME])
        {}

        usleep(time_info[CPU_IDLE_TIME]*1000);
    }
}

int main(int argc, char **argv)
{
    pthread_t t[NUM_THREADS];
    int core_index[NUM_THREADS];
    printf("cpu number=%d, busy time=%dSec, idle time=%dSec\n", NUM_THREADS, cpu_busy_time, cpu_idle_time);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        core_index[i] = i;
        int ret = pthread_create(&t[i], NULL, cpu_cost, &core_index[i]);
        if(ret)
        {
            printf("[error]:thread create error!\n");
        }
    }

    pthread_exit(NULL);

    return 0;
}