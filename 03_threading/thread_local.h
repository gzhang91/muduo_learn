#ifndef _THREAD_LOCAL_H_
#define _THREAD_LOCAL_H_

#include <pthread.h>

// malloc function
typedef void *(*malloc_fn)(void *arg);

// free function
typedef void *(*free_fn)(void *arg);

static malloc_fn s_malloc_handler;
static free_fn s_free_handler;

struct thread_local_s {
	pthread_key_t key;
};

typedef struct thread_local_s thread_local_t;

thread_local_t *InitThreadLocal(malloc_fn mfn, free_fn ffn);
void DestroyThreadLocal(thread_local_t *tl);
void *Value(thread_local_t *tl);

#endif