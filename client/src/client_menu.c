/*************************************************************************
    > File Name: client_menu.c
    > Author: yuwz
    > Mail: 1175141208@qq.com 
    > Created Time: 2016年09月01日 星期四 15时47分26秒
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

/*
 * 函数名：client_menu
 * 函数功能：一级菜单控制函数
 * 函数形参：无
 * 函数返回值：无
 */
void client_menu(void)
{
	system("clear");
	int choose=1;
	char ch = 0;
	while(1)
	{	
		menu_first(choose);
		ch=getch();//取第一个值进行判断是否是方向键
		if(ch==27){
			if(getch()==91){
				if(getch()==65){//方向键上时choose--，即菜单向上移动选择
					choose--;
					if(choose<1){
						choose=3;
					}system("clear");
				}
				else {//其他方向键时都执行choose++，即菜单向下移动选择
					choose++;
					if(choose>3){
						choose=1;
					}
					system("clear");
				}
			}
		}
		else if(ch =='\r')//回车确认选择
		{
			switch(choose)
			{
				case 1:
					client_register();//调用用户注册函数
					break;
				case 2:
					client_login();//调用用户登陆函数
					break;
				case 3:
					return ;
				default:
					break;
			}
		}
		else
		{
			system("clear");
		}
	}
}
/*
 * 函数名：menu_first
 * 函数功能：一级菜单界面显示
 * 函数形参：int(选项)
 * 函数返回值：无
 */
void menu_first(int choose)
{
	printf("\t\t╔═════════╦══════════════════╦══════════╗\n");
	printf("\t\t█—————————║ 远程终端管理系统 ║——————————█\n");
	printf("\t\t╚═════════╩══════════════════╩══════════╝\n");
	
	if(choose==1){
		printf("\t\t║\t\t\033[44m新人注册\033[0m\t\t║\n");
	}
	else{
		printf("\t\t║\t\t新人注册\t\t║\n");
	}
	if(choose==2){
		printf("\t\t║\t\t\033[44m用户登陆\033[0m\t\t║\n");
	}
	else{
		printf("\t\t║\t\t用户登陆\t\t║\n");
	}
	if(choose==3){
		printf("\t\t║\t\t\033[44m退出系统\033[0m\t\t║\n");
	}
	else{
		printf("\t\t║\t\t退出系统\t\t║\n");
	}
	printf("\t\t╚═══════════════════════════════════════╝\n");
	printf("\t\t请选择：\n");
}
/*
 * 函数名：client_submenu
 * 函数功能：用户登录后新建线程并调用二级菜单
 * 函数形参：char(用户名)
 * 函数返回值：无
 */
void client_submenu(char *name)
{
	/* 已分离属性创建新线程*/
	pthread_t pthread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if(pthread_create(&pthread,&attr,client_receive,(void*)name) < 0)
	{
		perror("pthread create error");
	}
	pthread_attr_destroy(&attr);//销毁属性
	submenu(name,pthread);//主线程调用二级菜单函数
}
/*
 * 函数名：submenu
 * 函数功能：二级菜单控制函数
 * 函数形参：char(用户名),pthread_t(线程标示)
 * 函数返回值：无
 */
void submenu(char* name,pthread_t pthread)
{
	int choose=1;
	char ch = 0;
	alarm(3);
	while(1)
	{	
		usleep(1000);
		menu_second(choose,name);
		ch=getch();//取第一个值进行判断是否是方向键
		if(ch==27){
			if(getch()==91){
				if(getch()==65){//方向键上时choose--，即菜单向上移动选择
					choose--;
					if(choose<1){
						choose=5;
					}system("clear");
				}
				else {//其他方向键时都执行choose++，即菜单向下移动选择
					choose++;
					if(choose>5){
						choose=1;
					}
					system("clear");
				}
			}
		}
		else if(ch =='\r')//回车确认选择
		{
			switch(choose)
			{
			case 1:
				client_shell();//调用发送shell命令函数
				pthread_mutex_lock(&mutex);//加锁
				pthread_cond_wait(&cond, &mutex);//等待被通知
				pthread_mutex_unlock(&mutex);//解锁
				break;
			case 2:
				client_online();//调用查看在线用户函数
				pthread_mutex_lock(&mutex);//加锁
				pthread_cond_wait(&cond, &mutex);//等待被通知
				pthread_mutex_unlock(&mutex);//解锁
				break;
			case 3:
				chat_menu(name);//调用聊天菜单函数
				break;
			case 4:
				client_passwd(name);//调用修改密码函数
				pthread_mutex_lock(&mutex);//加锁
				pthread_cond_wait(&cond, &mutex);//等待被通知
				pthread_mutex_unlock(&mutex);//解锁
				break;
			case 5:
				pthread_cancel(pthread);//取消子线程
				close(client_fd);//关闭客户端文件描述符
				alarm(0);//取消时钟
				system("clear");
				exit(0);//结束进程
			}
		}
		else
		{
			system("clear");
		}
		
		
	}
}
/*
 * 函数名：menu_second
 * 函数功能：二级菜单界面显示
 * 函数形参：int(选项),char(用户名)
 * 函数返回值：无
 */
void menu_second(int choose,char* name)
{
	printf("\n\t\t╔━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╗\n");
	printf("\t\t\t\t欢迎　%s　回来...\n",name);
	printf("\t\t╚━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╝\n");
	
	if(choose==1){
		printf("\t\t┃\t\t\033[44mMy Shell\033[0m\t\t┃\n");
	}
	else{
		printf("\t\t┃\t\tMy Shell\t\t┃\n");
	}
	if(choose==2){
		printf("\t\t┃\t\t\033[44m在线用户\033[0m\t\t┃\n");
	}
	else{
		printf("\t\t┃\t\t在线用户\t\t┃\n");
	}
	if(choose==3){
		printf("\t\t┃\t\t\033[44m在线聊天\033[0m\t\t┃\n");
	}
	else{
		printf("\t\t┃\t\t在线聊天\t\t┃\n");
	}
	if(choose==4){
		printf("\t\t┃\t\t\033[44m修改密码\033[0m\t\t┃\n");
	}
	else{
		printf("\t\t┃\t\t修改密码\t\t┃\n");
	}
	if(choose==5){
		printf("\t\t┃\t\t\033[44m退出系统\033[0m\t\t┃\n");
	}
	else{
		printf("\t\t┃\t\t退出系统\t\t┃\n");
	}
	printf("\t\t╚━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╝\n");
	printf("\t\t请选择：\n");
}
/*
 * 函数名：chat_menu
 * 函数功能：聊天菜单界面
 * 函数形参：char*(用户名)
 * 函数返回值：无
 */
void chat_menu(char* name)
{
	int choose = -1;
	system("clear");
	while(1)
	{
		usleep(1000);
		printf("\t┃━━━━━━━━━━━━━━━━━━━━━━━━━━━┃\n");
		printf("\t┃\t1:私聊\t\t    ┃\n");	
		printf("\t┃\t2:群聊\t\t    ┃\n");
		printf("\t┃\t3:消息记录\t    ┃\n");	
		printf("\t┃\t0:退出\t\t    ┃\n");
		printf("\t┃━━━━━━━━━━━━━━━━━━━━━━━━━━━┃\n");
		printf("\t  请选择:\n");
		scanf("%d",&choose);
		while(getchar() != '\n');
		switch(choose)
		{
			case 1:
				client_chat(name);//调用客户端私聊函数
				break;
			case 2:
				group_chat(name);//调用客户端群聊函数
				break;
			case 3:
				message_record(name);//调用查看消息记录函数
				go_on_clear();//调用任意键继续函数
				break;
			case 0:
				system("clear");
				return;
			default:
				printf("Wrong choice!\n");
				break;
		}
	}
}

