#include "server.h"

void get_rand_str(char *s, int num)
{
	char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./";//salt的取值范围
	int i;//循环控制变量
	int lstr;//str的长度
	lstr = strlen(str);
	char ss[2] = {0};//用来存生成的随机字符,第二个元素为\0
	srand((unsigned int)time((time_t *)NULL));//随机数种子
	for (i = 0; i < num; i++)
	{
		sprintf(ss, "%c", str[(rand()%lstr)]);
		strcat(s, ss);//生成的随机字符链接到传入的字符指针后
	}
}
