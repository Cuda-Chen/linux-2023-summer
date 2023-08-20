/* originated from here: https://github.com/qwe661234/MuThreadPackage/blob/main/Tests/test-04-priority-inversion.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "mutex.h"
#include "atomic.h"

#define THREADCNT 3

atomic bool thread_stop = false;
mutex_t mtx1, mtx2;

void *task1(void *arg)
{
    mutex_lock(&mtx1);
    printf("1\n");
    mutex_unlock(&mtx1);
	return NULL;
}

void *task2(void *arg)
{ 
    mutex_lock(&mtx2);
    sleep(1);
    printf("2\n");
    mutex_unlock(&mtx2);
	return NULL;
}

void *task3(void *arg)
{   
    mutex_lock(&mtx1);
    sleep(1);
    printf("3\n");
    mutex_unlock(&mtx1);
    return NULL;
}

int main() {
    pthread_t th[THREADCNT];

    mutexattr_t mattr;
    mutexattr_setprotocol(&mattr, PRIO_INHERIT);
    mutex_init(&mtx1, &mattr);
    mutex_init(&mtx2, &mattr);

    pthread_attr_t attr;
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    struct sched_param param;

	param.sched_priority = (THREADCNT - 2) * 10;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&th[2], NULL, task3, (void *)NULL);

    param.sched_priority = (THREADCNT - 1) * 10;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&th[1], NULL, task2, (void *)NULL);

    param.sched_priority = (THREADCNT) * 10;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&th[0], NULL, task1, (void *)NULL);

	for(int i = THREADCNT - 1; i >= 0; i--)
		pthread_join(th[i], NULL);
    sleep(2);
    return 0;
}
