#include "client.h"

void func_puts(int sfd, ptrain p)
{
	train t = *p;
	int fd;
	DIR *dir;
	struct dirent *d;
	char filename[20] = {0}; 
	char command[7] = {0};
	char cpath[50] = {0};
	sscanf(t.buf, "%s%s", command, filename);
	getcwd(cpath, sizeof(cpath));
	dir = opendir(cpath);
	if (NULL == dir)
	{
		perror("opendir");
		closedir(dir);
		goto err_puts;
	}
	printf("file opened\n");
	while(NULL != (d = readdir(dir)))
	{
		if (!strcmp(d->d_name, filename))
		{
			printf("ready to open file\n");
			fd = open(filename, O_RDWR);
			if (-1 == fd)
			{
				perror("open");
				bzero(filename, sizeof(filename));
				goto puts_END;
			}
			t.len = sizeof(command);
			send(sfd, &t.len, sizeof(int), 0);
			send_n(sfd, command, t.len);
			t.len = strlen(filename);
			send(sfd, &t.len, sizeof(int), 0);
			puts(filename);
			send_n(sfd, filename, t.len);
			printf("filename sended\n");
			recv(sfd, &t.len, sizeof(int), 0);
			if (t.len <= 0)
			{
				printf("server error\n");
				break;
			}
			bzero(&t, sizeof(train));
			while((t.len = read(fd, t.buf, sizeof(t.buf))) > 0)
			{
				send_n(sfd, (char *)&t, 4+t.len);
			}
			t.len = -1;
			send(sfd, &t.len, 4, 0);
			printf("send success\n");
			goto puts_END;
		}
	}
	printf("no such file on client dir\n");
puts_END:
	close(fd);
	closedir(dir);
err_puts:
	bzero(t.buf, sizeof(t.buf));
}
