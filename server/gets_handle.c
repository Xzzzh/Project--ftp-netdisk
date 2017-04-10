#include "server.h"

void gets_handle(int new_fd, ptrain p, pcmd_data cmd)
{
	int ret;
	int fd;
	train t;
	bzero(&t, sizeof(train));
	t.len = p->len;
	strcpy(t.buf, p->buf);
	char command[7] = {0};
	char filename[20] = {0};
	sscanf(t.buf, "%s%s", command, filename);
	strcpy(cmd->target, filename);
	DIR *dir;
	struct stat buf;
	char path[50];
	char cpath[50];
	int flen = 0;
	getcwd(cpath, sizeof(cpath));
	dir = opendir(cpath);
	if (NULL == dir)
	{
		perror("opendir");
		strcpy(cmd->success, "N\0");
		return;
	}
	struct dirent *d;
	while(NULL != (d = readdir(dir)))
	{
		if (0 == strcmp(d->d_name, ".") || 0 == strcmp(d->d_name, ".."))
			continue;
		if (!strcmp(d->d_name, filename))
		{
			sprintf(path, "%s%s%s", cpath, "/", d->d_name);
			stat(path, &buf);
			if(buf.st_size < 102400*1024)
			{
				bzero(&t, sizeof(train));
				t.len = strlen(filename);
				strcpy(t.buf, filename);
				send(new_fd, &t.len, 4, 0);
				send(new_fd, &t.buf, t.len, 0);
				fd = open(filename, O_RDWR);
				if(-1 == fd)
				{
					perror("open");
					strcpy(cmd->success, "N\0");
					goto END;
				}
				recv(new_fd, &flen, 4, 0);
				printf("flen = %d\n", flen);
				if (flen > 0)
				{
					printf("O_APPEND\n");
				}
				lseek(fd, flen, SEEK_SET);
				printf("normal sending\n");
				while((t.len = read(fd, t.buf, sizeof(t.buf))) > 0)
				{
					send_n(new_fd, (char *)&t, 4+t.len);
				}
				strcpy(cmd->success, "Y\0");
				printf("send success\n");
				goto END;
			}
			else
			{
				bzero(&t, sizeof(train));
				t.len = strlen(filename);
				strcpy(t.buf, filename);
				send(new_fd, &t.len, 4, 0);
				send(new_fd, &t.buf, t.len, 0);
				fd = open(filename, O_RDWR);
				if(-1 == fd)
				{
					perror("open");
					strcpy(cmd->success, "N\0");
					goto END;
				}
				char *pmap;
				int s = 0;
				pmap = (char *)mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
				if ((char *)-1 == pmap)
				{
					perror("mmap");
					strcpy(cmd->success, "N\0");
					goto END;
				}
				recv(new_fd, &flen, 4, 0);
				printf("flen = %d\n", flen);
				if (flen>0)
				{
					printf("O_APPEND\n");
				}
				printf("mmap sending\n");
				char *cur = pmap;
				cur = cur + flen;
				bzero(&t, sizeof(train));
				while(1)
				{
					if (sizeof(t.buf)*(s+1) > (buf.st_size - flen))
						break;
					t.len = sizeof(t.buf);
					memcpy(t.buf, cur, sizeof(t.buf));
					send_n(new_fd, (char *)&t, 4+t.len);
					cur = cur + sizeof(t.buf);
					s++;
				}
				t.len = (buf.st_size - flen - sizeof(t.buf)*(s));
				memcpy(t.buf, cur, t.len);
				send_n(new_fd, (char *)&t, 4+t.len);
				strcpy(cmd->success, "Y\0");
				printf("send success\n");
				munmap(pmap, buf.st_size);
				goto END;
			}
		}
	}
	strcpy(cmd->success, "N\0");
END:
	t.len = -1;
	send(new_fd, &t.len, sizeof(int), 0);
	close(fd);
	return;
}
