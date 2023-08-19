/* originated from here: https://github.com/qwe661234/MuThreadPackage/blob/main/Tests/test-04-priority-inversion.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "mutex.h"
#include "futex.h"

#define THREADCNT 3

mutex_t mtx1, mtx2;

void *TASK1(void *arg)
{
    mutex_lock(&mtx1);
    printf("1\n");
    mutex_unlock(&mtx1);
	return NULL;
}

void *TASK2(void *arg)
{ 
    mutex_lock(&mtx2);
    sleep(1);
    printf("2\n");
    mutex_unlock(&mtx2);
	return NULL;
}

void *TASK3(void *arg)
{   
    mutex_lock(&mtx1);
    sleep(1);
    printf("3\n");
    mutex_unlock(&mtx1);
    return NULL;
}

int main() {
    pthread_t th[THREADCNT];
    pthread_attr_t attr;

    struct sched_param param;
    mutex_init(&mtx1);
    mutex_init(&mtx2);

    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

	param.sched_priority = (THREADCNT - 2) * 10;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&th[2], NULL, TASK3, (void *)NULL);

    param.sched_priority = (THREADCNT - 1) * 10;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&th[1], NULL, TASK2, (void *)NULL);

    param.sched_priority = (THREADCNT) * 10;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&th[0], NULL, TASK1, (void *)NULL);

    void *result;
	for(int i = 0; i < THREADCNT; i++)
		pthread_join(th[i], &result);
    return 0;
}
