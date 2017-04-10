#include "server.h"

void regist(int new_fd)
{
	printf("register...\n");
	MYSQL *conn;
	open_mysql_init(&conn);
	train t;
	bzero(&t, sizeof(train));
	reg_data r;
	bzero(&r, sizeof(reg_data));
	char username[10] = {0};
	char salt[9] = {0};
	char spasswd[512] = {0};
	recv(new_fd, &t.len, 4, 0);//从客户端接受用户名
	recv_n(new_fd, username, t.len);
	get_rand_str(salt, 8);//生成随机字符串,8位,salt
	t.len = 0;
	t.len = strlen(salt);
	send(new_fd, &t.len, 4, 0);//发送salt给客户端
	send_n(new_fd, salt, t.len);
	recv(new_fd, &t.len, 4, 0);//从客户端接受密文
	recv_n(new_fd, spasswd, t.len);
	strcpy(r.username, username);
	strcpy(r.salt, salt);
	strcpy(r.spasswd, spasswd);
	int ret = reg_record(conn, &r);
	if (-1 == ret)
	{
		t.len = 238;
		printf("register failed\n");
	}
	else
	{
		t.len = 233;
		printf("register success\n");
	}
	mysql_close(conn);
	send(new_fd, &t.len, 4, 0);
	return;
}
