#include "server.h"

void func_reg(int sfd)
{
	printf("register...\n");
	train t;
	bzero(&t, sizeof(train));
	char username[10] = {0};//用户名
	char *passwd;//密码
	char spasswd[512] = {0};//加密后的密码
	char salt[9] = {0};//从客户端接受到的salt
	char lsalt[12] = {0};//包含$6$的salt
	printf("Input username:\n");
	read(STDIN_FILENO, username, sizeof(username));//获得用户名
	printf("server handling, please wait....\n");
	t.len = strlen(username)-1;
	send(sfd, &t.len, 4, 0);//将用户名发送给服务器
	send_n(sfd, username, t.len);
	bzero(&t, sizeof(train));
	recv(sfd, &t.len, 4, 0);//接受服务器返回的salt
	recv_n(sfd, salt, t.len);
	sprintf(lsalt,"%s%s", "$6$", salt);//生成lsalt
	passwd = getpass("Input passwd:");//获得密码
	strcpy(spasswd, crypt(passwd, lsalt)+strlen(lsalt)+1);//生成加密后的密码
	t.len = strlen(spasswd);
	send(sfd, &t.len, 4, 0);//将加密后的密码传输给服务器
	send_n(sfd, spasswd, t.len);
	recv(sfd, &t.len, 4, 0);//等待服务器返回233即注册成功
	if (233 == t.len)
		printf("register success\n");
	else
		printf("register falied\n");

}
