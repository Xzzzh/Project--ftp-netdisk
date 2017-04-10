#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <crypt.h>
#include <shadow.h>
#include <mysql/mysql.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <time.h>
#include <ctype.h>

typedef struct{
	pid_t pid;
	int fdw;
	short busy;
}child, *pchild;

typedef struct{
	int len;
	char buf[1000];
}train, *ptrain;

typedef struct
{
	char action[10];
	char username[20];
	char ip[16];
	int port;
}log_data, *plog_data;

typedef struct
{
	char username[20];
	char command[10];
	char target[20];
	char success[2];
}cmd_data, *pcmd_data;

typedef struct
{
	char username[20];
	char salt[9];
	char spasswd[512];
}reg_data, *preg_data;

void recv_fd(int, int *);
void send_fd(int, int);
void make_child(pchild, int, MYSQL *);
void child_handle(int, MYSQL *);
void handle_request(int, MYSQL *, struct sockaddr_in);
int send_n(int, char *, int);
int recv_n(int, char *, int);
void ls_handle(int, pcmd_data);
void pwd_handle(int, pcmd_data);
void cd_handle(int, ptrain, pcmd_data);
void remove_handle(int, ptrain, pcmd_data);
void gets_handle(int , ptrain, pcmd_data);
void puts_handle(int , ptrain, pcmd_data);
void open_mysql_init(MYSQL **);
void log_record(MYSQL *, plog_data);
void cmd_record(MYSQL *, pcmd_data);
int reg_record(MYSQL *, preg_data);
void regist(int);
void get_rand_str(char *, int);
int reg_query(MYSQL *, preg_data);
