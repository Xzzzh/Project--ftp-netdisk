#include "client.h"

int main()
{
	FILE *f = fopen("../conf/cli.conf", "rb");//读取配置文件
	if (NULL == f)
	{
		perror("fopen");
		return -1;
	}
	char trash[1000];
	char ip[16] = {0};
	unsigned short port;
	fscanf(f, "%s%hd", ip, &port);
	fclose(f);
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser, 0, sizeof(struct sockaddr_in));
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip);
	ser.sin_family = AF_INET;
	int ret = connect(sfd, (struct sockaddr *)&ser, sizeof(struct sockaddr_in));
	if (-1 == ret)
	{
		perror("connect");
		return -1;
	}
	printf("connect success\n");
	train t;
	bzero(&t, sizeof(train));
	char tag[200];
	int switchtime = 0;
	char username[10] = {0};
	char *passwd;
	char spasswd[512] = {0};
	char salt[9] = {0};
	char fsalt[12] = {0};
//	struct spwd *sp;
	int flag = 0;	
	int flag2 = 0;	
BEGIN:
	bzero(tag, sizeof(tag));
	printf("Longin or register? Register input 1, login input 2\n");//操作选择
	read(STDIN_FILENO, tag, sizeof(tag));
	if(!strncmp(tag, "1", 1))	
	{
		t.len = -1;
		send(sfd, &t.len, 4, 0);//发送-1标志位，表示注册
		func_reg(sfd);
		goto BEGIN;	
	}else if(!strncmp(tag, "2", 1))
	{
		t.len = 1;//发送1标志位，表示登录
		send(sfd, &t.len, 4, 0);
		goto NEXT;
	}else
	{
		printf("wrong input\n");
		if (switchtime < 5)
		{
			switchtime++;
			goto BEGIN;	
		}
		printf("Too many wrong time\n");	
		close(sfd);
		return -1;
	}
NEXT:
	printf("Input username:\n");
	read(STDIN_FILENO, username, sizeof(username));
//	while(read(STDIN_FILENO, trash, sizeof(trash)));
	bzero(&t, sizeof(train));
passwd_bg:
	t.len = strlen(username)-1;
	strcpy(t.buf, username);
	send_n(sfd, (char *)&t, t.len+4);
	recv(sfd, &t.len, 4, 0);//接受服务器端传来的用户名校验情况
	if (-5 == t.len)
	{
		if (flag > 4)
		{
			printf("Too many wrong times\n");
			close(sfd);	
			return -1;
		}
		flag++;
		printf("wrong username\n");
		goto NEXT;
	}
	recv_n(sfd, t.buf, t.len);
	strcpy(salt, t.buf);
	passwd = getpass("Passwd:");
	sprintf(fsalt, "%s%s", "$6$", salt);
	strcpy(spasswd, crypt(passwd, fsalt)+12);
	t.len = strlen(spasswd);
	send(sfd, &t.len, 4, 0);
	send_n(sfd, spasswd, t.len);
	recv(sfd, &t.len, 4, 0);
	if (1 == t.len)
	{
		printf("passwd check success\n");
		goto passwd_ed;
	}
	else
	{
		if (flag2 > 4)
		{
			printf("Too many wrong times\n");
			close(sfd);
			return -1;
		}
		flag2++;
		printf("wrong passwd\n");
		goto passwd_bg;
	}
passwd_ed:
	while (1)
	{
		bzero(&t, sizeof(train));
		printf("SYSTEM: input command:\n");
		read(STDIN_FILENO, t.buf, sizeof(t.buf));
		t.len = strlen(t.buf);
		if (!strncmp(t.buf, "ls", 2))
			func_4(sfd, &t);
		else if (!strncmp(t.buf, "cd", 2))
			func_4(sfd, &t);
		else if (!strncmp(t.buf, "pwd", 3))
			func_4(sfd, &t);
		else if (!strncmp(t.buf, "gets", 4))
			func_gets(sfd, &t);
		else if (!strncmp(t.buf, "puts", 4))
			func_puts(sfd, &t);
		else if (!strncmp(t.buf, "remove", 6))
			func_4(sfd, &t);
		else
			printf("error command\n");
	}
}
