#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
	�̴߳���֮ǰ��fork
	fork֮�󴴽��̣߳�����ָ��ӽ����ж�����һ�����߳�+main�߳�
*/

void *thrd_entry(void *arg) {
	pid_t get_pid = getpid();
	pid_t thr_id = (pid_t)syscall(SYS_gettid);
	
	int i = 5;
	while (i--) {
		printf("[%d]->[%d] other thread ... \n", get_pid, thr_id);
		sleep(1);
	}
	return NULL;
}

int main(void) {
	pid_t pid1;
	pthread_t thr1;

	pid1 = fork();
	if (pid1 < 0) {
		printf("fork failed !\n");
		return -1;
	}

	pid_t get_pid = getpid();

	pthread_create(&thr1, NULL, thrd_entry, NULL);
	pthread_join(thr1, NULL);

	printf("[%d]->[%d] main thread ... \n", get_pid, (pid_t)syscall(SYS_gettid));
	waitpid(pid1, NULL, 0);
	
	return 0;
}