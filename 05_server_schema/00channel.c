#include "00channel.h"
#include <unistd.h>
#include <sys/types.h>
#define __USE_GNU
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include "00common.h"

#define CREDSTRUCT		ucred
#define CR_UID				uid
#define CREDOPT				SO_PASSCRED
#define SCM_CREDTYPE	SCM_CREDENTIALS

#define RIGHTSLEN CMSG_LEN(sizeof(int))
#define CREDSLEN	CMSG_LEN(sizeof(struct CREDSTRUCT))
#define CONTROLLEN (RIGHTSLEN + CREDSLEN)

static struct cmsghdr *cmptr = NULL;

int InitChannel(channel_t *channels, int n) {
	int i = 0;
	int ret = 0;
	int cnt = 3;

	for (; i < n; ++i) {
		channels[i].pid = 0;

		ret = socketpair(PF_UNIX, SOCK_DGRAM, 0, channels[i].fd);
		if (ret < 0 && cnt > 0) { // will try again 3 times
			i--;
			cnt--;
			continue;
		}

		if (ret < 0) {
			return -1;
		}

		cnt = 3;
	}
	
	return 0;
}

int SendFd(int fd, int fd_to_send) {
	struct iovec iov[1]; 
	struct msghdr msg;  
	char buff[0];   

	//指定缓冲区
	iov[0].iov_base = buff;
	iov[0].iov_len = 1;

	//通过socketpair进行通信，不需要知道ip地址
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	//指定内存缓冲区
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	//辅助数据
	struct cmsghdr cm;
	cm.cmsg_len = CMSG_LEN(sizeof(fd)); //描述符的大小
	cm.cmsg_level = SOL_SOCKET;         //发起协议
	cm.cmsg_type = SCM_RIGHTS;          //协议类型
	*(int*)CMSG_DATA(&cm) = fd_to_send; //设置待发送描述符

	//设置辅助数据
	msg.msg_control = &cm;
	msg.msg_controllen = CMSG_LEN(sizeof(fd));

	sendmsg(fd, &msg, 0);  //发送描述符
}

int SendErr(int fd, int errcode, const char *msg) {
	int n;
	if ((n = strlen(msg)) > 0) {
		if (write(fd, msg, n) != n) {
			return -1;
		}
	}

	if (errcode >= 0) {
		errcode = -1;
	}

	if (SendFd(fd, errcode) < 0) {
		return -1;
	}

	return 0;
}

int RecvFd(int fd) {
	struct iovec iov[1];  
	struct msghdr msg;
	char buff[0];

	//指定缓冲区
	iov[0].iov_base = buff;
	iov[0].iov_len = 1;

	//通过socketpair进行通信，不需要知道ip地址
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	//指定内存缓冲区
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	//辅助数据
	struct cmsghdr cm;

	//设置辅助数据
	msg.msg_control = &cm;
	msg.msg_controllen = CMSG_LEN(sizeof(fd));

	recvmsg(fd, &msg, 0);  //接收文件描述符

	int cli_fd = *(int*)CMSG_DATA(&cm);
	return cli_fd;
}
