#include "server.h"

void ls_handle(int new_fd, pcmd_data cmd)
{
	train t;
	bzero(&t, sizeof(train));
	DIR *dir;
	dir = opendir(getcwd(NULL, 0));
	if (NULL == dir)
	{
		perror("opendir");
		closedir(dir);
		return;
	}
	struct dirent *p;
	struct stat buf;
	int i, j;
	char path[512];
	int set = 0;
	while(NULL != (p = readdir(dir)))
	{
		if (0 == strcmp(p->d_name, ".") || 0 == strcmp(p->d_name, ".."))
			continue;
		sprintf(path, "%s%s%s", getcwd(NULL, 0), "/", p->d_name);
		memset(&buf, 0, sizeof(buf));
		stat(path, &buf);
		switch (buf.st_mode >> 12 & 15)
		{
			case 4:
				sprintf(t.buf + set, "%s%s%s", "d ", p->d_name, "\n");
				break;
			case 8:
				sprintf(t.buf + set, "%s%s%s", "- ", p->d_name, "\n");
				break;
			default:
				sprintf(t.buf + set, "%s%s%s", "  ", p->d_name, "\n");
				break;
		}
		t.len = strlen(t.buf);
		set = strlen(t.buf);
	}
	t.len = strlen(t.buf) - 1;
	send_n(new_fd, (char *)&t, 4+t.len);
	closedir(dir);
	strcpy(cmd->success, "Y\0");
	return;
}
