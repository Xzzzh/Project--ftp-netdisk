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

typedef struct{
	pid_t pid;
	int fdw;
	short busy;
}child, *pchild;

typedef struct{
	int len;
	char buf[1000];
}train, *ptrain;

#define FILENAME "file1"

void recv_fd(int, int *);
void send_fd(int, int);
void make_child(pchild, int);
void child_handle(int);
void handle_request(int);
int send_n(int, char *, int);
int recv_n(int, char *, int);
void ls_handle(int);
void pwd_handle(int);
void cd_handle(int, ptrain);
void remove_handle(int, ptrain);
void gets_handle(int , ptrain);
void puts_handle(int , ptrain);
void func_4(int, ptrain);
void func_puts(int, ptrain);
void func_gets(int, ptrain);
void func_reg(int);
