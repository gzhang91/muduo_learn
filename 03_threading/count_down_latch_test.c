#include <stdio.h>
#include <unistd.h>
#include "count_down_latch.h"

void *worker(void *arg) {
	count_down_latch_t *cdt = (count_down_latch_t *)arg;
	CountDown(cdt);

	pthread_t pid = pthread_self();
	printf("thread[%d] finished !\n", (pid_t)pid);
}

int main(int argc, char **argv) {
	pthread_t worker_th1, worker_th2, worker_th3;
	
	printf("main thread start ... \n");
	count_down_latch_t *cdt = InitCountDownLatch(3);
	if (cdt == NULL) {
		printf("create count down latch failed !\n");
		return -1;
	}

	pthread_create(&worker_th1, NULL, worker, cdt);
	pthread_create(&worker_th2, NULL, worker, cdt);
	pthread_create(&worker_th3, NULL, worker, cdt);

	// 等待几秒, 让他们都运行了
	sleep(2);

	Wait(cdt);
	printf("wait for all thread done !\n");
	DestroyCountDownLatch(cdt);

	return 0;
}