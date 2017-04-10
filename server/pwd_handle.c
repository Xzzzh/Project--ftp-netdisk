#include "server.h"

void pwd_handle(int new_fd, pcmd_data cmd)
{
	train t;
	bzero(&t, sizeof(train));
	char path[100];
	if(NULL == getcwd(path, sizeof(path)))
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
