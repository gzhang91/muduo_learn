#include "00channel.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "00common.h"

#define HAVE_MSGHDR_MSG_CONTROL 1

int InitChannel(channel_t *channels, int n) {
	int i = 0;
	int ret = 0;
	int cnt = 3;

	for (; i < n; ++i) {
		channels[i].pid = 0;

		ret = pipe(channels[i].fd);
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

int WriteFd(int fd, void *ptr, size_t nbytes, int sendfd) {
	struct msghdr   msg;
	struct iovec    iov[1];

#ifdef  HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr    cm;
		char              control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr  *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
#else
	msg.msg_accrights = (caddr_t) &sendfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return (sendmsg(fd, &msg, 0));
}

int ReadFd(int fd, void *ptr, size_t nbytes, int *recvfd) {
    struct msghdr   msg;
    struct iovec    iov[1];
    ssize_t         n;
    int             newfd;

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
#else
    msg.msg_accrights = (caddr_t) &newfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ( (n = recvmsg(fd, &msg, 0)) <= 0)
        return(n);

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
        cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
					if (cmptr->cmsg_level != SOL_SOCKET) {
            PRINT(__FILE__, __LINE__, "control level != SOL_SOCKET");
						return -1;
					}
					if (cmptr->cmsg_type != SCM_RIGHTS) {
            PRINT(__FILE__, __LINE__, "control type != SCM_RIGHTS");
						return -1;
					}
        *recvfd = *((int *) CMSG_DATA(cmptr));
    } else
        *recvfd = -1;       /* descriptor was not passed */
#else
/* *INDENT-OFF* */
    if (msg.msg_accrightslen == sizeof(int))
        *recvfd = newfd;
    else
        *recvfd = -1;       /* descriptor was not passed */
/* *INDENT-ON* */
#endif

    return (n);
}
