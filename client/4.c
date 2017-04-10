#include "client.h"

void func_4(int sfd, ptrain p)
{
	train t = *p;
	send(sfd, &t.len, sizeof(int), 0);
	send_n(sfd, t.buf, t.len);
	bzero(t.buf, sizeof(t.buf));
	recv(sfd, &t.len, sizeof(int), 0);
	recv_n(sfd, t.buf, t.len);
	puts(t.buf);
}
