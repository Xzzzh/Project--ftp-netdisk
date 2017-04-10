#include "server.h"

void cd_handle(int new_fd, ptrain p, pcmd_data cmd)
{
	train t;
	t.len = p->len;
	strcpy(t.buf, p->buf);
	char command[7] = {0};
	char rpath[100] = {0};
	sscanf(t.buf, "%s%s", command, rpath);
	strcpy(cmd->target, rpath);
	char cpath[100] = {0};
	char path[100] = {0};
	if (NULL == getcwd(cpath, sizeof(cpath)))
	{
		perror("getcwd");
		strcpy(cmd->success, "N\0");
		return;
	}
	sprintf(path, "%s%s%s", cpath, "/", rpath);
	int ret = chdir(path);
	if (0 != ret)
	{
		perror("chdir");
		strcpy(t.buf, "no such file or directory\n");
		t.len = strlen(t.buf);
		send_n(new_fd, (char *)&t, 4+t.len);
		strcpy(cmd->success, "N\0");
		return;
	}
	bzero(&path, sizeof(path));
	if (NULL == getcwd(path, sizeof(path)))
	{
		perror("getcwd");
		strcpy(cmd->success, "N\0");
		return;
	}
	strcpy(t.buf, path);
	t.len = strlen(t.buf);
	send_n(new_fd, (char *)&t, 4+t.len);
	strcpy(cmd->success, "Y\0");
	return;
}
