/*************************************************************************
    > File Name: client_read.h
    > Author: yuwz
    > Mail: 1175141208@qq.com 
    > Created Time: 2016年08月31日 星期三 14时06分35秒
 ************************************************************************/

#ifndef CLIENT_READ_H_
#define CLIENT_READ_H_

#include"client.h"
/*客户端子线程获得服务器返回信息的函数*/
void* client_receive(void* name);
/*客户端打印服务器返回普通数据类型的函数*/
void printf_buff(void);
/*客户端打印服务器转发的聊天消息函数*/
void printf_msg(char* name,packet_t buff);
/*客户端打印修改密码返回的信息函数*/
void printf_passwd(packet_t buff);

#endif
