#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>
#include "thread.h"

struct thread_pool_s {
	pthread_mutex_t *mutex;
	pthread_cond_t *cond;

	size_t num_of_threads;
	thread_t *thr_base;

};

typedef struct thread_pool_s thread_pool_t;

#endif // _THREAD_POOL_H_