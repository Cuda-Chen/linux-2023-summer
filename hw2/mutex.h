#pragma once

#if USE_PTHREADS

#include <pthread.h>

#define mutex_t pthread_mutex_t
#define mutex_init(m) pthread_mutex_init(m, NULL)
#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define mutex_trylock(m) (!pthread_mutex_trylock(m))
#define mutex_lock pthread_mutex_lock
#define mutex_unlock pthread_mutex_unlock

#else

#include <stdbool.h>
#include "atomic.h"
#include "futex.h"
#include "spinlock.h"

typedef struct Mutex mutex_t;
struct Mutex {
    atomic int state;
    bool (*trylock)(mutex_t *);
    void (*lock)(mutex_t *);
    void (*unlock)(mutex_t *);
};

typedef struct {
    int protocol;
} mutexattr_t;

enum {
    MUTEX_LOCKED = 1 << 0,
    MUTEX_SLEEPING = 1 << 1,
};

enum {
    PRIO_NONE = 0,
    PRIO_INHERIT,
};

#define MUTEX_SPINS 128

#define MUTEX_INITIALIZER         \
    {                             \
        .state = 0, .protocol = 0 \
    }

#define cmpxchg(obj, expected, desired) \
    compare_exchange_strong(obj, expected, desired, relaxed, relaxed)

#define MUTEX_SPINNING                          \
    do {                                        \
        for (int i = 0; i < MUTEX_SPINS; ++i) { \
            if (mutex->trylock(mutex))          \
                return;                         \
            spin_hint();                        \
        }                                       \
    } while (0)

#define GETTID syscall(SYS_gettid)

static inline bool mutex_trylock_default(mutex_t *mutex)
{
    int state = load(&mutex->state, relaxed);
    if (state & MUTEX_LOCKED)
        return false;

    state = fetch_or(&mutex->state, MUTEX_LOCKED, relaxed);
    if (state & MUTEX_LOCKED)
        return false;

    thread_fence(&mutex->state, acquire);
    return true;
}

static inline bool mutex_trylock_pi(mutex_t *mutex)
{
    pid_t zero = 0;
    pid_t tid = GETTID;

    if (cmpxchg(&mutex->state, &zero, tid))
        return true;

    thread_fence(&mutex->state, acquire);
    return false;
}

static inline void mutex_lock_default(mutex_t *mutex)
{
    MUTEX_SPINNING;

    int state = exchange(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING, relaxed);

    while (state & MUTEX_LOCKED) {
        futex_wait(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING);
        state = exchange(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING, relaxed);
    }

    thread_fence(&mutex->state, acquire);
}

static inline void mutex_lock_pi(mutex_t *mutex)
{
    MUTEX_SPINNING; 

    futex_lock_pi(&mutex->state, NULL);
    thread_fence(&mutex->state, acquire);
}

static inline void mutex_unlock_default(mutex_t *mutex)
{
    int state = exchange(&mutex->state, 0, release);
    if (state & MUTEX_SLEEPING)
        futex_wake(&mutex->state, 1);  // FFFF
}

static inline void mutex_unlock_pi(mutex_t *mutex) {
    pid_t tid = GETTID;
    if(cmpxchg(&mutex->state, &tid, 0))
        return;
    futex_unlock_pi(&mutex->state);
}

static inline void mutex_init(mutex_t *mutex, mutexattr_t *attr)
{
    atomic_init(&mutex->state, 0);

    mutex->trylock = mutex_trylock_default;
    mutex->lock = mutex_lock_default;
    mutex->unlock = mutex_unlock_default;
    if (attr) {
        switch (attr->protocol) {
        case PRIO_INHERIT:
            mutex->trylock = mutex_trylock_pi;
            mutex->lock = mutex_lock_pi;
            mutex->unlock = mutex_unlock_pi;
            break;
        default:
            break;
        }
    }
}

static inline bool mutex_trylock(mutex_t *mutex)
{
    return mutex->trylock(mutex);
}

static inline void mutex_lock(mutex_t *mutex)
{
    mutex->lock(mutex);
}

static inline void mutex_unlock(mutex_t *mutex)
{
    mutex->unlock(mutex);
}

static inline void mutexattr_setprotocol(mutexattr_t *attr, int protocol)
{
    attr->protocol = protocol;
}

#endif
