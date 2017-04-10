#include "client.h"

//准确发送len个字节到达对方
int send_n(int sfd, char *buf, int len)
{
	int total = 0;
	int ret;
	while(total < len)
	{
		ret = send(sfd, buf + total, len - total, 0);
		if (-1 == ret)
			return -1;
		total = total + ret;
	}
	return -1;
}

//准确接受len个字节
int recv_n(int sfd, char *buf, int len)
{
	int total = 0;
	int ret;
	while(total < len)
	{
		ret = recv(sfd, buf + total, len - total, 0);
		total = total + ret;
	}
	return -1;
}
