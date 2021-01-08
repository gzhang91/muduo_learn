#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
	创建线程之后fork
	fork之后只会拷贝对应的执行流，不会管理之前的执行流
*/

void *thrd_entry(void *arg) {
	pid_t thr_id = (pid_t)syscall(SYS_gettid);
	printf("[%d]->[%d] other thread3 ... \n", getpid(), thr_id);

	pid_t pid1 = fork();
	if (pid1 < 0) {
		printf("fork failed !\n");
		return NULL;
	}

	printf("[%d]->[%d] other thread4 ... \n", getpid(), (pid_t)syscall(SYS_gettid));
	if (pid1 > 0) {
		waitpid(pid1, NULL, 0);
	}

	return NULL;
}

int main(void) {
	pthread_t thr1;
	pid_t get_pid = getpid();
	pthread_create(&thr1, NULL, thrd_entry, NULL);
	printf("[%d]->[%d] main thread1 ... \n", get_pid, (pid_t)syscall(SYS_gettid));
	pthread_join(thr1, NULL);

	pid_t pid1 = fork();
	if (pid1 < 0) {
		printf("fork failed !\n");
		return -1;
	}

	printf("[%d]->[%d] main thread2 ... \n", getpid(), (pid_t)syscall(SYS_gettid));
	if (pid1 > 0) {
		waitpid(pid1, NULL, 0);
	}

	return 0;
}