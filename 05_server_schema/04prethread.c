#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include "00common.h"
#include "00channel.h"
#include "../03_threading/thread_pool.h"

#define PREFORK_PROCESS_MAX_N 5
#define IP_PORT_LEN sizeof("255.255.255.255:6666.")

channel_t *channels;
int process_n = 0;

void *TaskFunc(void *arg) {
	int cli_fd = *(int *)arg;
	int i = 0, ret = 0, buffer_len = 0;
	char *buffer = NULL;

	while (1) {
		// 这里尽管获取到了父进程发过来的fd,但是子进程中并没有打开该fd，不能立即使用
		buffer_len = Read(cli_fd, &buffer);
		if (buffer_len <= 0) {
			free(buffer);
			buffer = NULL;
			break;
		}

		PRINT(__FILE__, __LINE__, "child process[%d] fd[%d] get buffer 3!\n", (int)getpid(), cli_fd);

		printf("id[%d] Get buffer[%d], content:", cli_fd, buffer_len);
		for (i = 0; i < buffer_len; i++) {
			printf("%c", buffer[i]);
		}
		printf("\n");

		ret = Write(cli_fd, buffer, buffer_len);
		if (ret <= 0) {
			free(buffer);
			buffer = NULL;
			break;
		}

		free(buffer);
		buffer = NULL;
	}

	PRINT(__FILE__, __LINE__, "id[%d] Service over for this connection[%d]\n", cli_fd, cli_fd);

}

int main(int argc, char **argv) {
	pid_t pid;
	int i = 0, ret = 0;

	if (argc >= 2) {
		process_n = atoi(argv[1]);
	}

	if (process_n <= 0) {
		process_n = get_nprocs() + 1;
	}
	int curr_pos = 0;
	int write_cnt = 0;
	struct sigaction new_action;

	channels = (channel_t *)malloc(sizeof(channel_t) * process_n);
	memset(channels, 0x0, sizeof(channel_t) * process_n);

	// 初始化channel
	if ((ret = InitChannel(channels, process_n)) < 0) {
		PRINT(__FILE__, __LINE__, "cannot open channel\n");
		exit(-1);
	}

	thread_pool_t *thr_pool = thread_pool_init(process_n);
	thread_pool_start(thr_pool);

	// 父线程处理监听事件
	int lsn_fd, cli_fd;
	char ip_port[IP_PORT_LEN] = {0};

	lsn_fd = CreateSocketOrDie();
	BindOrDie(lsn_fd, "0.0.0.0", 8888);
	ListenOrDie(lsn_fd);

	while (1) {
		// Attention: accept会将连接从就绪队列中取出，如果现在应用程序不会调用accept，连接发送的数据就会就此阻塞住。
		cli_fd = AcceptOrDie(lsn_fd, ip_port, IP_PORT_LEN);
		PRINT(__FILE__, __LINE__, "id[%d] Get a new connection[%s]\n", cli_fd, ip_port);

		do {
			// 将任务塞到当前的序号的进程中
			task_t *task = (task_t *)malloc(sizeof(*task));
			if (!task) {
				continue;
			}

			task->fn = TaskFunc;
			task->post_time = time(NULL);
			task->data = &cli_fd;
			task->from_thrid = (pid_t)syscall(__NR_gettid);

			thread_pool_post(thr_pool, task);
			break;
		} while(1);

	}

	return 0;
}