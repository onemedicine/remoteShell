/*************************************************************************
    > File Name: client.h
    > Author: yuwz
    > Mail: 1175141208@qq.com 
    > Created Time: 2016年08月31日 星期三 14时06分35秒
 ************************************************************************/

#ifndef CLIENT_H_
#define CLIENT_H_
/*终端不同字体颜色宏*/
#define RED printf("\033[31m")
#define GREEN printf("\033[32m")
#define BLUE printf("\033[34m")
#define YELLOW printf("\033[33m")
#define CLOSE printf("\033[0m")
/*数据包结构体*/
typedef struct 
{
	int type;
	char username[20];
	char userpwd[20];
	char chatname[20];
	char data[1024];
}packet_t;
/*数据包类型的宏*/
#define TYPE_REGS 1 //注册
#define TYPE_LOGIN 2//登陆
#define TYPE_SHELL 3//shell
#define TYPE_ONLINE 4//查看在线用户
#define TYPE_CHAT 5//私聊
#define TYPE_HEART 6//心跳(时钟)
#define TYPE_PASSWD 7//修改密码
#define TYPE_GROUP 8//群聊
#define TYPE_ANONYMOUS 9//匿名
/*客户端心跳捕获函数*/
void signal_alrm(int signo);
/*客户端注册函数*/
void client_register(void);
/*客户端登陆函数*/
void client_login(void);
/*客户端修改密码函数*/
void client_passwd(char* name);
/*客户端ｍｙ　ｓｈｅｌｌ函数*/
void client_shell(void);
/*客户端查看在线用户函数*/
void client_online(void);
/*客户端查看消息记录函数*/
void message_record(char* name);
/*客户端保存消息记录函数*/
void save_message(packet_t buff,char* name);
/*客户端私聊函数*/
void client_chat(char* name);
/*客户端群聊函数*/
void group_chat(char* name);
/*客户端群聊匿名函数*/
int anonymous(int type);
/*获得当前时间函数*/
char* rettime(void);
/*从标准输入读取一个字符并隐藏函数*/
int getch(void);
/*客户端隐藏密码函数*/
void hide_passwd(char* passwd);
/*按任意键继续函数*/
void go_on_clear(void);
/*验证码生成并比对函数*/
int random_codes(void);

#endif
