#include <stdio.h>
#include "00common.h"
#include "../03_threading/thread.h"

/*
	����ʽ(concurrent)������
	1. one connection per process / one connection per thread
	2. �ʺ�ִ��ʱ��Ƚϳ��ķ���
*/

#define IP_PORT_LEN sizeof("255.255.255.255:6666.")

void *connection_dealer(void *arg) {
	char *buffer = NULL;
	int buffer_len = 0;
	int i = 0, ret = 0;

	int cli_fd = *((int *)arg);
	
	while (1) {
		buffer_len = Read(cli_fd, &buffer);
		if (buffer_len <= 0) {
			free(buffer);
			buffer = NULL;
			break;
		}

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

	return NULL;
}

int main(int argc, char **argv) {
	int lsn_fd, cli_fd;
	char ip_port[IP_PORT_LEN] = {0};

	lsn_fd = CreateSocketOrDie();
	BindOrDie(lsn_fd, "0.0.0.0", 8888);
	ListenOrDie(lsn_fd);
	
	while (1) {
		// Attention: accept�Ὣ���ӴӾ���������ȡ�����������Ӧ�ó��򲻻����accept�����ӷ��͵����ݾͻ�ʹ�����ס��
		cli_fd = AcceptOrDie(lsn_fd, ip_port, IP_PORT_LEN);
		PRINT(__FILE__, __LINE__, "id[%d] Get a new connection[%s]\n", cli_fd, ip_port);

		// ����io�߳�
		start_thread(create_thread(1, connection_dealer, (void *)&cli_fd));
	}

	return 0;
}