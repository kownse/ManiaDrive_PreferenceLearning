#ifndef COLLECTNET_H_INCLUDED
#define COLLECTNET_H_INCLUDED

#include "../raydium/raydium/common.h"
#include "../../raydium/PreferenceModeling/globle.h"


//�յ���ǰ�ؿ������ݿ��е�id
void prelearn_netcall_receive_levelid(int type, char *buff);
//�յ�session_id
void prelearn_netcall_receive_sessionid(int type, char *buff);
//���ӵ�total_server
void prelearn_connect_total_server();
//�õ�collect_server�Ķ˿ں�
void prelearn_get_connect_server_port(int type, char *buff);
//������session�ź�
void prelearn_send_new_session();
//��ָ��port����collect_server
unsigned char prelearn_connect_collect_server(void);
unsigned char prelearn_connect_collect_server_test(void);

#endif // COLLECTNET_H_INCLUDED
