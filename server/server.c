#include "server.h"

int main()
{
	FILE *f = fopen("../conf/ser.conf", "rb");//读取配置文件
	if (NULL == f)
	{
		perror("fopen");
		return -1;
	}
	int num;//进程池深度
	char ip[16] = {0};//服务器ip
	unsigned short port;//服务器端口
	fscanf(f, "%s%hd%d", ip, &port, &num);
	fclose(f);
	MYSQL *conn;//打开数据库
	open_mysql_init(&conn);
	if (NULL == conn)
	{
		printf("error open mysql\n");
		return -1;
	}
//	char *query = "insert into log(username)values('server')";
//	printf("main query = %s\n", query);
//	int t = mysql_query(conn,query);
//	cmd_data cmd;
//	log_data log;
	pchild p = (pchild)calloc(num, sizeof(child));//结构体数组记录子进程的状态和信息
	make_child(p, num, conn);//生成子进程
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	int reuse = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(int));//设置服务器端口和ip可以复用
	if (-1 == sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	struct sockaddr_in cli;
	int len = sizeof(cli);
	memset(&ser, 0, sizeof(struct sockaddr_in));
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip);
	ser.sin_family = AF_INET;
	int ret = bind(sfd, (struct sockaddr *)&ser, sizeof(ser));
	if (-1 == ret)
	{
		perror("bind");
		return -1;
	}
	int epfd;
	epfd = epoll_create(1);
	struct epoll_event event, *events;
	memset(&event, 0, sizeof(struct epoll_event));
	event.events = EPOLLIN;
	event.data.fd = sfd;
	events = (struct epoll_event *)calloc(num + 1, sizeof(struct epoll_event));
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	if (-1 == ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	int i;
	for (i = 0; i < num; i++)//循环注册子进程描述符为监听状态,此描述符是用来传递new_fd 和子进程状态信息的
	{
		event.events = EPOLLIN;
		event.data.fd = p[i].fdw;
		epoll_ctl(epfd, EPOLL_CTL_ADD, p[i].fdw, &event);
	}
	ret = listen(sfd, num);
	if (-1 == ret)
	{
		perror("listen");
		return -1;
	}
	int new_fd;
	int j;
	short flag;
	int ret_epoll;
	int recv_ret;
	train t;
	int clinum = 0;
	while(1)//循环监听
	{
		memset(events, 0, sizeof(struct epoll_event)*(num + 1));
		ret_epoll = epoll_wait(epfd, events, num+1, -1);
		if (ret_epoll > 0)
		{
			for (i = 0; i < ret_epoll; i++)
			{
				if (sfd == events[i].data.fd)
				{
					bzero(&cli, sizeof(struct sockaddr_in));
					len = sizeof(cli);
					new_fd = accept(sfd, (struct sockaddr *)&cli, &len);
					clinum++;
					printf("%d st client connecting\n", clinum);
regist_bg:
					bzero(&t, sizeof(train));
					recv_ret = recv(new_fd, &t.len, 4, 0);//从客户端接受选择信息
					if (recv_ret == 0)//判断客户端是否退出
					{
						printf("client colsed\n");
						close(new_fd);
						continue;
					}
					if (t.len < 0)//客户端发来-1表示注册
					{
						regist(new_fd);	
						goto regist_bg;						
					}
					for (j = 0; j < num; j++)
					{
						if (0 == p[j].busy)
						{
					//		clinum++;
							printf("%d st client connected\n", clinum);
						//	printf("process %d will be busy\n", p[j].pid);
							send_fd(p[j].fdw, new_fd);//把new_fd 传递给空闲的子进程
//							printf("send cli ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
							write(p[j].fdw, &cli, sizeof(cli));
							p[j].busy = 1;
							break;
						}
					}
					close(new_fd);
				}
				for (j = 0; j < num; j++)
				{
					if (events[i].events == EPOLLIN && p[j].fdw == events[i].data.fd)
					{
						read(p[j].fdw, &flag, sizeof(flag));
						p[j].busy = 0;//子进程变为非忙碌
						printf("%d st client disconnected\n", clinum);
						clinum--;
					//	printf("process %d is avaliable\n", p[j].pid);
					}
				}
			}
		}
	}
	return 0;
}
