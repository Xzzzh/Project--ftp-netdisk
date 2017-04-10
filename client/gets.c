#include "client.h"

void func_gets(int sfd, ptrain p)
{
	train t = *p;
	int fd;
	struct stat buf;
	DIR *dir;
	struct dirent *d;
	char path[50] = {0};
	char cpath[50] = {0};
	char filename[50] = {0};
	getcwd(cpath, sizeof(cpath));
	dir = opendir(cpath);
	if (NULL == dir)
	{
		perror("opendir");
		return;
	}
	send(sfd, &t.len, sizeof(int), 0);
	send_n(sfd, t.buf, t.len);
	bzero(t.buf, sizeof(t.buf));
	recv(sfd, &t.len, sizeof(int), 0);
	if(0 >= t.len)
	{
		printf("server error or no such file\n");
		closedir(dir);
		return;
	}
	bzero(&t.buf, sizeof(t.buf));
	recv(sfd, t.buf, t.len, 0);
	puts(t.buf);
	strcpy(filename, t.buf);
	int flen = 0;
	while(NULL != (d = readdir(dir)))
	{
		if(0 == strcmp(d->d_name, ".") || 0 == strcmp(d->d_name, ".."))
			continue;
		if (!strcmp(d->d_name, filename))
		{
			sprintf(path, "%s%s%s", cpath, "/", d->d_name);
			stat(path, &buf);
			flen = buf.st_size;	
			break;
		}
	}
	printf("flen = %d\n", flen);
	send(sfd, &flen, 4, 0);
	if (flen > 0)
	{
		fd = open(filename, O_APPEND|O_RDWR);
		printf("O_APPEND\n");
	}else{
		printf("O_CREAT\n");
		fd = open(filename, O_CREAT|O_RDWR, 0600);
	}
	if (-1 == fd)
	{
		perror("open");
		close(fd);
		return;
	}
	printf("receving ...\n");
	while(1)
	{
		recv_n(sfd, (char *)&t.len, sizeof(t.len));
		if (t.len >= 0)
		{
			bzero(t.buf, sizeof(t.buf));
			recv_n(sfd, t.buf, t.len);
			write(fd, t.buf, t.len);
		}else{
			close(fd);
			printf("receve success\n");
			return;
		}
	}
	printf("continue\n");
}
