#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "thread_local.h"

typedef int count;

thread_local_t *g_tl;

// malloc function
void *malloc_count(void *arg) {
	count *init_val = (count *)arg;

	count *cnt = (count *)malloc(sizeof(*cnt));
	if (!cnt) {
		return NULL;
	}

	if (arg) {
		*cnt = *init_val;
	}

	return cnt;
}

// free function
void *free_count(void *arg) {
	free(arg);
}

void* WorkerFunc(void *arg) {

	while (1) {
		int *val = (int *)Value(g_tl);
		printf("&g_tl[%p] [%d] [addr: %p]count: %d\n", g_tl, (pid_t)pthread_self(), val, *val);
		if (*val >= 10) {
			break;
		}
		// *val++ ?
		//++*val;
		(*val)++;
		sleep(1);
	}

	return NULL;
}

int main(int argc, char **argv) {
	pthread_t worker_th1, worker_th2;

	g_tl = InitThreadLocal(malloc_count, free_count);
	if (!g_tl) {
		printf("[%d] init thread local failed!\n", (pid_t)pthread_self());
		return -1;
	}

	pthread_create(&worker_th1, NULL, WorkerFunc, NULL);
	pthread_create(&worker_th2, NULL, WorkerFunc, NULL);

	pthread_join(worker_th1, NULL);
	pthread_join(worker_th2, NULL);
	DestroyThreadLocal(g_tl);

	return 0;
}