/*************************************************************************
    > File Name: client_read.c
    > Author: yuwz
    > Mail: 1175141208@qq.com 
    > Created Time: 2016年09月01日 星期四 15时47分09秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include "client.h"
#include "client_menu.h"
#include "client_read.h"

extern int client_fd;//客户端文件描述符
extern pthread_cond_t	cond; //条件变量
extern pthread_mutex_t	mutex;//互斥锁

void go_on_clear(void);
/*
 * 函数名：clent_receive
 * 函数功能：用户登录后读取服务器发送的数据
 * 函数形参：void*(用户名)
 * 函数返回值：(void*)
 */
void* client_receive(void* name)
{
	int retval = -1;
	packet_t buff;
	while(1)
	{
		memset(&buff,0,sizeof(buff));
		if((retval = read(client_fd,&buff,sizeof(buff))) < 0)
		{
			perror("read error");
		}
		else if(retval == 0)
		{
			printf("Unable to connect to the server\n");
			pthread_exit((void*)0);
		}
		else
		{
			switch(buff.type)//检测第一次读到的数据包类型
			{
				case TYPE_SHELL:
					printf_buff();//调用读取并打印普通数据包函数
					go_on_clear();//调用任意键函数
					pthread_cond_broadcast(&cond);//通知主线程
					break;
				case TYPE_ONLINE:
					printf_buff();//调用读取并打印普通数据包函数
					go_on_clear();//调用任意键函数
					pthread_cond_broadcast(&cond);//通知主线程
					break;
				case TYPE_CHAT:
					printf_msg((char*)name,buff);//调用打印聊天信息函数
					break;
				case TYPE_GROUP:
					printf_msg((char*)name,buff);//调用打印聊天信息函数
					break;
				case TYPE_ANONYMOUS:
					printf_msg((char*)name,buff);//调用打印匿名聊天信息函数
					break;
				case TYPE_PASSWD:
					printf_passwd(buff);//调用打印修改密码返回的信息函数
					go_on_clear();//调用任意键继续函数
					pthread_cond_broadcast(&cond);//通知主线程
					break;
				default:
					break;
			}
		}
	}
	return (void*)0;
}
/*
 * 函数名：printf_passwd
 * 函数功能：修改密码后检测服务器返回的是否修改成功信息
 * 函数形参：packet_t(数据包)
 * 函数返回值：无
 */
void printf_passwd(packet_t buff)
{
	if(strcmp(buff.data,"permint") == 0)
	{
		printf("\n修改成功，下次登陆启用新密码！\n");
		return;
	}
	else
	{
		printf("\n修改失败，请重试！\n");
	}
}
/*
 * 函数名：print_buff
 * 函数功能：读取并打印服务器返回的普通数据包
 * 函数形参：无
 * 函数返回值：无
 */
void printf_buff(void)
{
	char buff[1024];
	int retval = -1;
	while(1)//读取服务器发送给自己的无类型数据包信息
	{
		memset(buff,'\0',sizeof(buff));
		if((retval = read(client_fd,buff,sizeof(buff))) < 0)
		{
			perror("read error");
			break;
		}
		else if(retval == 0)
		{
			printf("Unable to connect to the server\n");
			exit(1);
		}
		else
		{
			printf("\033[32m%s\033[0m",buff);
			if(retval < 1024)
			{
				break;
			}
		}
	}
}
/*
 * 函数名：print_msg
 * 函数功能：打印服务器发送的聊天数据包
 * 函数形参：char*(用户名),packet_t(数据包)
 * 函数返回值：无
 */
void printf_msg(char* name,packet_t buff)
{

	if(!strcmp(name,buff.username))//服务器返回给自己发的消息的发送状态
	{
		YELLOW;
		printf("%s\n",buff.data);
		CLOSE;
	}
	else//别人发给自己的消息
	{
		printf("\n\033[34m%s%s:%s\033[0m\n",rettime(),buff.username,buff.data);
		save_message(buff,name);
	}
	pthread_cond_broadcast(&cond);//通知主进程
}

