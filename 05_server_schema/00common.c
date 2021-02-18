#include "00common.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

void PRINT(const char *file, int line, const char *fmt, ...) {
	printf("[%s] [%d] ", file, line); 
	va_list args;	
	va_start(args, fmt); 
	vprintf(fmt, args); 
	va_end(args);
}

int CreateSocketOrDie() {
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd <= 0) {
		PRINT(__FILE__, __LINE__, "create socket failed, errno[%d]!\n", errno);
		exit(-1);
	}

	return fd;
}

void BindOrDie(int fd, const char *ip_text, unsigned short port) {
	int ret;

	struct sockaddr_in addr;
	memset(&addr, 0x0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	ret = inet_pton(AF_INET, ip_text, &(addr.sin_addr));
	if (ret < 0) {
		PRINT(__FILE__, __LINE__, "ip address[%s] is invalid!\n", ip_text);
		exit(-1);
	}

	ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		PRINT(__FILE__, __LINE__, "bind failed!, errno[%d]\n", errno);
		exit(-1);
	}
}

void ListenOrDie(int fd) {
	int ret;

	ret = listen(fd, 128);
	if (ret < 0) {
		PRINT(__FILE__, __LINE__, "listen failed!, errno[%d]\n", errno);
		exit(-1);
	}
}

int AcceptOrDie(int fd, char ip_text[], int len) {
	int ret, retfd;

	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);

	retfd = accept(fd, (struct sockaddr *)&addr, &addr_len);
	if (retfd < 0) {
		PRINT(__FILE__, __LINE__, "accept failed, errno[%d]!\n", errno);
		exit(-1);
	}

	const char *ptr = inet_ntop(AF_INET, &(addr.sin_addr), ip_text, len);
	if (ptr == NULL) {
		PRINT(__FILE__, __LINE__, "inet_ntop failed!\n");
		exit(-1);
	}

	sprintf(ip_text + strlen(ip_text), ":%d", addr.sin_port);

	return retfd;
}

int Read(int fd, char **buff) {
	int read_len = 0;
	char *tmp = NULL;
	int len = 0;

	int buff_len = 0;
	*buff = (char *)malloc(sizeof(char) * 1024);
	if (!*buff) {
		PRINT(__FILE__, __LINE__, "malloc failed!\n");
		return -1;
	}

	while (1) {
		read_len = read(fd, *buff + buff_len, 1024);
		if (read_len < 0) {
			PRINT(__FILE__, __LINE__, "error occurred, read failed[%d], errno[%d]\n", read_len, errno);
			return -1;
		}

		if (read_len == 0) {
			PRINT(__FILE__, __LINE__, "remote had closed\n");
			return 0;
		}

		if (read_len < 1024) {
			len += read_len;
			break;
		}

		tmp = realloc(*buff, 1024);
		if (!tmp) {
			PRINT(__FILE__, __LINE__, "realloc failed!\n");
			return -1;
		}

		buff_len += 1024;
		len = buff_len;
		*buff = tmp;
	}

	return len;
}

int Write(int fd, char *buff, int len) {
	int write_len = 0;
	int ret = 0;

	while (write_len < len) {
		ret = write(fd, buff + write_len, len - write_len);
		if (ret < 0) {
			PRINT(__FILE__, __LINE__, "error occurred, write failed[%d], errno[%d]\n", ret, errno);
			return -1;
		}

		if (ret == 0) {
			PRINT(__FILE__, __LINE__, "remote had closed\n");
			return 0;
		}

		write_len += ret;
	}

	return write_len;
}