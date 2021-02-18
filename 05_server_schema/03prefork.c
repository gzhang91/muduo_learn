#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "00common.h"
#include "00channel.h"

/*
	����fork�󣬸�����ʹ��accept��ȡ���׽��ִ��ݸ��ӽ����ǲ���Ч�ģ���Ϊ��ʱ���ӽ��̲�û�ж�����һ���׽��֣�
	���²���ʹ�ø��׽��ֽ������ݴ���
	�˰汾��prefork���ɹ�����������������������
*/

#define PREFORK_PROCESS_MAX_N 5
#define IP_PORT_LEN sizeof("255.255.255.255:6666.")

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

	while (1) {
		// ��ȡ�����̷��͹�����������Ϣ
		//read_cnt = read(channel->fd[0], &cmd, sizeof(cmd));
		ReadFd(channel->fd[0], NULL, 0, &cmd);

		PRINT(__FILE__, __LINE__, "child process[%d] read cmd[%d]!\n", (int)getpid(), cmd);
		if (read_cnt != sizeof(cmd)) {
			PRINT(__FILE__, __LINE__, "Get an invalid cmd[%d]\n", cmd);
			continue;
		}

		if (cmd == -1) {
			PRINT(__FILE__, __LINE__, "child process[%d] will exit!\n", (int)getpid());
			break;
		}

		cli_fd = cmd;

		while (1) {
			// ���ﾡ�ܻ�ȡ���˸����̷�������fd,�����ӽ����в�û�д򿪸�fd����������ʹ��
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
}

void SignalHandler(int signum) {
	if (signum != SIGCHLD) {
		return;
	}

	pid_t pid;
	while ((pid = waitpid(-1, NULL, WNOHANG)) >= 0) {
		;; // do nothing
	}
}

int main(int argc, char **argv) {
	pid_t pid;
	int i = 0, ret = 0;

	
	int curr_pos = 0;
	int write_cnt = 0;
	struct sigaction new_action;

	process_n = get_nprocs() + 1;
	channels = (channel_t *)malloc(sizeof(channel_t*) * process_n);
	memset(channels, 0x0, sizeof(*channels));

	// ע���ź�
	new_action.sa_handler = SignalHandler;
	new_action.sa_sigaction = NULL;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction(SIGCHLD, &new_action, NULL);

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

	// �����̴�������¼�
	int lsn_fd, cli_fd;
	char ip_port[IP_PORT_LEN] = {0};

	lsn_fd = CreateSocketOrDie();
	BindOrDie(lsn_fd, "0.0.0.0", 8888);
	ListenOrDie(lsn_fd);

	while (1) {
		// Attention: accept�Ὣ���ӴӾ���������ȡ�����������Ӧ�ó��򲻻����accept�����ӷ��͵����ݾͻ�ʹ�����ס��
		cli_fd = AcceptOrDie(lsn_fd, ip_port, IP_PORT_LEN);
		PRINT(__FILE__, __LINE__, "id[%d] Get a new connection[%s]\n", cli_fd, ip_port);

		do {
			// ������������ǰ����ŵĽ�����
			if (channels[curr_pos].fd == 0) {
				curr_pos = (curr_pos ++) % process_n;
				continue;
			}

			//write_cnt = write(channels[curr_pos].fd[1], (void *)&cli_fd, sizeof(cli_fd));
			write_cnt = WriteFd(channels[curr_pos].fd[1], NULL, 0, cli_fd);
			/*if (write_cnt != sizeof(cli_fd)) {
				PRINT(__FILE__, __LINE__, "parent write to [%d] failed\n", (int)channels[curr_pos].pid);
				continue;
			}*/
			
			curr_pos = (curr_pos ++) % process_n;
			break;
		} while(1);

	}

	return 0;
}