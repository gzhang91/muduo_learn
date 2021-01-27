#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define _GNU_SOURCE  
#include <unistd.h>
#include <sys/syscall.h>

thread_t *create_thread(int detach_stat, void *(*fn)(void *), void *arg) {
	thread_t *thr = (thread_t *)malloc(sizeof(*thr));
	if (!thr) {
		return NULL;
	}

	thr->arg = arg;
	thr->fn = fn;
	thr->pid = 0;
	thr->detach_stat = detach_stat;
	
	return thr;
}

void destroy_thread(thread_t *thr) {
	if (!thr) {
		return;
	}

	free(thr);
}

void start_thread(thread_t *thr) {
	if (!thr) {
		return;
	}
	
	int err = 0;
	pthread_attr_t attr;

	err = pthread_attr_init(&attr);
	if (err) {
		return;
	}

	if (thr->detach_stat) {
		err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if (err) {
			return;
		}
	}

	err = pthread_create(&thr->pid, &attr, thr->fn, thr->arg);
	if (err) {
		return;
	}

	(void) pthread_attr_destroy(&attr);	
}

void wait_thread(thread_t *thr) {
	int err = 0;

	if (!thr) {
		return;
	}

	if (thr->detach_stat) {
		return;
	}

	err = pthread_join(thr->pid, (void **)&thr->ret_stat);
	if (!err) {
		return;
	}

	if (err == EDEADLK) {
		printf("deadlock, exit..\n");
		exit(-1);
	}

	if (err == EINVAL) {
		printf("cannot join, or already join by other\n");
		return;
	}

	if (err == ESRCH) {
		printf("cannot search this thread by [%d]\n", (int)syscall(__NR_gettid));
		return;
	}
}

int get_status(thread_t *thr) {
	return thr->ret_stat;
}