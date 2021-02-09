#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

/*
	区分线程对象和线程
*/

struct thread_s {
	pthread_t pid;
	int detach_stat;
	int ret_stat;

	void *(*fn)();
	void *arg;
};

typedef struct thread_s thread_t;

thread_t *create_thread(int detach_stat, void *(*fn)(void *), void *arg);
void destroy_thread(thread_t *thr);

void start_thread(thread_t *thr);
void wait_thread(thread_t *thr);
int get_status(thread_t *thr);

#endif // _THREAD_H_