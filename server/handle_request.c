#include "server.h"

void handle_request(int new_fd, MYSQL *conn, struct sockaddr_in cli)
{
	MYSQL *con;
	open_mysql_init(&con);
	if (NULL == con)
	{
		printf("error open mysql\n");
		return;
	}
	train t;
	reg_data r;
//	struct spwd *sp;
	char *passwd;
	char salt[12] = {0};
passwd_bg:
	bzero(&r, sizeof(reg_data));
	bzero(&t, sizeof(train));
	recv(new_fd, &t.len, 4, 0);
	if(0 == t.len){
		return;
	}
	recv_n(new_fd, t.buf, t.len);//接受客户端传来的用户名
	log_data log;
	cmd_data cmd;
	strcpy(log.username, t.buf);
	strcpy(cmd.username, t.buf);
	strcpy(r.username, t.buf);
	strcpy(log.action, "login");
	strcpy(log.ip, inet_ntoa(cli.sin_addr));
	log.port = ntohs(cli.sin_port);
	int ret = reg_query(con, &r);//查询不到返回-1
//	printf("ret = %d\n", ret);
	if (-1 == ret)
	{
		t.len = -5;
		send(new_fd, &t.len, 4, 0);
//		printf("illigal name\n");
		goto passwd_bg;
	}
	else{
//		printf("ligal name\n");
		t.len = strlen(r.salt);
		send(new_fd, &t.len, 4, 0);
		send_n(new_fd, r.salt, t.len);
		recv(new_fd, &t.len, 4, 0);
		recv_n(new_fd, t.buf, t.len);
		if (strcmp(r.spasswd, t.buf) == 0){
			t.len = 1;
			send(new_fd, &t.len, 4, 0);
			log_record(con, &log);
			goto passwd_ed;
		}
		else{
			t.len = -1;
			send(new_fd, &t.len, 4, 0);
			goto passwd_bg;
		}
	}
passwd_ed:
	printf("client login success\n");
	while (1)
	{

		bzero(&t, sizeof(t));
		recv(new_fd, &t.len, sizeof(t.len), 0);
		if(0 == t.len){
			break;
		}
		recv(new_fd, &t.buf, t.len, 0);
		if (!strncmp(t.buf, "ls", 2)){
			strcpy(cmd.command, "ls\0");
			strcpy(cmd.target, "./\0");
			printf("command ls receved\n");
			ls_handle(new_fd, &cmd);
			cmd_record(con, &cmd);
		}
		else if (!strncmp(t.buf, "cd", 2)){
			strcpy(cmd.command, "cd\0");
			printf("command cd receved\n");
			cd_handle(new_fd, &t, &cmd);
			cmd_record(con, &cmd);
		}
		else if (!strncmp(t.buf, "pwd", 3)){
			strcpy(cmd.command, "pwd\0");
			strcpy(cmd.target, "./\0");
			printf("command pwd receved\n");
			pwd_handle(new_fd, &cmd);
			cmd_record(con, &cmd);
		}
		else if (!strncmp(t.buf, "puts", 4)){
			strcpy(cmd.command, "puts\0");
			printf("command puts receved\n");
			puts_handle(new_fd, &t, &cmd);
			cmd_record(con, &cmd);
		}
		else if (!strncmp(t.buf, "gets", 4)){
			strcpy(cmd.command, "gets\0");
			printf("command gets receved\n");
			gets_handle(new_fd, &t, &cmd);
			cmd_record(con, &cmd);
		}
		else if (!strncmp(t.buf, "remove", 6)){
			strcpy(cmd.command, "remove\0");
			printf("command remove receved\n");
			remove_handle(new_fd, &t, &cmd);
			cmd_record(con, &cmd);
		}
	}
	strcpy(log.action, "logout");
	log_record(con, &log);
	mysql_close(con);
}
