#include <stdio.h>
#include "00common.h"
/*
	循环迭代式服务器
*/

#define IP_PORT_LEN sizeof("255.255.255.255:6666.")

int main(int argc, char **argv) {
	
	int i = 0, ret = 0;
	int lsn_fd, cli_fd;
	char *buffer = NULL;
	int buffer_len = 0;

	lsn_fd = CreateSocketOrDie();
	BindOrDie(lsn_fd, "0.0.0.0", 8888);
	ListenOrDie(lsn_fd);

	char ip_port[IP_PORT_LEN] = {0};

	while (1) {
		// Attention: accept会将连接从就绪队列中取出，如果现在应用程序不会调用accept，连接发送的数据就会就此阻塞住。
		cli_fd = AcceptOrDie(lsn_fd, ip_port, IP_PORT_LEN);
		PRINT(__FILE__, __LINE__, "Get a new connection[%s]\n", ip_port);

		while (1) {
			buffer_len = Read(cli_fd, &buffer);
			if (buffer_len <= 0) {
				free(buffer);
				buffer = NULL;
				break;
			}

			printf("Get buffer[%d], content:", buffer_len);
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

		PRINT(__FILE__, __LINE__, "Service over for this connection[%s]\n", ip_port);
	}

	return 0;
}