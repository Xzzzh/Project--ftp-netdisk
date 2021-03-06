#include "server.h"

void send_fd(int fdw, int fd)
{
	struct msghdr msg;
	memset(&msg, 0, sizeof(struct msghdr));
	struct iovec iov[2];
	char buf1[5] = " ";
	char buf2[5] = " ";
	iov[0].iov_base = buf1;
	iov[0].iov_len = strlen(buf1);
	iov[1].iov_base = buf2;
	iov[1].iov_len = strlen(buf2);
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	struct cmsghdr *cmsg;
	int len = CMSG_LEN(sizeof(int));
	cmsg = (struct cmsghdr *)calloc(1, len);
	cmsg->cmsg_len = len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int *)CMSG_DATA(cmsg) = fd;
	msg.msg_control = cmsg;
	msg.msg_controllen = len;
	int ret = sendmsg(fdw, &msg, 0);
	if (-1 == ret)
	{
		perror("sendmsg");
		return;
	}
}

void recv_fd(int fdr, int *pfd)
{
	struct msghdr msg;
	memset(&msg, 0, sizeof(struct msghdr));
	struct iovec iov[2];
	char buf1[5] = " ";
	char buf2[5] = " ";
	iov[0].iov_base = buf1;
	iov[0].iov_len = strlen(buf1);
	iov[1].iov_base = buf2;
	iov[1].iov_len = strlen(buf2);
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	struct cmsghdr *cmsg;
	int len = CMSG_LEN(sizeof(int));
	cmsg = (struct cmsghdr *)calloc(1, len);
	cmsg->cmsg_len = len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control = cmsg;
	msg.msg_controllen = len;
	int ret = recvmsg(fdr, &msg, 0);
	if (-1 == ret)
	{
		perror("recvmsg");
		return;
	}
	*pfd = *(int *)CMSG_DATA(cmsg);
}
