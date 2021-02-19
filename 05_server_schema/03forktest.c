#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "00common.h"
#include "00channel.h"

channel_t *channels;
int process_n = 0;

void ProcessDealer(channel_t *channel) {
	int cmd = 0;
	int cli_fd = 0;
	int read_cnt; 
	char *buffer = NULL;
	int buffer_len = 0;
	int i = 0, ret = 0;

	// �ӽ��̹ر�д
	close(channel->fd[1]); 

	do {
		// ��ȡ�����̷��͹�����������Ϣ
		//read_cnt = read(channel->fd[0], &cmd, sizeof(cmd));
		PRINT(__FILE__, __LINE__, "Recv a message\n");
		cmd = RecvFd(channel->fd[0]);

		PRINT(__FILE__, __LINE__, "child process[%d] read cmd[%d]!\n", (int)getpid(), cmd);

		if (cmd == -1) {
			PRINT(__FILE__, __LINE__, "child process[%d] will exit!\n", (int)getpid());
			break;
		}
	} while (0);

	write(cli_fd, "TEST", 5);
}

int main() {
	pid_t pid;
	int i = 0, ret = 0;
	int curr_pos = 0;
	int fd;
	char filename[32] = {0};

	process_n = 1; //get_nprocs() + 1;
	channels = (channel_t *)malloc(sizeof(channel_t) * process_n);
	memset(channels, 0x0, sizeof(channels) * process_n);

	// ��ʼ��channel
	if ((ret = InitChannel(channels, process_n)) < 0) {
		PRINT(__FILE__, __LINE__, "cannot open channel\n");
		exit(-1);
	}

	for (i = 0; i < process_n; ++i) {
		pid	= fork();
		if (pid == 0) { // �ӽ���
			break;
		}

		// �����̹رն�
		close(channels[i].fd[0]);
		channels[i].pid = pid;
	}

	if (pid == 0) {
		ProcessDealer(&channels[i]);
		return 0;
	}

	sleep(1);

	for (i = 0; i < process_n; ++i) {
		memset(filename, 0x0, sizeof(filename));
		sprintf(filename, "/tmp/%d", i);
		fd = open(filename, O_CREAT);

		PRINT(__FILE__, __LINE__, "fd[%d] Send a message\n", fd);
		SendFd(channels[i].fd[1], fd);
		close(fd);
	}
	
	return 0;
}
