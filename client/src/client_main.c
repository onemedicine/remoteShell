/*************************************************************************
    > File Name: client_main.c
    > Author: yuwz
    > Mail: 1175141208@qq.com 
    > Created Time: 2016年08月30日 星期二 14时12分18秒
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
#include "serveraddr.h"
#include "client.h"
#include "client_menu.h"
#include "client_read.h"

int client_fd;//客户端文件描述符
pthread_cond_t	cond; //条件变量
pthread_mutex_t	mutex;//互斥锁


int main(void)
{
	/*创建套接字，IPV4域，流式套接字默认协议TCP*/
	if((client_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("socket error");
		exit(1);
	}
	/*登记时钟信号处理函数*/
	if(signal(SIGALRM,signal_alrm) == SIG_ERR)
	{
		perror("signal_alrm error");
	}
	pthread_cond_init(&cond, NULL);//初始化条件变量
	pthread_mutex_init(&mutex,NULL);//初始化互斥锁
	/*定义一个网络专用地址结构体并初始化*/
	struct sockaddr_in clientaddr;
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(PORT);
	clientaddr.sin_addr.s_addr = inet_addr(IP);
	
	socklen_t len = sizeof(clientaddr);
	/*连接服务器*/
	if(connect(client_fd,(struct sockaddr*)&clientaddr,len) < 0)
	{
		perror("connect error");
		exit(1);
	}
	client_menu();//调用主菜单函数
	close(client_fd);//关闭文件描述符
	pthread_cond_destroy(&cond);//销毁条件变量
	pthread_mutex_destroy(&mutex);//销毁互斥锁
	return 0;
}
/*
 * 函数名：signal_alrm
 * 函数功能：时钟信号捕获函数
 * 函数形参：信号
 * 函数返回值：无
 */
void signal_alrm(int signo)
{
	packet_t buff;
	memset(&buff,0,sizeof(buff));
	alarm(3);
	int retval = -1;
	if(signo == SIGALRM)
	{
		buff.type = TYPE_HEART;//心跳包数据类型
		time_t bg = time(NULL);
		sprintf(buff.data,"%ld",bg);
		if((retval = write(client_fd,&buff,sizeof(buff))) < 0)//向服务器发送心跳包
		{
			perror("write error");
			close(client_fd);
		}
		else if(retval == 0)
		{
			close(client_fd);
			exit(0);
		}
	}
}

/*
 * 函数名：client_register
 * 函数功能：新用户注册函数
 * 函数形参：无
 * 函数返回值：无
 */
void client_register(void)
{
	char repeat[20] = "";
	packet_t buff;
	int retval = -1;
	memset(&buff,0,sizeof(buff));
	buff.type = TYPE_REGS;
	printf("\t\t请设置用户名:");
	fflush(stdout);
	scanf("%s",buff.username);
	while(getchar() != '\n');
	printf("\t\t请设置密码:");
	fflush(stdout);
	hide_passwd(buff.userpwd);
	printf("\t\t请确认密码:");
	fflush(stdout);
	hide_passwd(repeat);
	if(strncmp(repeat,buff.userpwd,20))//比对两次输入的密码是否一样
	{
		system("clear");
		printf("密码确认失败,注册失败!\n");
		return ;
	}
	/*发送注册类型数据包到服务器*/
	if((retval = write(client_fd,&buff,sizeof(buff))) < 0)
	{
		perror("write error");
	}
	else if(retval == 0)
	{
		printf("Unable to connect to the server\n");
		exit(1);
	}
	else 
	{	/*发送成功检测服务器返回的数据判断是否注册成功*/
		memset(&buff,0,sizeof(buff));
		if((retval = read(client_fd,&buff,sizeof(buff))) < 0)
		{
			perror("read error");
		}
		else if(retval == 0)
		{
			printf("Unable to connect to the server\n");
			exit(1);
		}
		else
		{
			system("clear");
			printf("\n%s!\n",buff.data);
		}
	}
}
/*
 * 函数名：client_login
 * 函数功能：用户登陆函数
 * 函数形参：无
 * 函数返回值：无
 */
void client_login(void)
{
	packet_t buff;
	char name[20];
	memset(&buff,0,sizeof(buff));
	memset(name,0,sizeof(name));
	int retval = -1;

	int i = 3;
	for(;i > 0;i--)
	{
		memset(&buff,0,sizeof(buff));
		buff.type = TYPE_LOGIN;
		printf("\t\t请输入用户名:");
		fflush(stdout);
		scanf("%s",buff.username);
		while(getchar() != '\n');
		strcpy(name,buff.username);
		printf("\t\t请输入密码:");
		fflush(stdout);
		hide_passwd(buff.userpwd);
		while(1)
		{	/*调用验证码比对函数*/
			if(random_codes())
			{
				continue;
			}
			else
			{
				break;
			}
		}
		/*发送登陆数据包给服务器*/
		if((retval = write(client_fd,&buff,sizeof(buff))) < 0)
		{
			perror("write error");
		}
		else if(retval == 0)
		{
			printf("Unable to connect to the server\n");
			exit(1);
		}
		else 
		{	/*读取服务器返回的数据判断是否登陆成功*/
			memset(&buff,0,sizeof(buff));
			if((retval = read(client_fd,&buff,sizeof(buff))) < 0)
			{
				perror("read error");
			}
			else if(retval == 0)
			{
				printf("Unable to connect to the server\n");
				exit(1);
			}
			else
			{
				if(strcmp(buff.data,"permint") == 0)
				{
					system("clear");
					printf("\n\t\t登陆成功\n");
					client_submenu(name);
					return ;
				}
				else
				{
					system("clear");
					printf("\n\t\t登录失败(还有%d机会)\n",i-1);
					continue;
				}
			}
		}
	}
}

/*
 * 函数名：client_passwd
 * 函数功能：用户修改密码的逻辑函数
 * 函数形参：char(用户名)
 * 函数返回值：无
 */
void client_passwd(char* name)
{
	packet_t buff;
	int retval = -1;
	memset(&buff,0,sizeof(buff));
	strcpy(buff.username,name);
	buff.type = TYPE_PASSWD;
	while(1)
	{
		printf("请输入旧密码:");
		fflush(stdout);
		hide_passwd(buff.userpwd);
		printf("\n请输入新密码:");
		fflush(stdout);
		hide_passwd(buff.data);
		if(!strcmp(buff.userpwd,buff.data))//排除修改与旧密码一样的情况
		{
			printf("\n请不要与旧密码一样\n");
			continue;
		}
		if((retval = write(client_fd,&buff,sizeof(buff))) < 0)//发送修改密码数据包给服务器
		{
			perror("write error");
			break;
		}
		else if(retval == 0)
		{
			printf("Unable to connect to the server\n");
			exit(1);
		}
		break;
	}	
}

/*
 * 函数名：client_shell
 * 函数功能：用户发送shell命令的逻辑函数
 * 函数形参：无
 * 函数返回值：无
 */
void client_shell(void)
{
	packet_t buff;
	int retval = -1;
	memset(&buff,0,sizeof(buff));
	buff.type = TYPE_SHELL;
	printf("please input order:");
	fflush(stdout);
	read(STDIN_FILENO,&buff.data,sizeof(buff.data));
	if((retval = write(client_fd,&buff,sizeof(buff))) < 0)//发送给服务器
	{
		perror("write error");
	}
	else if(retval == 0)
	{	
		printf("Unable to connect to the server\n");
		exit(1);
	}
}
/*
 * 函数名：client_online
 * 函数功能：用户查看在线用户逻辑函数
 * 函数形参：无
 * 函数返回值：无
 */
void client_online(void)
{
	packet_t buff;
	memset(&buff,0,sizeof(buff));
	buff.type = TYPE_ONLINE;
	int retval = -1;
	if((retval = write(client_fd,&buff,sizeof(buff))) < 0)//发送给服务器
	{
		perror("write error");
	}
	else if(retval == 0)//服务器关闭时
	{
		printf("Unable to connect to the server\n");
		exit(1);
	}
}

/*
 * 函数名：go_on_clear
 * 函数功能：按任意键继续
 * 函数形参：无
 * 函数返回值：无
 */
void go_on_clear(void)
{
	printf("按任意键继续...");
	fflush(stdout);
	getchar();
	system("clear");
}
/*
 * 函数名：message_record
 * 函数功能：用户查看消息记录函数
 * 函数形参：char*(用户名)
 * 函数返回值：无
 */
void message_record(char* name)
{
	char file[25] = "";
	strcpy(file,name);
	char* path = strcat(file,".txt");//拼接用户名和后缀
	char buff[1024];
	FILE* fp = fopen(path,"r");//只读模式打开文件
	while(1)//循环读取文价信息并打印
	{
		memset(buff,'\0',sizeof(buff));
		fgets(buff,sizeof(buff),fp);
		printf("%s",buff);
		if(feof(fp))//读到文件尾跳出循环
		{
			break;
		}
	}
}
/*
 * 函数名：save_message
 * 函数功能：用户保存消息记录函数
 * 函数形参：packet_t(数据包),char*(用户名)
 * 函数返回值：无
 */
void save_message(packet_t buff,char* name)
{
	char file[25] = "";
	strcpy(file,name);
	char* path = strcat(file,".txt");//拼接用户名和后缀
	int save = dup(STDOUT_FILENO);
	
	int fd = open(path,O_CREAT|O_WRONLY|O_APPEND,0666);//打开该用户的聊天记录文件以创建文件描述符
	dup2(fd,STDOUT_FILENO);//重定向聊天记录文件
	printf("\n%s%s: %s\n",rettime(),buff.username,buff.data);
	dup2(save,STDOUT_FILENO);
}
/*
 * 函数名：client_chat
 * 函数功能：客户端私聊函数
 * 函数形参：char*(用户名)
 * 函数返回值：无
 */
void client_chat(char* name)
{
	packet_t buff;
	memset(&buff,0,sizeof(buff));
	strcpy(buff.username,name);
	printf("＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿\n");
	int retval = -1;
	while(1)
	{
		usleep(1000);
		memset(buff.chatname,0,sizeof(buff.chatname));//清空聊天好友名
		memset(buff.data,0,sizeof(buff.data));//清空消息
		buff.type = TYPE_CHAT;
		printf("ID：");
		fflush(stdout);
		scanf("%s",buff.chatname);
		while(getchar()!='\n');
		if(!strcmp(buff.chatname,buff.username))//排除自己发送给自己的情况
		{
			printf("You can't send to yourself!\n");
			continue;
		}
		
		printf("Message(\"exit\" for exit)：");
		fflush(stdout);
		fgets(buff.data,sizeof(buff.data),stdin);
		
		if(strncmp(buff.data,"exit",4) == 0)
		{
			system("clear");
			break;
		}
		save_message(buff,name);//调用保存消息记录函数
		if((retval = write(client_fd,&buff,sizeof(buff))) < 0)//发送给服务器
		{
			perror("write error");
			break;
		}
		else if(retval == 0)
		{
			printf("Unable to connect to the server\n");
			exit(1);
		}
		pthread_mutex_lock(&mutex);//加锁
		pthread_cond_wait(&cond, &mutex);//加入等待队列,等待返回发送状态
		pthread_mutex_unlock(&mutex);//解锁
	}
}
/*
 * 函数名：group_chat
 * 函数功能：客户端群聊的函数
 * 函数形参：char*(用户名)
 * 函数返回值：无
 */
void group_chat(char* name)
{
	packet_t buff;
	memset(&buff,0,sizeof(buff));
	strcpy(buff.username,name);
	printf("＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿\n");
	int retval = -1;
	while(1)
	{
		memset(buff.data,0,sizeof(buff.data));//清空消息
		buff.type = anonymous(TYPE_GROUP);//调用匿名函数
		
		printf("Message(\"exit\" for exit)：");
		fflush(stdout);
		fgets(buff.data,sizeof(buff.data),stdin);

		if(strncmp(buff.data,"exit",4) == 0)
		{
			system("clear");
			break;
		}
		save_message(buff,name);//调用保存消息记录函数
		if((retval = write(client_fd,&buff,sizeof(buff))) < 0)//发送给服务器
		{
			perror("write error");
			break;
		}
		else if(retval == 0)
		{
			printf("Unable to connect to the server\n");
			exit(1);
		}
		pthread_mutex_lock(&mutex);//加锁
		pthread_cond_wait(&cond, &mutex);//加入等待队列,等待返回的发送的状态
		pthread_mutex_unlock(&mutex);//解锁
	}
}
/*
 * 函数名：anonymous
 * 函数功能：匿名函数
 * 函数形参：int(数据包类型)
 * 函数返回值：int
 */
int anonymous(int type)
{
	char choose = 0;
	printf("Anonymous(Y/y)?");
	fflush(stdout);
	scanf("%c",&choose);
	while(getchar()!='\n');
	if((choose == 'Y')||(choose == 'y'))
	{
		return TYPE_ANONYMOUS;//返回数据包匿名类型
	}
	else
	{
		return type;
	}
}
/*
 * 函数名：rettime
 * 函数功能：获取当前时间函数
 * 函数形参：无
 * 函数返回值：char*(当前时间的字符串首地值)
 */
char* rettime(void)
{
	time_t seconds = time(NULL);//time函数获取当前距1970.1.1的秒数
	return ctime(&seconds);//ctime函数将秒数转换为时间日期字符串
}

/*
 * 函数名：getch
 * 函数功能：吸收标准输入一个字符
 * 函数形参：无
 * 函数返回值：无
 */
int getch(void)
{
    struct termios tm, tm_old;//定义两个结构体
    int fd = 0, ch;//定义两个整型变量
 
    if (tcgetattr(fd, &tm) < 0) 
	{//保存现在的终端设置
		return -1;
    }
    tm_old = tm;
    cfmakeraw(&tm);//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    if (tcsetattr(fd, TCSANOW, &tm) < 0) 
	{//设置上更改之后的设置
		return -1;
    }
    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) 
	{//更改设置为最初的样子
		return -1;
    }
    return ch;
}
/*
 * 函数名：hide_passwd
 * 函数功能：接收标准输入，回显*，用于密码隐藏
 * 函数形参：字符型指针变量
 * 函数返回值：无
 */
void hide_passwd(char* passwd)
{		
		int i = 0;
		while(i < 20 && ((passwd[i] = getch()) != '\r'))//i限制密码长度，回车结束输入
		{		
			if((i >= 0) && (passwd[i] == 127))//'\b'删除输入的一个密码，便于重新输入
			{			
				if(i == 0)//删完第一次的输入时，不再向前覆盖，跳出等待输入
				{
					continue;
				}
				else
				{
					i--;
					printf("\b \b");//擦除*
				}
			}
			else
			{
				printf("*");//打印*
				i++;
			}
		}
		passwd[i]  = '\0';//字符数组最后补'\0'
}
/*
 * 函数名：random_codes
 * 函数功能：生成验证码，并对输入的验证码进行比对
 * 函数形参：无
 * 函数返回值：整型
 */
int random_codes(void)
{
	char arr[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";//验证码字符范围
	int i = 0,j = 0,k = 0,a[4] = {};; 
	char temp[5] = "";
	srand((unsigned)time(NULL));//初始化随机数发生器
	for(i = 0;i < 4;i++)	
	{	
		j = rand()%62;//随机数取余取得0-61范围的数字
		a[i] = j;
		temp[i] = arr[j];
		for(k = 0;k < i;k++)//使验证码生成的字符不相同
		{	if(a[i] == a[k])
			{
				i--;
				break;
			}
		}
	}
	printf("\n\t\t验证码：%s\n",temp);//打印验证码
	char data[10] = "";
	printf("\t\t请输入验证码：");//提示输入验证码
	i=0;
	while(i < 4 && ((data[i] = getch()) != '\r'))//验证码回删功能,前三个输错可以回删重新输入，输完第四个验证码时直接跳转验证
	{
		if((i >= 0) && (data[i] == 127))//'\b'删除输入的一个验证码，便于重新输入
		{			
			if(i == 0)//删完第一个后不再往前覆盖，跳出，等待输入
			{
				continue;
			}
			else
			{
				i--;
				printf("\b \b");//擦除输入的验证码
			}
		}
		else
		{
			printf("%c",data[i]);//打印输入的验证码
			i++;
		}
	}
	putchar('\n');
	for(i = 0;i < 4;i++)//遍历验证码的4位
	{
		if((data[i] == temp[i])||(data[i] == temp[i]+32)||(data[i] == temp[i]-32))//使字母不区分大小写
		{
			continue;
		}
		else
		{
			printf("\033\[1A");//光标上移一行
			printf("\033[K");//清除光标之后的整行
			printf("\033\[1A");
			printf("\033[K");
			printf("\033\[1A");
			return 1;//错误返回1
		}	
	}
	return 0;//正确返回0
}

