#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/*
	本案例用来测试使用fork后之后的两个进程对于mutex等对象的访问
*/

static pthread_mutex_t mutex; // = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutexattr_t attr;

int main(void) {
	pid_t pid;
	pthread_mutexattr_init(&attr);
	int ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	printf("ret: %d\n", ret);
	if (ret < 0) {
		printf("failed to set process shared, errno: %d\n", errno);
		return -1;
	}

	pthread_mutex_init(&mutex, &attr);
	
	if ((pid = fork()) < 0) {
		printf("fork failed\n");
		return -1;
	}

	if (pid > 0) {
		printf("[%d] parent ... \n", getpid());
		printf("main &mutex: %p\n", &mutex); 
		pthread_mutex_lock(&mutex);
		printf("parent come on\n");
		int i = 8;
		while (i--) {
			printf("[%d] sleep.\n", getpid());
			sleep(1);
		}
		pthread_mutex_unlock(&mutex);
		printf("main over ...\n");
		waitpid(pid, NULL, 0);
	} else {
		sleep(5);
		printf("[%d] child ... \n", getpid());
		printf("child &mutex: %p\n", &mutex);
		pthread_mutex_lock(&mutex);
		printf("have nothing\n");
		sleep(1);
		pthread_mutex_unlock(&mutex);
	}
	
	printf("[%d] end ...\n", getpid());
		
	return 0;
}