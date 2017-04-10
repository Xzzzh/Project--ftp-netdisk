#include "server.h"

void puts_handle(int new_fd, ptrain p, pcmd_data cmd)
{
	int len;
	char buf[1000];
	bzero(buf, sizeof(buf));
	train t;
	bzero(&t, sizeof(train));
	t.len = p->len;
	strcpy(t.buf, p->buf);
	int fd;
	printf("command = %s\n", t.buf);
	char filename[20] = {0};
	char command[10] = {0};
	recv(new_fd, &t.len, 4, 0);
	recv(new_fd, filename, t.len, 0);
	strcpy(cmd->target, filename);
	puts(filename);
	fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, 0600);
	if (-1 == fd)
	{
		perror("open");
		len = -1;
		send(new_fd, &len, sizeof(int), 0);
		strcpy(cmd->success, "N\0");
		return;
	}
	printf("file opend\n");
	len = 1;
	send(new_fd, &len, sizeof(int), 0);
	printf("receving ...\n");
	while(1)
	{
		recv(new_fd, &len, sizeof(int), 0);
		if (len > 0)
		{
			bzero(buf, sizeof(buf));
			recv_n(new_fd, buf, len);
			write(fd, buf, len);
		}
		else
		{
			close(fd);
			printf("receve success\n");
			break;
		}
	}
	strcpy(cmd->success, "Y\0");
	return;
}
