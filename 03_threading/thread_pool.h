#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <pthread.h>
#include "thread.h"
#include <sys/sysinfo.h>
#include <stdlib.h>

#define inline 

typedef void *(*task_func)(void *);

struct task_s {
	// fixed，使用更加精细的时间计数器
	time_t post_time;
	time_t finish_time;
	pid_t from_thrid;
	pid_t deal_thrid;

	task_func fn;
	void *data;
};

typedef struct task_s task_t;

static inline void print_task_info(task_t *task) {
	printf("###### task info ######\n");
	printf("post from [%d], post to [%d]\n", task->from_thrid, task->deal_thrid);
	printf("start time [%ld], finish time [%ld], latency is [%ld]\n", task->post_time, task->finish_time, task->finish_time - task->post_time);
	printf("###### end ###### \n\n");
}

struct task_queue_s {
	task_t *data;

	struct task_queue_s *prev;
	struct task_queue_s *next;
};

typedef struct task_queue_s task_queue_t;

struct thread_pool_s {
	pthread_mutex_t *task_mutex;
	pthread_cond_t *task_cond;

	size_t num_of_threads;
	thread_t **thr_base;
	task_queue_t task_queue;
	int runnable;
	int pos_index;
};

typedef struct thread_pool_s thread_pool_t;

// task function
// empty 
static inline int queue_empty(task_queue_t *queue) {
	if (!queue || queue->next == queue) {
		return 1;
	}

	return 0;
}

// init
static inline void queue_init(task_queue_t *queue) {
	if (!queue) {
		return;
	}

	queue->next = queue;
	queue->prev = queue;
	// 第一个data为nul
	queue->data = NULL;
}

// push
static inline int queue_push(task_queue_t *queue, task_t *t) {
	if (!queue) {
		return -1;
	}

	task_queue_t *q = (task_queue_t *) malloc(sizeof(*q));
	if (!q) {
		goto failed;
	}

	/*q->data = (task_t *)malloc(sizeof(*(q->data)));
	if (!q->data) {
		goto failed;
	}*/
	q->data = t;
	q->prev = queue->next;
	q->next = queue;
	queue->next->next = q;
	queue->prev = q;
	
	return 0;

failed:

	if (q) {
		free(q);
	}

	return -1;
}

// pop
static inline task_queue_t *queue_pop(task_queue_t *queue) {
	if (!queue) {
		return NULL;
	}

	if (queue_empty(queue)) {
		return NULL;
	}

	task_queue_t *q = queue->next;
	q->prev->next = q->next;
	q->next->prev = q->prev;

	return q;
}

// get function
static inline task_queue_t *queue_head(task_queue_t *queue) {
	if (!queue) {
		return NULL;
	}

	if (queue_empty(queue)) {
		return NULL;
	}

	return queue->next;
}

static inline task_queue_t *queue_tail(task_queue_t *queue) {
	if (!queue) {
		return NULL;
	}

	if (queue_empty(queue)) {
		return NULL;
	}

	return queue->prev;
}

static inline task_queue_t *queue_next(task_queue_t *queue, task_queue_t *p) {
	if (!queue) {
		return NULL;
	}

	if (queue_empty(queue)) {
		return NULL;
	}

	return p->next;
}

// destroy
static void queue_destroy(task_queue_t *queue) {
	task_queue_t *cur = queue_head(queue);
	task_queue_t *q = NULL;

	for (; cur != queue_tail(queue); ) {
		q = cur;
		cur = queue_next(queue, cur);

		if (q->data) {
			free(q->data);
		}

		free(q);
		q = NULL;
	}

	if (cur->data) {
		free(cur->data);
	}
	free(cur);

	// free(queue);
}


// ========================= pool function
void *thread_pool_func(void *arg);
thread_pool_t *thread_pool_init(int num_of_threads);
void thread_pool_destroy(thread_pool_t *thread_pool);

void thread_pool_start(thread_pool_t *thread_pool);
void thread_pool_stop(thread_pool_t *thread_pool);
int thread_pool_post(thread_pool_t *thread_pool, task_t *t);

#undef inline

#endif // _THREAD_POOL_H_