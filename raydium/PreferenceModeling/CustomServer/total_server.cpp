// This is ManiaDrive mini server.

#define RAYDIUM_NETWORK_ONLY
#include "raydium/index.c"

#define PARTY_TIMEOUT           (1*10) // default : 5
#define PARTY_SCORE_TIME        (10)
#define REMAINING_SEND          5


#define NET_SCORE_TRACK (RAYDIUM_NETWORK_PACKET_BASE+1)
#define NET_RESTART     (RAYDIUM_NETWORK_PACKET_BASE+2)
#define NET_CHANGE_MAP  (RAYDIUM_NETWORK_PACKET_BASE+3)
#define NET_REMAINING   (RAYDIUM_NETWORK_PACKET_BASE+4)
#define NET_SCORE_BASE  (RAYDIUM_NETWORK_PACKET_BASE+10)

#define C2S_GAMEINFO_ATTEMPT    (RAYDIUM_NETWORK_PACKET_BASE+5)
#define C2S_GAMEINFO_LEVEL      (RAYDIUM_NETWORK_PACKET_BASE+6)
#define C2S_NEW_COLLECTION      (RAYDIUM_NETWORK_PACKET_BASE+7)
#define C2S_NEW_SESSION         (RAYDIUM_NETWORK_PACKET_BASE+8)
#define C2S_PREFER_RESULT       (RAYDIUM_NETWORK_PACKET_BASE+9)

#define S2C_SEND_LEVELID        (RAYDIUM_NETWORK_PACKET_BASE+10)
#define S2C_SEND_SESSIONID      (RAYDIUM_NETWORK_PACKET_BASE+11)
#define S2C_SEND_SERVER_PORT    (RAYDIUM_NETWORK_PACKET_BASE+12)
#define S2C_SEND_COLLECT_INFO   (RAYDIUM_NETWORK_PACKET_BASE+13)
#define S2C_SESSION_END         (RAYDIUM_NETWORK_PACKET_BASE+14)

//#include "../raydium/PreferenceModeling/DataCollect/DataCollect.h"
#include <tchar.h>


char str[RAYDIUM_NETWORK_PACKET_SIZE];
char track[RAYDIUM_NETWORK_PACKET_SIZE];
float steps=0;
//char str_send[RAYDIUM_NETWORK_PACKET_SIZE];
//int freq;

char *index_text="\
<br/><center>This is a <b>ManiaServer</b>. You can connect to this server using \
<a href=\"http://maniadrive.raydium.org/\">the ManiaDrive game</a>.</center><br/>\
Here, you can find :<br/>\
- <a href=\"data.dyn\">Informations about current party</a><br/>\
- <a href=\"console.dyn\">Server console</a><br/>\
<br>\
- <a href=\"http://maniadrive.raydium.org/index.php?scores=all\">Live scores on the web</a><br/>\
- <a href=\"http://maniadrive.raydium.org/\">Maniadrive website</a><br/>\
- <a href=\"http://raydium.org/\">Game Engine: Raydium website</a><br/>\
";

char *gCollectServerName="collect_server.exe";

void wrong_msg(int type,char *buff)
{
    //raydium_network_broadcast(type,buff);
    raydium_log("error: get game info");
}

//void send_client_sessionid(int n)
//{
//    steps = 0;
//
//    char data[RAYDIUM_NETWORK_PACKET_SIZE];
//    static int id=0;
//    ++id;
//
//    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &id, sizeof(id));
//    raydium_network_write(&raydium_network_client_addr[n],-1, S2C_SEND_SESSIONID, data);
//
//    raydium_log("__________________\nsend session id: %d\n______________________________", id);
//}

void create_collect_server(int n)
{
    PROCESS_INFORMATION info;
    memset(&info, 0, sizeof(PROCESS_INFORMATION));
    STARTUPINFO startInfo;
    memset(&startInfo, 0, sizeof(STARTUPINFO));
    LPPROCESS_INFORMATION pStructInfo = &info;

    //获取当前工作目录，以绝对路径开启updater
    TCHAR fullName[MAX_PATH];
    //取得当前应用程序绝对路径
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL,szFileName,MAX_PATH);
    char szDrive[MAX_PATH]= {0};
    char szDir[MAX_PATH]= {0};
    char szPath[MAX_PATH]= {0};
    _tsplitpath(szFileName,szDrive,szDir,NULL,NULL);
    sprintf(fullName,"%s%s%s",szDrive,szDir,gCollectServerName);

    static unsigned int port = RAYDIUM_NETWORK_BEACON_PORT;
    ++port;
    char szPort[MAX_PATH];
    itoa(++port, szPort, 10);
    if(0 ==CreateProcess(fullName, szPort, NULL, NULL,FALSE, 0, NULL, NULL, &startInfo, pStructInfo))
    {
        raydium_log("create collect_server failed");
    }
    else
    {
#ifndef WIN32
        usleep(5000); // MUST disable this with real servers
#else
        // Now 1ms, trouble detected with Win7 64 bits, where Sleep(0) was
        // making the time stop decreasing correctly ... :/
        Sleep(5000);
#endif

        char data[RAYDIUM_NETWORK_PACKET_SIZE];
        memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &port, sizeof(unsigned int));
        raydium_network_write(&raydium_network_client_addr[n],-1, S2C_SEND_SERVER_PORT, data);

        raydium_log("__________________\nsend session SERVER_PORT: %d\n______________________________", port);
        raydium_log("network: connect_server created for client %i, %s deleted !",n,raydium_network_name[n]);
        raydium_network_client[n]=0;
    }
}


void loop(void)
{
    signed char type;
    int id;

    if(raydium_network_read(&id, &type, str)==RAYDIUM_NETWORK_DATA_OK)
    {
        if(type != C2S_GAMEINFO_ATTEMPT)
        {
            raydium_network_broadcast(type,str);
        }
    }
    raydium_timecall_callback();
    raydium_web_callback();
}

signed char http_req(char *req, char *response, int max_size)
{
    int i;

    raydium_log("-%s-",req);
    if(!strcmp("data.dyn",req))
    {
        sprintf(response,"Party informations\n\
    Running on track <b>%s</b> since <b>%.2f</b> seconds (<b>%.2f</b> left)\
    <br/><br/>",track,steps,PARTY_TIMEOUT-steps);

        sprintf(response+strlen(response),"<table class=\"tables\">\
    <tr><td><div class=\"greenfont\">Connected players (%i max)</div></td></tr>\
    ",RAYDIUM_NETWORK_MAX_CLIENTS);

        for(i=0; i<RAYDIUM_NETWORK_MAX_CLIENTS; i++)
            if(raydium_network_client[i])
                sprintf(response+strlen(response),"<tr><td>%s</td></tr>",raydium_network_name[i]);

        sprintf(response+strlen(response),"</table><br/>");
        sprintf(response+strlen(response),"<center><a href=\"http://maniadrive.raydium.org/index.php?scores=track&track=%s\">\
    You can find bests online scores for '%s' track here !</a></center>\
    ",track,track);
        sprintf(response+strlen(response),"<a href=\"/\">Back</a>");

        return 1;
    }


    if(!strcmp("console.dyn",req))
    {
        char *hist[RAYDIUM_CONSOLE_MAX_LINES];
        int i,n;

        n=raydium_console_history_read(hist);

        sprintf(response,"Server Console\n\
    <i>Raydium console:</i><pre class=\"border_one\">");

        for(i=0; i<n; i++)
            sprintf(response+strlen(response),"%s\n",hist[i]);

        sprintf(response+strlen(response),"</pre>");
        sprintf(response+strlen(response),"<br/><a href=\"/\">Back</a>");
        return 1;
    }

    return 0;
}

void send_tick(GLfloat step)
{
    char buff[RAYDIUM_NETWORK_PACKET_SIZE];
    static int port=RAYDIUM_NETWORK_BEACON_PORT;
    ++port;
    //raydium_log("remaining time : broadcast (%.2f)",tt);
    memcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET,&port,sizeof(int));

    raydium_network_broadcast(S2C_SEND_SERVER_PORT,buff);
}


int main(int argc, char **argv)
{
    int i;
    char title[RAYDIUM_MAX_NAME_LEN];

    raydium_network_only_init(argc,argv);

//    raydium_web_init();
//    raydium_web_start("Total Server");
//    raydium_web_extension_add("mni","raw/unknown",NULL);
//    raydium_web_extension_add("dyn",NULL,http_req); // use default "writer"
//    raydium_web_body_default=index_text;

    raydium_init_cli_option_default("title",title,"ManiaDrive Server. Use --title to change :)");

    raydium_network_server_broadcast(title,"TotalServer",10);

    // Ugly hack, for now...
    raydium_network_netcall_add((void*)wrong_msg,RAYDIUM_NETWORK_PACKET_ODE_EXPLOSION,1);
    raydium_network_netcall_add((void*)wrong_msg,RAYDIUM_NETWORK_PACKET_ODE_NEWELEM,1);
    raydium_network_netcall_add((void*)wrong_msg,RAYDIUM_NETWORK_PACKET_ODE_REMELEM,1);
    raydium_network_netcall_add((void*)wrong_msg,RAYDIUM_NETWORK_PACKET_ODE_NIDWHO,1);
    raydium_network_netcall_add((void*)wrong_msg,NET_SCORE_TRACK,1);
    raydium_network_netcall_add((void*)wrong_msg,NET_RESTART,1);

    raydium_network_on_connect = (void*)create_collect_server;

//    raydium_timecall_add(send_tick,30);
    do
    {
        loop();
#ifndef WIN32
        usleep(1); // MUST disable this with real servers
#else
        // Now 1ms, trouble detected with Win7 64 bits, where Sleep(0) was
        // making the time stop decreasing correctly ... :/
        Sleep(1);
#endif
        //printf(".\n");
    }
    while(1);

}
