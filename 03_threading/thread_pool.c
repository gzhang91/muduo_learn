#include "thread_pool.h"
#include <stddef.h>

//////////////////// pool function
void *thread_pool_func(void *arg) {
	thread_pool_t *thread_pool = (thread_pool_t *)arg;
	if (!thread_pool) {
		return NULL;
	}

	task_t *task;
	task_queue_t *q;

	if (!thread_pool) {
		return NULL;
	}

	while (thread_pool->runnable) {
		pthread_mutex_lock(thread_pool->task_mutex);

		while (queue_empty(&thread_pool->task_queue)) {
			pthread_cond_wait(thread_pool->task_cond, thread_pool->task_mutex);
		}

		task_queue_t *q = queue_pop(&thread_pool->task_queue);
		pthread_mutex_unlock(thread_pool->task_mutex);

		(void)q->data->fn(q->data->data);
		free(q->data);
		free(q);
	}
}

// init
thread_pool_t *thread_pool_init(int num_of_threads) {
	int i = 0;

	thread_pool_t *thread_pool = (thread_pool_t *)malloc(sizeof(*thread_pool));
	if (!thread_pool) {
		return NULL;
	}

	thread_pool->task_mutex = (pthread_mutex_t *)malloc(sizeof(*(thread_pool->task_mutex)));
	if (!thread_pool->task_mutex) {
		goto failed;
	}
	(void)pthread_mutex_init(thread_pool->task_mutex, NULL);

	thread_pool->task_cond = (pthread_cond_t *)malloc(sizeof(*(thread_pool->task_cond)));
	if (!thread_pool->task_cond) {
		goto failed;
	}
	(void)pthread_cond_init(thread_pool->task_cond, NULL);

	if (num_of_threads <= 0) {
		num_of_threads = get_nprocs();
	}

	thread_pool->num_of_threads = num_of_threads;
	thread_pool->thr_base = (thread_t **)malloc(num_of_threads * sizeof(thread_t *));
	if (!thread_pool->thr_base) {
		goto failed;
	}

	for (; i < num_of_threads; ++i) {
		thread_pool->thr_base[i] = create_thread(1, thread_pool_func, (void *)thread_pool);
		if (!thread_pool->thr_base[i]) {
			goto failed;
		}
	}

	thread_pool->runnable = 1;
	thread_pool->pos_index = -1;

	queue_init(&thread_pool->task_queue);

	return thread_pool;

failed:
	if (thread_pool->task_mutex) {
		free(thread_pool->task_mutex);
	}

	if (thread_pool->task_cond) {
		free(thread_pool->task_cond);
	}

	return NULL;
}

void thread_pool_destroy(thread_pool_t *thread_pool) {
	int i = 0;

	if (!thread_pool) {
		return;
	}

	thread_pool->runnable = 0;
	pthread_cond_broadcast(thread_pool->task_cond);

	pthread_mutex_destroy(thread_pool->task_mutex);
	pthread_cond_destroy(thread_pool->task_cond);
	
	for (; i < thread_pool->num_of_threads; ++i) {
		destroy_thread(thread_pool->thr_base[i]);
	}

	free(thread_pool->thr_base);
	queue_destroy(&thread_pool->task_queue);
	free(thread_pool);
}

void thread_pool_start(thread_pool_t *thread_pool) {
	int i = 0;
	for (; i < thread_pool->num_of_threads; ++i) {
		start_thread(thread_pool->thr_base[i]);
	}
}

void thread_pool_stop(thread_pool_t *thread_pool) {
	int i = 0;
	for (; i < thread_pool->num_of_threads; ++i) {
		wait_thread(thread_pool->thr_base[i]);
	}
}

int thread_pool_post(thread_pool_t *thread_pool, task_t *t) {
	int ret = 0;

	if (!thread_pool || !t) {
		return -1;
	}

	pthread_mutex_lock(thread_pool->task_mutex);
	ret = queue_push(&thread_pool->task_queue, t);
	pthread_cond_signal(thread_pool->task_cond);
	pthread_mutex_unlock(thread_pool->task_mutex);

	return ret;
}

