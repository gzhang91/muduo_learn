#include <stdio.h>
#include <unistd.h>
#include "../03_threading/thread.h"
#include "logger.h"

void custom_output_handler(int fd, const char *msg, int len) {
	write(fd, msg, len);
}

void *thr_func(void *arg) {
	for (int i = 0; i < 10; i++) {
		LOG_DEBUG("[%d] file[%s] cannot open, will exit\n", i, "/etc/my.cnf");
	}

	return NULL;
}

int main(int argc, char **argv) {
	SetLogLevel(DEBUG);

	for (int i = 0; i < 10; i++) {
		LOG_DEBUG("[%d] file[%s] cannot open, will exit\n", i, "/etc/my.cnf");
	}
	
	thread_t *thr1 = create_thread(0, thr_func, NULL);
	start_thread(thr1);

	thread_t *thr2 = create_thread(0, thr_func, NULL);
	start_thread(thr2);

	InitLog("./log.txt", INFO, custom_output_handler);
	for (int i = 0; i < 10; i++) {
		LOG_INFO("[%d] write to file[%s]\n", i, "log.txt");
	}

	thread_t *thr3 = create_thread(0, thr_func, NULL);
	start_thread(thr3);

	LOG_FATAL("some error, will exit!\n");

	wait_thread(thr1);
	wait_thread(thr2);
	wait_thread(thr3);

	return 0;
}
