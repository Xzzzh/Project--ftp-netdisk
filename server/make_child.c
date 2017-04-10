#include "server.h"

void make_child(pchild p, int num, MYSQL *conn)
{
	int fds[2];
	int i;
	pid_t pid;
	for (i = 0; i < num; i++)
	{
		socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
		pid = fork();
		if (!pid)
		{
			close(fds[1]);
			child_handle(fds[0], conn);
		}else{
			close(fds[0]);
			p[i].pid = pid;
			p[i].fdw = fds[1];
			p[i].busy = 0;
		}
	}
}

void child_handle(int fdr, MYSQL *conn)
{
	int new_fd;
	short closeflag = 1;
	struct sockaddr_in cli;
	while(1)
	{
		recv_fd(fdr, &new_fd);//会阻塞
		bzero(&cli, sizeof(struct sockaddr_in));
		read(fdr, &cli, sizeof(struct sockaddr_in));
//		printf("cli ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
		chdir("/mnt/hgfs/share2/p_poll/server/child");
		handle_request(new_fd, conn, cli);
		write(fdr, &closeflag, sizeof(closeflag));
		close(new_fd);
	}
}
