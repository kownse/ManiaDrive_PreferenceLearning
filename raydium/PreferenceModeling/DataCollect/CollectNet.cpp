#include "raydium/index.h"
#include "../raydium/PreferenceModeling/DataCollect/CollectNet.h"
#include "../raydium/PreferenceModeling/DataCollect/DataCollect.h"
#include "../raydium/PreferenceModeling/DataCollect/CollectGUI.h"
#include "libintl.h"

//收到自己的数据库id
void prelearn_netcall_receive_levelid(int type, char *buff)
{
    int id = *((int*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET));
    level_id = id;
    levelinfo.session_id = session_id;
    levelinfo.level_id = level_id;

    raydium_log("receive level id: %d", level_id);
}

void prelearn_netcall_receive_sessionid(int type, char *buff)
{
    int id = *((int*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET));
    session_id = id;

    raydium_log("receive session id: %d", session_id);
}

//连接到total_server
void prelearn_connect_total_server()
{

}

//得到connect_server_port
void prelearn_get_connect_server_port(int type, char *buff)
{
    u_short tmp = *((u_short*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET));
    if(tmp >= RAYDIUM_NETWORK_BEACON_PORT)
    {
        server_port = tmp;
        raydium_log("receive COLLECT_SERVER_PORT: %d", server_port);
    }
    else
    {
        raydium_log("receive WRONG_COLLECT_SERVER_PORT: %d", tmp);
        server_port = RAYDIUM_NETWORK_PORT;
    }

    char server[512];
    int handle;

    if(!prelearn_connect_collect_server())
    {
        handle=raydium_gui_window_create("error",25,45,50,15);
        raydium_gui_widget_sizes(0,0,18);
        raydium_gui_label_create("lblError",handle,50,80,gettext("Cannot connect to collect_server ..."),0,0,0);
//        raydium_gui_widget_sizes(15,5,18);
//        raydium_gui_button_create("btnErrorOk",handle,35,15,gettext("OK"),btnErrorOkClick);
        return;
    }
//    prelearn_gameinfo_userinfo_send();
    prelearn_build_gui_New_session();
}

//用指定port连接collect_server
unsigned char prelearn_connect_collect_server(void)
{
    struct sockaddr_in sock;
    int ret,empty;
    char str[RAYDIUM_NETWORK_PACKET_SIZE];
    signed char type;
    struct hostent *server_addr;
    int on=1;

    //先把连接total_server的socket关了
    raydium_network_socket_close(raydium_network_socket);
    raydium_network_mode=RAYDIUM_NETWORK_MODE_NONE;

    if(raydium_network_mode!=RAYDIUM_NETWORK_MODE_NONE)
    {
        raydium_log("network: ERROR: cannot create client : already connected");
        return(0);
    }

    raydium_network_start=time(NULL);
    raydium_network_socket=socket(AF_INET,RAYDIUM_NETWORK_UDP,0);
    if(raydium_network_socket==-1)
    {
        raydium_log("ERROR ! network: cannot create client socket");
        perror("System");
        return(0);
    }
    raydium_log("network: client socket created");


    server_addr = gethostbyname(server_ip);
    if(!server_addr)
    {
        raydium_log("ERROR ! DNS/resolv error with \"%s\"",server_ip);
        perror("System");
        return(0);
    }

//inet_pton(AF_INET,server,&sock.sin_addr);
    memcpy((char*)(&(sock.sin_addr.s_addr)), server_addr->h_addr, server_addr->h_length);
    sock.sin_family=AF_INET;
    sock.sin_port=htons(server_port);

    ret=connect(raydium_network_socket,(struct sockaddr *)&sock,sizeof(sock));
    if(ret)
    {
        raydium_log("ERROR ! local UDP socket error (contacting %s)",server_ip);
        perror("System");
        return(0);
    }
    raydium_log("network: connecting to %s and waiting UID...",server_ip);
    raydium_network_set_socket_block(1);
    setsockopt(raydium_network_socket,SOL_SOCKET,SO_BROADCAST,(char *)&on,sizeof(on));
// needed now, because we use network_write
    raydium_network_mode=RAYDIUM_NETWORK_MODE_CLIENT;
// we need to send request for uid (and send our name)
    strcpy(str+RAYDIUM_NETWORK_PACKET_OFFSET,raydium_network_name_local);
    raydium_network_write(NULL,-1,RAYDIUM_NETWORK_PACKET_REQUEST_UID,str);


// probably needs timeouts, here ...
    if (raydium_network_read(&empty,&type,str)!=RAYDIUM_NETWORK_DATA_OK)
    {
        raydium_network_mode=RAYDIUM_NETWORK_MODE_NONE;
        raydium_log("ERROR ! network: cannot connect to server %s:%d",server_ip, server_port);
        perror("System");
        raydium_network_socket_close(raydium_network_socket);
        return(0);
    }

    if(type==RAYDIUM_NETWORK_PACKET_ATTRIB_UID)
    {
        raydium_network_uid=str[RAYDIUM_NETWORK_PACKET_OFFSET];
        raydium_log("network: accepted as client %i",raydium_network_uid);
        raydium_network_set_socket_block(0);
        strcpy(raydium_network_connected_server,server_ip);
        return(1);
    }

    if(type==RAYDIUM_NETWORK_PACKET_ERROR_NO_MORE_PLACE)
    {
        raydium_network_mode=RAYDIUM_NETWORK_MODE_NONE;
        raydium_log("ERROR ! network: no more room (server said: %s)",str+RAYDIUM_NETWORK_PACKET_OFFSET);
        raydium_network_socket_close(raydium_network_socket);
        return(0);
    }

    raydium_network_mode=RAYDIUM_NETWORK_MODE_NONE;
    raydium_log("ERROR ! network: unknow server message type (%i). abort.",type);
    raydium_network_socket_close(raydium_network_socket);
    return(0);
}

//尝试函数
unsigned char prelearn_connect_collect_server_test(void)
{
//    u_short port = *((u_short*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET));
//    raydium_log("__________________________\nreceive COLLECT_SERVER_PORT: %d\n______________________________", port);
//    if(port < RAYDIUM_NETWORK_BEACON_PORT)
    return NULL;
}
