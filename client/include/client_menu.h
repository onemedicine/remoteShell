/*************************************************************************
    > File Name: client_menu.h
    > Author: yuwz
    > Mail: 1175141208@qq.com 
    > Created Time: 2016年08月31日 星期三 14时06分35秒
 ************************************************************************/

#ifndef CLIENT_MENU_H_
#define CLIENT_MENU_H_

#include"client.h"
/*客户端一级登陆注册菜单控制函数*/
void client_menu(void);
/*客户端一级登录注册菜单界面函数*/
void menu_first(int choose);
/*客户端登陆后新建子线程并调用二级菜单函数*/
void client_submenu(char *name);
/*客户端二级菜单控制函数*/
void submenu(char* name,pthread_t pthread);
/*客户端二级菜单界面函数*/
void menu_second(int choose,char* name);
/*客户端聊天菜单界面函数*/
void chat_menu(char* name);

#endif
