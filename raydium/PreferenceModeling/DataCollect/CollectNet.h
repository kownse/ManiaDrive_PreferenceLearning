#ifndef COLLECTNET_H_INCLUDED
#define COLLECTNET_H_INCLUDED

#include "../raydium/raydium/common.h"
#include "../../raydium/PreferenceModeling/globle.h"


//收到当前关卡在数据库中的id
void prelearn_netcall_receive_levelid(int type, char *buff);
//收到session_id
void prelearn_netcall_receive_sessionid(int type, char *buff);
//连接到total_server
void prelearn_connect_total_server();
//得到collect_server的端口号
void prelearn_get_connect_server_port(int type, char *buff);
//发送新session信号
void prelearn_send_new_session();
//用指定port连接collect_server
unsigned char prelearn_connect_collect_server(void);
unsigned char prelearn_connect_collect_server_test(void);

#endif // COLLECTNET_H_INCLUDED
