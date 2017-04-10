#include "server.h"

void remove_handle(int new_fd, ptrain p, pcmd_data cmd)
{
	train t;
	bzero(&t, sizeof(train));
	t.len = p->len;
	strcpy(t.buf, p->buf);
	char command[7] = {0};
	char filename[20] = {0};
	sscanf(t.buf, "%s%s", command, filename);
//	puts(command);
//	puts(filename);
	strcpy(cmd->target, filename);
	int ret = remove(filename);
	if (-1 == ret)
	{
		perror("remove");
		strcpy(cmd->success, "N\0");
		strcpy(t.buf, "remove failed\n");
	}else{
		strcpy(cmd->success, "Y\0");
		strcpy(t.buf, "file removed success\0");
	}
	t.len = strlen(t.buf);
	send_n(new_fd, (char *)&t, 4+t.len);
	return;
}
