#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/syscall.h>
#include "thread.h"
#include "thread_pool.h"

void *task_handler(void *arg) {
	task_t *task = (task_t *)arg;

	time_t local_time = time(NULL);
	struct tm tm_val;
	char time_buf[32] = {0};

	(void)localtime_r(&local_time, &tm_val);
	(void)asctime_r(&tm_val, time_buf);

	printf("[do_task] now: %s\n", time_buf);
	
	task->deal_thrid = (pid_t)syscall(__NR_gettid);
	task->finish_time = time(NULL);
	print_task_info(task);
}

void *producer(void *arg) {
	srand(time(NULL));

	thread_pool_t *thread_pool = (thread_pool_t *)arg;

	int pos_cnt = 8;
	while (pos_cnt) {
		task_t *task = (task_t *)malloc(sizeof(*task));
		if (!task) {
			continue;
		}

		task->fn = task_handler;
		task->post_time = time(NULL);
		task->data = task;
		task->from_thrid = (pid_t)syscall(__NR_gettid);

		thread_pool_post(thread_pool, task);
		pos_cnt --;

		printf("pos_cnt: %d\n", pos_cnt);

		sleep(rand() % 3);
	}
}

int main(int argc, char **argv) {
	int num_of_threads = -1;
	thread_pool_t *thread_pool = NULL;
	thread_t *task_thread1, *task_thread2;

	if (argc >= 2) {
		num_of_threads = atoi(argv[1]);
	}

	thread_pool = thread_pool_init(num_of_threads);
	if (!thread_pool) {
		printf("thread pool init failed!\n");
		return -1;
	}
	thread_pool_start(thread_pool);

	task_thread1 = create_thread(0, producer, thread_pool);
	task_thread2 = create_thread(0, producer, thread_pool);
	start_thread(task_thread1);
	start_thread(task_thread2);
	wait_thread(task_thread1);
	wait_thread(task_thread2);

	return 0;
}
