#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <unistd.h>

/*
	用于父子进程间消息传递，使用的是信号方式
	fd: 父进程写，打开fd[1], 关闭fd[0]
			子进程读，打开fd[0], 关闭fd[1]
		写的内容：（简单设计）
		-1: 代表需要关闭子进程
		>-1: 代表正常的文件描述符
*/
struct channel_s {
	int fd[2];	// socket pair
	pid_t pid;	// 子进程id
	//int cmd;		// 0-close, 1-io event
};

typedef struct channel_s channel_t;

int InitChannel(channel_t *channels, int n);

int SendFd(int fd, int fd_to_send);

int SendErr(int fd, int errcode, const char *msg);

int RecvFd(int sock_fd);


#endif // _CHANNEL_H_