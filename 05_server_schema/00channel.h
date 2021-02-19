#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <unistd.h>

/*
	���ڸ��ӽ��̼���Ϣ���ݣ�ʹ�õ����źŷ�ʽ
	fd: ������д����fd[1], �ر�fd[0]
			�ӽ��̶�����fd[0], �ر�fd[1]
		д�����ݣ�������ƣ�
		-1: ������Ҫ�ر��ӽ���
		>-1: �����������ļ�������
*/
struct channel_s {
	int fd[2];	// socket pair
	pid_t pid;	// �ӽ���id
	//int cmd;		// 0-close, 1-io event
};

typedef struct channel_s channel_t;

int InitChannel(channel_t *channels, int n);

int SendFd(int fd, int fd_to_send);

int SendErr(int fd, int errcode, const char *msg);

int RecvFd(int sock_fd);


#endif // _CHANNEL_H_