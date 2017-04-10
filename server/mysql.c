#include "server.h"

void open_mysql_init(MYSQL **con)
{
	MYSQL *conn;
	char *server = "localhost";
	char *user = "root";
	char *password = "sjzxuzihao";
	char *database = "xzh_netdisk";
	conn = mysql_init(NULL);
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{
		*con = NULL;
		printf("Error connecting to databases:%s\n", mysql_error(conn));
	}
	else
	{
		*con = conn;
	}	
}

void log_record(MYSQL *conn, plog_data p)
{
	char query[200];//这里必须申请空间，如果用指针会报错
	sprintf(query,"%s%s%s%s%s%s%s%s%s%s%s%s%s%d%s%s", "insert into log (action,username,ip,port)values(", "'", p->action, "'", ",", "'", p->username, "'", ",", "'", p->ip, "'", ",", p->port, ")",";");
	int t;
	t = mysql_query(conn, query);
	if (t)
		printf("Error log insert:%s\n", mysql_error(conn));
}

void cmd_record(MYSQL *conn, pcmd_data p)
{
	char query[200];//这里必须申请空间，如果用指针会报错
	sprintf(query, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", "insert command (username,command,target,success)values(", "'", p->username, "'", ",", "'", p->command, "'", ",", "'", p->target, "'", ",", "'", p->success, "'", ")");
	int t;
	t = mysql_query(conn, query);
	if (t)
		printf("Error command insert:%s\n", mysql_error(conn));
}

int reg_record(MYSQL *conn, preg_data p)
{
	char query[200];//这里必须申请空间，如果用指针会报错
	sprintf(query, "%s%s%s%s%s%s%s%s%s%s%s%s%s", "insert user (username,salt,spasswd)values(", "'", p->username, "'", ",", "'", p->salt, "'", ",", "'", p->spasswd, "'", ")");
	int t;
	t = mysql_query(conn, query);
	if (t)
	{
		printf("Error register insert:%s\n", mysql_error(conn));
		return -1;
	}
	return 0;
}

int reg_query(MYSQL *conn, preg_data p)
{
	char query1[200], query2[200];//这里必须申请空间，如果用指针会报错
	MYSQL_RES *res1, *res2;
	MYSQL_ROW row1, row2;
	sprintf(query1, "%s%s%s%s", "select salt from user where username = ", "'", p->username, "'");
	sprintf(query2, "%s%s%s%s", "select spasswd from user where username = ", "'", p->username, "'");
	int t1, t2;
	t1 = mysql_query(conn, query1);
	if (t1)
	{
		printf("Error username\n");
		return -1;
	}
	else
	{
//		printf("quering salt...\n");
		res1 = mysql_use_result(conn);
		if (res1)
		{
			while((row1 = mysql_fetch_row(res1)) != NULL)
			{
				for(t1 = 0;t1 < mysql_num_fields(res1); t1++);
				{
					strcpy(p->salt, row1[0]);
				}
			}
		}
	}
	t2 = mysql_query(conn, query2);
	if (t2)
	{
		printf("Error username\n");
		return -1;
	}
	else
	{
//		printf("quering spasswd...\n");
		res2 = mysql_use_result(conn);
		if (res2)
		{
			while((row2 = mysql_fetch_row(res2)) != NULL)
			{
				for(t2 = 0;t2 < mysql_num_fields(res2); t2++);
				 strcpy(p->spasswd, row2[0]);
			}
		}
	}
	mysql_free_result(res1);
	mysql_free_result(res2);
	return 0;
}
