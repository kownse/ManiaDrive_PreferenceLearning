#define RAYDIUM_NETWORK_ONLY
#include "raydium/index.h"
//#include "../../raydium/PreferenceModeling/DataCollect/DataCollect.h"
#include "../../raydium/mania_server.h"
#include <vector>
#include "../../raydium/PreferenceModeling/globle.h"
//#include "../../raydium/PreferenceModeling/DataBase/MySqlConnect.h"
#include "pre_db/pre_db.h"

#define PARTY_TIMEOUT           (2*60) // default : 5
#define PARTY_SCORE_TIME        (6)
#define REMAINING_SEND          5


#define NET_SCORE_TRACK (RAYDIUM_NETWORK_PACKET_BASE+1)
#define NET_RESTART     (RAYDIUM_NETWORK_PACKET_BASE+2)
#define NET_CHANGE_MAP  (RAYDIUM_NETWORK_PACKET_BASE+3)
#define NET_REMAINING   (RAYDIUM_NETWORK_PACKET_BASE+4)
#define NET_SCORE_BASE  (RAYDIUM_NETWORK_PACKET_BASE+20)


#define MAX_ATTEMPT 128
#define MAX_LEVEL 8
#define DIRECT_DEBUG

CMySQLSaver mysql_db;

TrackData trackdata;

//c2s_driveinfo_attempt cache_attempt[MAX_ATTEMPT];
//c2s_driveinfo_level cache_level[MAX_LEVEL];
c2s_4afc_result cache_prefer_result;
c2s_user_info cache_user_info;
std::vector<c2s_driveinfo_attempt_original> cache_vec_original;
std::vector<c2s_driveinfo_level> cache_vec_level;
std::vector<c2s_driveinfo_attempt> cache_vec_attempt;
int index_attempt_cache = 0;
int index_level_cache = 0;

int cur_level_id = 0;
int cur_session_id = 0;
unsigned char bWaitResult = 0;
unsigned char bAcceptNewSession = 0;

char str[RAYDIUM_NETWORK_PACKET_SIZE];
char track[RAYDIUM_NETWORK_PACKET_SIZE];
float steps=0;

int nOriginal = 0;
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

void send_db_error(void)
{

}

signed char switch_track(void)
{
    if(bAcceptNewSession)
        return 1;
    else
    {
        bAcceptNewSession = 1;
    }

    static int last=-1;
    FILE *fp;

    raydium_register_variable(track,RAYDIUM_REGISTER_STR,"track");
    raydium_register_variable(&last,RAYDIUM_REGISTER_INT,"last");

    raydium_php_exec("mania_server_tracks.php");

    raydium_register_variable_unregister_last();
    raydium_register_variable_unregister_last();

    if(! (fp=raydium_file_fopen(track,"rt")) )
    {
        raydium_log("ERROR ! cannot switch to track '%s' !",track);
        return 0;
    }
    fclose(fp);
    raydium_log("Switching to track '%s'",track);
    //dat_load(track);
    raydium_network_server_broadcast_info(track);

    return 1;
}

void change_track(char *track)
{
    char buff[RAYDIUM_NETWORK_PACKET_SIZE];

    raydium_log("Broadcasting MNI track '%s'",track);
    strcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET,track);
    raydium_network_broadcast(NET_CHANGE_MAP,buff);

    //static int id=0;
    int i;
    for(i=0; i<RAYDIUM_NETWORK_MAX_CLIENTS; i++)
    {
        if(raydium_network_client[i])
        {
            char data[RAYDIUM_NETWORK_PACKET_SIZE];
            //++id;

            memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &cur_level_id, sizeof(cur_level_id));
            raydium_network_write(&raydium_network_client_addr[i],-1, S2C_SEND_LEVELID, data);
        }
    }
}

void send_time(GLfloat step)
{
    char buff[RAYDIUM_NETWORK_PACKET_SIZE];
    static float t=0;
    float tt;

    t+=step;
    if(t>=REMAINING_SEND)
    {
        t=0;
        tt=PARTY_TIMEOUT-steps;
        //raydium_log("remaining time : broadcast (%.2f)",tt);
        memcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET,&tt,sizeof(float));
        strcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET+sizeof(float),track);
        raydium_network_broadcast(NET_REMAINING,buff);
    }
}

// load associated track data
void dat_load(char *filename)
{
    FILE *fp;
    int ret;
    char var[RAYDIUM_MAX_NAME_LEN];
    char val_s[RAYDIUM_MAX_NAME_LEN];
    float val_f[10];
    int size;
    int done;

    fp=fopen(filename,"rt");

    if(!fp)
    {
        raydium_log("error: cannot open '%s'",filename);
        return;
    }

// reset
    trackdata.name[0]=0;
    trackdata.author[0]=0;
    trackdata.gold_time=0;
    trackdata.author_time=0;
    trackdata.message_file[0]=0;
    trackdata.ent_file[0]=0;

    while( (ret=raydium_parser_read(var,val_s,val_f,&size,fp))!=RAYDIUM_PARSER_TYPE_EOF)
    {
        done=0;

        if(!strcasecmp(var,"name"))
        {
            if(ret!=RAYDIUM_PARSER_TYPE_STRING)
            {
                raydium_log("'name' is wrong type");
                continue;
            }
            strcpy(trackdata.name,val_s);
            done=1;
        }

        if(!strcasecmp(var,"author"))
        {
            if(ret!=RAYDIUM_PARSER_TYPE_STRING)
            {
                raydium_log("'author' is wrong type");
                continue;
            }
            strcpy(trackdata.author,val_s);
            done=1;
        }

        if(!strcasecmp(var,"message"))
        {
            if(ret!=RAYDIUM_PARSER_TYPE_STRING)
            {
                raydium_log("'message' is wrong type");
                continue;
            }
            strcpy(trackdata.message_file,val_s);
            done=1;
        }

        if(!strcasecmp(var,"ent"))
        {
            if(ret!=RAYDIUM_PARSER_TYPE_STRING)
            {
                raydium_log("'ent' is wrong type");
                continue;
            }
            strcpy(trackdata.ent_file,val_s);
            done=1;
        }

        if(!strcasecmp(var,"gold_time"))
        {
            if(ret!=RAYDIUM_PARSER_TYPE_FLOAT || size!=1)
            {
                raydium_log("'gold_time' is wrong type");
                continue;
            }
            trackdata.gold_time=val_f[0];
            done=1;
        }

        if(!strcasecmp(var,"author_time"))
        {
            if(ret!=RAYDIUM_PARSER_TYPE_FLOAT || size!=1)
            {
                raydium_log("'author_time' is wrong type");
                continue;
            }
            trackdata.author_time=val_f[0];
            done=1;
        }

        if(!done)
            raydium_log("invalid statement '%s'",var);

    }
    fclose(fp);
}

/*
void newClient(int client)
{
char buff[RAYDIUM_NETWORK_PACKET_SIZE];
float tt=0;
memcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET,&tt,sizeof(float));
strcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET+sizeof(float),track);
raydium_network_write(&raydium_network_client_addr[client],-1,NET_REMAINING,buff);
}
*/

void change_track_netcall(GLfloat step)
{
    steps+=step;

    if(bWaitResult)
        return;

    if(steps>=(PARTY_TIMEOUT+PARTY_SCORE_TIME))
    {
        if(cur_level_id >= 2)
        {
            int i;
            for(i=0; i<RAYDIUM_NETWORK_MAX_CLIENTS; i++)
            {
                if(raydium_network_client[i])
                {
                    char data[RAYDIUM_NETWORK_PACKET_SIZE];
                    //++id;

                    raydium_network_write(&raydium_network_client_addr[i],-1, S2C_SESSION_END, data);
                }
            }
            bWaitResult = 1;
        }
        else
        {
            steps=0;
            bAcceptNewSession = 0;
            // change map
            if(switch_track())
            {
                change_track(track);
                ++cur_level_id;
            }

        }
    }
}


void broad(int type,char *buff)
{
    raydium_network_broadcast(type,buff);
//    raydium_log("broadcasting %i",type);
}

void collectAttemptInfo(int type, char *buff)
{
    if(type != C2S_GAMEINFO_ATTEMPT)
        return;

    c2s_driveinfo_attempt* pInfo = (c2s_driveinfo_attempt*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET);
    //memcpy(&cache_attempt[index_attempt_cache++], pInfo, sizeof(c2s_driveinfo_attempt));
    cache_vec_attempt.push_back(*pInfo);

    if(pInfo->bFinished == 2)
    {
        //过关了就不用继续游戏了
        steps = PARTY_TIMEOUT;

        float tt=PARTY_TIMEOUT-steps + 0.1;
        //raydium_log("remaining time : broadcast (%.2f)",tt);
        memcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET,&tt,sizeof(float));
        strcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET+sizeof(float),track);
        raydium_network_broadcast(NET_REMAINING,buff);
    }
}

void collectOriginalInfo(int type, char *buff)
{
    if(type != C2S_GAMEINFO_ORIGINAL)
        return;

    c2s_driveinfo_attempt_original* pInfo = (c2s_driveinfo_attempt_original*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET);
    cache_vec_original.push_back(*pInfo);

    //raydium_log("收到速度: %f, %f, %f, %f", pInfo->pLinearSpeed[0], pInfo->pLinearSpeed[1], pInfo->pLinearSpeed[2]);
    //raydium_log("收到原始数据: %d", nOriginal++);
}

void collectUserInfo(int type, char *buff)
{
    if(type != C2S_USER_INFO)
        return;

    c2s_user_info* pInfo = (c2s_user_info*)(buff + RAYDIUM_NETWORK_PACKET_OFFSET);
    raydium_log("用户: name=%s, sex=%d, age=%d, type=%d, frequency=%d", pInfo->name, pInfo->sex, pInfo->age, pInfo->type, pInfo->frequency);
    memcpy(&cache_user_info, pInfo, sizeof(c2s_user_info));
}

void collectLevelInfo(int type, char *buff)
{
    if(type != C2S_GAMEINFO_LEVEL)
        return;

    c2s_driveinfo_level* pInfo = (c2s_driveinfo_level*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET);
    //memcpy(&cache_level[index_level_cache++], pInfo, sizeof(c2s_driveinfo_level));
    cache_vec_level.push_back(*pInfo);
}

void collectPreferenceResult(int type, char *buff)
{
    if(type != C2S_PREFER_RESULT)
        return;

    bAcceptNewSession = 0;

    c2s_4afc_result* pInfo = (c2s_4afc_result*)(buff+RAYDIUM_NETWORK_PACKET_OFFSET);
    memcpy(&cache_prefer_result, pInfo, sizeof(c2s_4afc_result));

    //现在可以保存了
    /*
    db_save_all_MySQL(index_level_cache,
                    &cache_level[0],
                    index_attempt_cache,
                    &cache_attempt[0],
                    (void*)(&cache_vec_original),
                    cache_prefer_result);

    mysql_db.db_save_all_MySQL(index_level_cache,
                    &cache_level[0],
                    index_attempt_cache,
                    &cache_attempt[0],
                    cache_vec_original,
                    cache_prefer_result,
                    cache_user_info);
    */
    mysql_db.save_session_data(cache_vec_attempt,
                               cache_vec_level,
                               cache_vec_original,
                               cache_prefer_result,
                               cache_user_info);

    char data[RAYDIUM_NETWORK_PACKET_SIZE];
    raydium_network_broadcast(S2C_READY_NEW_SESSION, data);
}

void send_client_sessionid(int n)
{
    char data[RAYDIUM_NETWORK_PACKET_SIZE];

    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &cur_session_id, sizeof(cur_session_id));
    ++cur_session_id;
    raydium_network_write(&raydium_network_client_addr[n],-1, S2C_SEND_SESSIONID, data);

    steps=0;
    if(switch_track())
        change_track(track);
}

void newSessionBegin(int type, char *buff)
{
    int i;
    char data[RAYDIUM_NETWORK_PACKET_SIZE];

    cur_level_id=0;
    cur_session_id=0;
    index_attempt_cache = 0;
    index_level_cache = 0;
    bWaitResult = 0;
    //memset(cache_attempt, 0, sizeof(cache_attempt));
    //memset(cache_level, 0, sizeof(cache_level));
    memset(&cache_prefer_result, 0, sizeof(cache_prefer_result));
    cache_vec_attempt.clear();
    cache_vec_level.clear();
    cache_vec_original.clear();

    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &cur_session_id, sizeof(cur_session_id));
    ++cur_session_id;
    for(i=0; i<RAYDIUM_NETWORK_MAX_CLIENTS; i++)
    {
        if(raydium_network_client[i])
        {
            raydium_network_write(&raydium_network_client_addr[i],-1, S2C_SEND_SESSIONID, data);
        }
    }

    steps=0;
    if(switch_track())
    {
        change_track(track);
        ++cur_level_id;
    }
}


void close_server(int n)
{
    raydium_log("\n__________________\nclient_disconnected,server_close\n______________________________");
//    disconnect_db();
    exit(0);
}


void loop(void)
{
    signed char type;
    int id;

    if(raydium_network_read(&id, &type, str)==RAYDIUM_NETWORK_DATA_OK)
    {
        if(type != C2S_GAMEINFO_ATTEMPT && type != C2S_GAMEINFO_LEVEL && type != C2S_GAMEINFO_ORIGINAL)
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

//为了能直接从connect_server开始调试
//需要向客户端发送一个伪端口号
#ifdef DIRECT_DEBUG
void send_debug_port(int n)
{
    int port = RAYDIUM_NETWORK_PORT;
    char data[RAYDIUM_NETWORK_PACKET_SIZE];
    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &port, sizeof(unsigned int));
    raydium_network_write(&raydium_network_client_addr[n],-1, S2C_SEND_SERVER_PORT, data);
    raydium_log("发送DEBUG服务器port: %d", port);
}
#endif

int main(int argc, char **argv)
{
    int i;
    char title[RAYDIUM_MAX_NAME_LEN];
    raydium_network_only_init_port(argc,argv);
    for(i=0; i<argc; ++i)
    {
        raydium_log("\n****************\nargv[%d]: %s\n*****************",i, argv[i]);
    }
    //prelearn_raydium_network_only_init(argc, argv);

    raydium_web_init();
    raydium_web_start("ManiaDrive Server");
    raydium_web_extension_add("mni","raw/unknown",NULL);
    raydium_web_extension_add("dyn",NULL,(void*)http_req); // use default "writer"
    raydium_web_body_default=index_text;

    raydium_init_cli_option_default("title",title,"ManiaDrive Server. Use --title to change :)");

    raydium_network_server_broadcast(title,"ManiaDrive",10);

    // Ugly hack, for now...
    raydium_network_netcall_add((void*)broad,RAYDIUM_NETWORK_PACKET_ODE_EXPLOSION,1);
    raydium_network_netcall_add((void*)broad,RAYDIUM_NETWORK_PACKET_ODE_NEWELEM,1);
    raydium_network_netcall_add((void*)broad,RAYDIUM_NETWORK_PACKET_ODE_REMELEM,1);
    raydium_network_netcall_add((void*)broad,RAYDIUM_NETWORK_PACKET_ODE_NIDWHO,1);
    raydium_network_netcall_add((void*)broad,NET_SCORE_TRACK,1);
    raydium_network_netcall_add((void*)broad,NET_RESTART,1);

    raydium_network_netcall_add((void*)collectAttemptInfo,C2S_GAMEINFO_ATTEMPT,1);
    raydium_network_netcall_add((void*)collectLevelInfo, C2S_GAMEINFO_LEVEL, 1);
    raydium_network_netcall_add((void*)collectOriginalInfo, C2S_GAMEINFO_ORIGINAL, 1);
    raydium_network_netcall_add((void*)newSessionBegin, C2S_NEW_SESSION, 1);
    raydium_network_netcall_add((void*)collectPreferenceResult, C2S_PREFER_RESULT, 1);
    raydium_network_netcall_add((void*)collectUserInfo, C2S_USER_INFO, 1);

    raydium_network_on_disconnect = (void*)close_server;
#ifdef DIRECT_DEBUG
    raydium_network_on_connect = (void*)send_debug_port;
#endif

    for(i=0; i<RAYDIUM_NETWORK_MAX_CLIENTS; i++)
        raydium_network_netcall_add((void*)broad,NET_SCORE_BASE+i,1);


    raydium_timecall_add((void*)change_track_netcall,-1);
    raydium_timecall_add((void*)send_time,-1);

//    connect_db();

    raydium_log_file = fopen( "server_log.txt","w"); //只供读取
    if(raydium_log_file==NULL) //如果失败了
    {
        raydium_log("打开日志文件失败");
    }

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

    fclose(raydium_log_file);
}
