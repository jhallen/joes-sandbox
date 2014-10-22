#ifndef _Imutex
#define _Imutex 1

#include <pthread.h>

#ifdef PTHREAD_MUTEX_INITIALIZER

#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define THREAD_COND_INITIALIZER PTHREAD_COND_INITIALIZER
typedef pthread_mutex_t mutex_t;
typedef pthread_t thread_t;
typedef pthread_cond_t thread_cond_t;
#define mutex_unlock(x) pthread_mutex_unlock(x)
#define mutex_lock(x) pthread_mutex_lock(x)
#define thread_self() pthread_self()
#define thread_equal(x,y) pthread_equal(x,y)
#define thread_cond_wait(x,y) pthread_cond_wait(x,y)
#define thread_cond_broadcast(x) pthread_cond_broadcast(x)

#else

#define MUTEX_INITIALIZER 0
#define THREAD_COND_INITIALIZER 0
typedef int mutex_t;
typedef int thread_t;
typedef int thread_cond_t;
#define mutex_unlock(x) 0
#define mutex_lock(x) 0
#define thread_self() 0
#define thread_equal(x,y) ((x)==(y))
#define thread_cond_wait(x,y) 0
#define thread_cond_broadcast(x) 0

#endif

#endif
