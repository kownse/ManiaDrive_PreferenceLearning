#include "MySQLSaver.h"
#include "../raydium/PreferenceModeling/utility.h"
#include <time.h>
#include <cstdio>
#include <vector>
#include <assert.h>
#include "pre_analyse/define.h"
#include "pre_analyse/include/COriAnalyserMgr.h"

#define SAFE_CLOSE_STMT(stmt) if(stmt) mysql_stmt_close(stmt);

static const char *prefix = "MySql: ";

CMySQLSaver::CMySQLSaver():
    g_bConnected(-1),
    TEX_LENGTH(128),
    m_szServer("192.168.0.1"),
    m_szUser("root"),
    m_szPass("kingofcs"),
    m_szDB("gameplay_db")
{
    if(connect_db())
        fprintf(stdout,"%sMySQL连接成功\n", prefix);

    ///创建prepared_statment
    if(db_bind_insert_original())
        fprintf(stdout, "%sMySQL准备完成\n", prefix);

    if(db_mysql_init_statistictable())
        fprintf(stdout, "%sLevelStatistic准备完成\n", prefix);

    if(db_mysql_init_trackresult())
        fprintf(stdout, "%sTrackStatistic准备完成\n", prefix);
}

CMySQLSaver::~CMySQLSaver()
{
    //释放返回的结果
    mysql_free_result(m_rsSelectUID);
    mysql_free_result(m_rsSelectLID);
    mysql_free_result(m_rsSelectAID);
    mysql_free_result(m_rsSelectSID);

    //把stmt关闭掉
    SAFE_CLOSE_STMT(m_stInsertOrig);
    SAFE_CLOSE_STMT(m_stSelectUID);
    SAFE_CLOSE_STMT(m_stInsertUID);
    SAFE_CLOSE_STMT(m_stSelectLID);
    SAFE_CLOSE_STMT(m_stInsertLID);
    SAFE_CLOSE_STMT(m_stInsertAID);
    SAFE_CLOSE_STMT(m_stSelectAID);
    SAFE_CLOSE_STMT(m_stSelectSID);
    SAFE_CLOSE_STMT(m_stInsertSID);
    SAFE_CLOSE_STMT(m_stInsertPrefer);

    SAFE_CLOSE_STMT(m_stInsertLS);

    disconnect_db();
    fprintf(stdout,"MySQL关闭");
}

unsigned char CMySQLSaver::connect_db(void)
{
    if (0 != mysql_library_init(0, NULL, NULL))
    {
        return 0;
    }

    if (NULL == mysql_init(&mysql))
    {
        //AfxMessageBox("连接初始化失败");
        return 0;
    }

    if (0 != mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gb2312"))
    {
        //AfxMessageBox("设置字符集错误");
        return 0;
    }


    ////首先连接服务器
    //if (!mysql_real_connect(&mysql, server, user, pass, db,3306, NULL, 0))
    if(!mysql_real_connect(&mysql, m_szServer.c_str(), m_szUser.c_str(), m_szPass.c_str(), m_szDB.c_str(), 3306, NULL, 0))
    {
        g_bConnected = 0;
        return 0;
    }
    else
    {
        //连接服务器成功
        g_bConnected = 1;
        return 1;
    }
    return 1;
}

unsigned char CMySQLSaver::disconnect_db(void)
{
    mysql_close(&mysql);
    return 1;
}

int CMySQLSaver::db_get_attempt_id(int level_id, int attempt_index)
{
    int aid;
    char sqlGetSID[500];
    memset(sqlGetSID, 0, sizeof(sqlGetSID));
    sprintf(sqlGetSID, "select attempt_id from attempt where level_id=\'%d\' and attempt_index=\'%d\'\0",
            level_id,
            attempt_index);

    mysql_real_query(&mysql, sqlGetSID, strlen(sqlGetSID));

    MYSQL_RES *result;
    MYSQL_ROW    row;

    if (!(result=mysql_use_result(&mysql)))
    {
        if(g_bConnected == 1)
            fprintf(stderr,"%smysql lost... %s", prefix, mysql_error(&mysql));
        return 0;
    }
    //找到了记录
    if(row=mysql_fetch_row(result))
    {
        aid = atoi(row[0]);
        mysql_free_result(result);
    }
    else
    {
        fprintf(stderr,"%s找不到aid", prefix);
        aid = -1;
    }

    return aid;
}

int CMySQLSaver::db_get_uid(char* name, char* ip)
{
    int usr_id;
    char sqlGetUID[500];
    memset(sqlGetUID, 0, sizeof(sqlGetUID));
    sprintf(sqlGetUID, "select uid from user where name=\'%s\' and ip=\'%s\'\0",
            name,
            ip);

    mysql_real_query(&mysql, sqlGetUID, strlen(sqlGetUID));

    MYSQL_RES *result;
    MYSQL_ROW    row;

    if (!(result=mysql_use_result(&mysql)))
    {
        if(g_bConnected == 1)
            fprintf(stderr,"%smysql lost... %s", prefix, mysql_error(&mysql));
        return 0;
    }
    //找到了记录
    if(row=mysql_fetch_row(result))
    {
        usr_id = atoi(row[0]);
        mysql_free_result(result);
    }
    //没找到记录，创建新纪录
    else
    {
        mysql_free_result(result);
        char sqlInsert[500];
        memset(sqlInsert, 0, sizeof(sqlInsert));
        sprintf(sqlInsert, "insert into user (name, ip) values (\'%s\',\'%s\')\0", name, ip);
        if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
        {
            if(g_bConnected == 1)
            {
                fprintf(stderr,"%smysql lost... %s", prefix, mysql_error(&mysql));
                return 0;
            }
        }
        else
        {
            //重新获得rec_id
            mysql_real_query(&mysql, sqlGetUID, strlen(sqlGetUID));

            MYSQL_RES *result;
            MYSQL_ROW    row;

            if (!(result=mysql_use_result(&mysql)))
            {
                if(g_bConnected == 1)
                    fprintf(stderr,"%smysql lost... %s", prefix, mysql_error(&mysql));
            }
            //找到了记录
            if(row=mysql_fetch_row(result))
            {
                usr_id = atoi(row[0]);
            }
            else
            {
                if(g_bConnected == 1)
                    fprintf(stderr,"%smysql lost... %s", prefix, mysql_error(&mysql));
            }

            mysql_free_result(result);
        }
    }

    return usr_id;
}

int CMySQLSaver::db_save_get_uid(c2s_user_info &info)
{
    int usr_id = 0;
    char sqlGetUID[512];
    memset(sqlGetUID, 0, sizeof(sqlGetUID));
    sprintf(sqlGetUID, "select uid from user where name=\'%s\' and \
                                                        age=\'%d\' and \
                                                        player_type=\'%d\' and\
                                                        sex=\'%d\' and\
                                                        game_frequncy=\'%d\'\0",
            info.name,
            info.age,
            info.type,
            info.sex,
            info.frequency);

    mysql_real_query(&mysql, sqlGetUID, strlen(sqlGetUID));

    int affected_rows = mysql_stmt_affected_rows(m_stSelectUID);
    if(affected_rows != 1)
    {
        fprintf(stderr, "%sselect UID影响了错误的行数", prefix);
        return -1;
    }

    MYSQL_RES *result;
    MYSQL_ROW    row;

    if (!(result=mysql_use_result(&mysql)))
    {
        if(g_bConnected == 1)
            fprintf(stderr, "%sdb_error:\n%s", prefix, mysql_error(&mysql));
        return 0;
    }
    //找到了记录
    if(row=mysql_fetch_row(result))
    {
        usr_id = atoi(row[0]);
        mysql_free_result(result);
    }
    //没找到记录，创建新纪录
    else
    {
        mysql_free_result(result);
        char sqlInsert[500];
        memset(sqlInsert, 0, sizeof(sqlInsert));
        sprintf(sqlInsert, "insert into user (name, age, player_type, sex, game_frequncy) values (\'%s\',\'%d\', \'%d\',\'%d\',\'%d\')\0",
                info.name,
                info.age,
                info.type,
                info.sex,
                info.frequency);
        if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
        {
            if(g_bConnected == 1)
            {
                fprintf(stderr, "%sdb_error:\n%s", prefix, mysql_error(&mysql));
                return 0;
            }
        }
        else
        {
            //重新获得rec_id
            mysql_real_query(&mysql, sqlGetUID, strlen(sqlGetUID));

            int affected_rows = mysql_stmt_affected_rows(m_stSelectUID);
            if(affected_rows != 1)
            {
                fprintf(stderr, "%sselect UID影响了错误的行数", prefix);
                return -1;
            }

            MYSQL_RES *result;
            MYSQL_ROW    row;

            if (!(result=mysql_use_result(&mysql)))
            {
                if(g_bConnected == 1)
                    fprintf(stderr, "%sdb_error:\n%s", prefix, mysql_error(&mysql));
            }
            //找到了记录
            if(row=mysql_fetch_row(result))
            {
                usr_id = atoi(row[0]);
            }
            else
            {
                if(g_bConnected == 1)
                    fprintf(stderr, "%sdb_error:\n%s", prefix, mysql_error(&mysql));
            }

            mysql_free_result(result);
        }
    }

    return usr_id;
}

int CMySQLSaver::db_save_get_uid_stmt(c2s_user_info &info)
{
    int user_id = db_get_uid_stmt(info);

    if(user_id == 0)
    {
        //新用户
        if(mysql_stmt_execute(m_stInsertUID))
        {
            fprintf(stderr, "%sm_stInsertUID执行失败: %s", prefix, mysql_stmt_error(m_stInsertUID));
            return -1;
        }
        else
        {
            //user_id = db_get_uid_stmt(info);
            user_id = mysql_insert_id(&mysql);
            user_id = (user_id > 0) ? user_id : -1;
        }
    }
    return user_id;
}

int CMySQLSaver::db_get_uid_stmt(c2s_user_info &info)
{
    int usr_id = 0;

    memcpy(&m_user, &info, sizeof(c2s_user_info));
    m_user.name_length = strlen(m_user.name);

    if(mysql_stmt_execute(m_stSelectUID))
    {
        fprintf(stderr, "%sm_stSelectUID执行失败: %s", prefix, mysql_stmt_error(m_stSelectUID));
        return -1;
    }

    if(mysql_stmt_store_result(m_stSelectUID))
    {
        fprintf(stderr, "%sm_stSelectUID store result失败: %s", prefix, mysql_stmt_error(m_stSelectUID));
        return -1;
    }

    //int nRowResult = 0;
    int rc = mysql_stmt_fetch(m_stSelectUID);

    if(rc == 0)
    {
        usr_id = m_user.user_id;
    }
    else if(rc == MYSQL_NO_DATA)
    {
        usr_id = 0;
    }
    else if(rc == 1)
    {
        fprintf(stderr, "%sm_stSelectUID获取结果失败: %s", prefix, mysql_stmt_error(m_stSelectUID));
        usr_id = -1;
    }

    //mysql_free_result(m_rsSelectUID);
    return usr_id;
}

int CMySQLSaver::db_save_get_session_id(int usr_id, int level_id1, int level_id2)
{
    int sid = 0;
    struct tm *newtime;
    time_t ltime;
    time (&ltime);
    newtime = localtime(&ltime);
    char szTime[50];
    sprintf(szTime, "%s\0", asctime(newtime));

    char sqlInsert[500];
    memset(sqlInsert, 0, sizeof(sqlInsert));
    sprintf(sqlInsert, "insert into session (uid, time, level_id_A, level_id_B) values (\'%d\',\'%s\',\'%d\',\'%d\')\0",
            usr_id,
            szTime,
            level_id1,
            level_id2
           );

    if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "%sdb_error:\n%s", prefix, mysql_error(&mysql));
            return -1;
        }
    }
    else
    {
        char sqlGetSID[500];
        int size = sizeof(sqlGetSID);
        memset(sqlGetSID, 0, sizeof(sqlGetSID));
        sprintf(sqlGetSID, "select sid from session where time=\'%s\' and level_id_A=\'%d\' and level_id_B=\'%d\'\0",
                szTime,
                level_id1,
                level_id2);
        if(mysql_real_query(&mysql, sqlGetSID, strlen(sqlGetSID)) != 0)
        {
            if(g_bConnected == 1)
            {
                fprintf(stderr, "%sdb_error:\n%s", prefix, mysql_error(&mysql));
                return -1;
            }
        }

        MYSQL_RES *result;
        MYSQL_ROW    row;

        if (!(result=mysql_use_result(&mysql)))
        {
            if(g_bConnected == 1)
                fprintf(stderr, "%sdb_error: %s", prefix, mysql_error(&mysql));
        }
        //找到了记录
        if(row=mysql_fetch_row(result))
        {
            sid = atoi(row[0]);
        }
        else
        {
            if(g_bConnected == 1)
                fprintf(stderr, "%sdb_error: %s", prefix, mysql_error(&mysql));
        }

        mysql_free_result(result);
        return sid;
    }
    return -1;
}

int CMySQLSaver::db_save_get_session_id_stmt(session_info &session)
{
    int sid = -1;

    if(!db_save_session_id_stmt(session))
    {
        //现在可以得到session_id了
        //sid = db_get_session_id_stmt(session);
        sid = mysql_insert_id(&mysql);

    }
    else
    {
        sid = -1;
    }
    return sid;
}

int CMySQLSaver::db_save_session_id_stmt(session_info &session)
{
    memcpy(&m_session, &session, sizeof(session_info));

    if(mysql_stmt_execute(m_stInsertSID))
    {
        fprintf(stderr, "%s执行m_stInsertSID失败: %s", prefix, mysql_stmt_error(m_stInsertSID));
        return -1;
    }
    else
    {
        return 0;
    }
}

int CMySQLSaver::db_get_session_id_stmt(session_info &session)
{
    int sid = 0;

    memcpy(&m_session, &session, sizeof(session_info));

    if(mysql_stmt_execute(m_stSelectSID))
    {
        fprintf(stderr, "%sm_stSelectSID执行失败: %s", prefix, mysql_stmt_error(m_stSelectSID));
        return -1;
    }

    if(mysql_stmt_store_result(m_stSelectSID))
    {
        fprintf(stderr, "%sm_stSelectSID store result失败: %s", prefix, mysql_stmt_error(m_stSelectSID));
        return -1;
    }

    //int nRowResult = 0;
    int rc = mysql_stmt_fetch(m_stSelectSID);

    if(rc == 0)
    {
        sid = m_session.session_id;
    }
    else if(rc == MYSQL_NO_DATA)
    {
        sid = 0;
    }
    else if(rc == 1)
    {
        fprintf(stderr, "%sm_stSelectSID获取结果失败: %s", prefix, mysql_stmt_error(m_stSelectSID));
        sid = -1;
    }

    //mysql_free_result(m_rsSelectLID);
    return sid;
}

int CMySQLSaver::db_save_get_level_id(c2s_driveinfo_level &level)
{
    int lid = 0;
    char sqlInsert[1024];
    memset(sqlInsert, 0, sizeof(sqlInsert));
    sprintf(sqlInsert,
            //"insert into level (attempt_time, collision_time, leap_time, forward_time, back_time, turn_left_time, turn_right_time, forward_left_time, forward_right_time, back_left_time, back_right_time, no_control_time, collision_no_control_time, leap_no_control_time, finish_proportion, number_attempt, num_finished, record_time, track_name) values (\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%d\',\'%d\',\'%ld\',\'%s\')\0",
            "insert into level (party_time, finish_proportion, num_attempt, num_finished, record_time, track_name) values (\'%f\',\'%f\',\'%d\',\'%d\',\'%ld\',\'%s\')\0",
            level.partytime,
            level.finishProportionaTotal,
            level.num_attempt,
            level.num_finished,
            level.time_record,
            level.trackname);
    int len = strlen(sqlInsert);
//    raydium_log(sqlInsert);
    if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "%sdb error: %s", prefix, mysql_error(&mysql));
            return 0;
        }
    }
    else
    {
        char sqlGetLID[500];
        int size = sizeof(sqlGetLID);
        memset(sqlGetLID, 0, sizeof(sqlGetLID));
        sprintf(sqlGetLID, "select level_id from level where record_time=\'%ld\' and num_attempt=\'%d\' and num_finished=\'%d\'\0",
                level.time_record,
                level.num_attempt,
                level.num_finished);
        if(mysql_real_query(&mysql, sqlGetLID, strlen(sqlGetLID)) != 0)
        {
            if(g_bConnected == 1)
            {
                fprintf(stderr, "%sdb_error:\n%s", prefix, mysql_error(&mysql));
                return 0;
            }
        }

        MYSQL_RES *result;
        MYSQL_ROW    row;

        if (!(result=mysql_use_result(&mysql)))
        {
            if(g_bConnected == 1)
                fprintf(stderr, "db_error: %s", mysql_error(&mysql));
        }
        //找到了记录
        if(row=mysql_fetch_row(result))
        {
            lid = atoi(row[0]);
        }
        else
        {
            if(g_bConnected == 1)
                fprintf(stderr, "db_error: %s", mysql_error(&mysql));
        }

        mysql_free_result(result);
        return lid;
    }

    return(0);
}

int CMySQLSaver::db_save_get_level_id_stmt(c2s_driveinfo_level &level)
{
    int lid = 0;

    memcpy(&m_level, &level, sizeof(c2s_driveinfo_level));
    m_level.trackname_length = strlen(m_level.trackname);

    if(mysql_stmt_execute(m_stInsertLID))
    {
        fprintf(stderr, "执行m_stInsertLID失败: %s", mysql_stmt_error(m_stInsertLID));
        return -1;
    }
    else
    {
        //lid = db_get_level_id_stmt(level);
        lid = mysql_insert_id(&mysql);


        if(lid > 0)
        {
            return lid;
        }
        else
        {
            fprintf(stderr, "插入level有误");
            return -1;
        }
    }
    return -1;
}

int CMySQLSaver::db_save_level_id_stmt(c2s_driveinfo_level &lLevel)
{
    return -1;
}

int CMySQLSaver::db_get_level_id_stmt(c2s_driveinfo_level &level)
{
    int lid = 0;

    memcpy(&m_level, &level, sizeof(c2s_driveinfo_level));
    m_level.trackname_length = strlen(m_level.trackname);

    if(mysql_stmt_execute(m_stSelectLID))
    {
        fprintf(stderr, "m_stSelectLID执行失败: %s", mysql_stmt_error(m_stSelectLID));
        return -1;
    }

    if(mysql_stmt_store_result(m_stSelectLID))
    {
        fprintf(stderr, "m_stSelectLID store result失败: %s", mysql_stmt_error(m_stSelectLID));
        return -1;
    }

    //int nRowResult = 0;
    int rc = mysql_stmt_fetch(m_stSelectLID);

    if(rc == 0)
    {
        lid = m_level.level_id;
    }
    else if(rc == MYSQL_NO_DATA)
    {
        lid = 0;
    }
    else if(rc == 1)
    {
        fprintf(stderr, "m_stSelectLID获取结果失败: %s", mysql_stmt_error(m_stSelectLID));
        lid = -1;
    }

    //mysql_free_result(m_rsSelectLID);
    return lid;
}

int CMySQLSaver::db_get_attempt_id_cache(int level_id, int attempt_index)
{
    //用这个判断来减少读取aid的次数
    static int last_lid;
    static int last_attemp_index;
    static int last_aid;
    if(last_lid != level_id || last_attemp_index != attempt_index)
    {
        last_aid = db_get_attempt_id(level_id, attempt_index);
        last_lid = level_id;
        last_attemp_index = attempt_index;
    }

    return last_aid;
}

int CMySQLSaver::db_get_attempt_id_stmt(c2s_driveinfo_attempt_original &original)
{
    static int last_lid;
    static int last_attemp_index;
    static int last_aid;

    if(last_lid != original.nLevel_id || last_attemp_index != original.nAttempIndex)
    {
        memcpy(&m_original, &original, sizeof(c2s_driveinfo_attempt_original));

        if(mysql_stmt_execute(m_stSelectAID))
        {
            fprintf(stderr, "m_stSelectAID执行失败: %s\n", mysql_stmt_error(m_stSelectAID));
            return -1;
        }

        if(mysql_stmt_store_result(m_stSelectAID))
        {
            fprintf(stderr, "m_stSelectAID store result失败: %s", mysql_stmt_error(m_stSelectAID));
            return -1;
        }

        //int nRowResult = 0;
        int rc = mysql_stmt_fetch(m_stSelectAID);

        if(rc == 0)
        {
            last_aid = m_attempt.attempt_id;
        }
        else if(rc == MYSQL_NO_DATA)
        {
            last_aid = -1;
        }
        else if(rc == 1)
        {
            fprintf(stderr, "m_stSelectAID获取结果失败: %s", mysql_stmt_error(m_stSelectAID));
            last_aid = -1;
        }

        last_lid = original.nLevel_id;
        last_attemp_index = original.nAttempIndex;
    }

    return last_aid;
}

int CMySQLSaver::db_save_attemp_info(int level_id, c2s_driveinfo_attempt* pAttempt)
{
    int lid = 0;
    char sqlInsert[1000];
    memset(sqlInsert, 0, sizeof(sqlInsert));
    sprintf(sqlInsert,
            //"insert into attempt (attempt_time, collision_time, leap_time, forward_time, back_time, turn_left_time, turn_right_time, forward_left_time, forward_right_time, back_left_time, back_right_time, no_control_time, collision_no_control_time, leap_no_control_time, avg_speed, max_speed, std_var_speed, finish_proportion, attempt_index, finished, level_id, record_time, ori_total) values (\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%d\',\'%d\',\'%d\',\'%ld\', \'%d\')\0",
            "insert into attempt (attempt_time, finish_proportion, attempt_index, finished, level_id, record_time, ori_total) values (\'%f\',\'%f\',\'%d\',\'%d\',\'%d\',\'%ld\', \'%d\')\0",
            pAttempt->fTotaltime,
            pAttempt->finishProportion,
            pAttempt->attemp_index,
            pAttempt->bFinished,
            level_id,
            pAttempt->time_record,
            pAttempt->nOriginalTotal);
    if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "db_error: %s", mysql_error(&mysql));
            return(0);
        }
    }

    return(0);
}

int CMySQLSaver::db_save_attemp_info(c2s_driveinfo_attempt* pAttempt)
{
    char sqlInsert[1000];
    memset(sqlInsert, 0, sizeof(sqlInsert));
    sprintf(sqlInsert,
            //"insert into attempt (attempt_time, collision_time, leap_time, forward_time, back_time, turn_left_time, turn_right_time, forward_left_time, forward_right_time, back_left_time, back_right_time, no_control_time, collision_no_control_time, leap_no_control_time, avg_speed, max_speed, std_var_speed, finish_proportion, attempt_index, finished, level_id, record_time, ori_total) values (\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%d\',\'%d\',\'%d\',\'%ld\', \'%d\')\0",
            "insert into attempt (attempt_time, finish_proportion, attempt_index, finished, level_id, record_time, ori_total) values (\'%f\',\'%f\',\'%d\',\'%d\',\'%d\',\'%ld\', \'%d\')\0",
            pAttempt->fTotaltime,
            pAttempt->finishProportion,
            pAttempt->attemp_index,
            pAttempt->bFinished,
            pAttempt->level_id,
            pAttempt->time_record,
            pAttempt->nOriginalTotal);
    if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "db_error: %s", mysql_error(&mysql));
            return(0);
        }
    }

    return(0);
}

int CMySQLSaver::db_save_attempt_info_stmt(c2s_driveinfo_attempt &attempt)
{
    memcpy(&m_attempt, &attempt, sizeof(c2s_driveinfo_attempt));

    if(mysql_stmt_execute(m_stInsertAID))
    {
        fprintf(stderr, "执行m_stInsertAID失败: %s", mysql_stmt_error(m_stInsertAID));
        return -1;
    }
    else
    {
        return 0;
    }
}

int CMySQLSaver::db_save_original_info(c2s_driveinfo_attempt_original* pOriginal)
{
    //首先通过original的attempt_index和level_id找出attempt在数据中的ID
    char sqlInsert[1000];
    memset(sqlInsert, 0, sizeof(sqlInsert));
    sprintf(sqlInsert,
            "insert into original (level_id, attempt_id, attempt_index, ori_index, lspdX, lspdY, lspdZ, aspdX, aspdY, aspdZ, rotX, rotY, rotZ, dirX, dirY, dirZ, posX, posY, posZ, time, step, motor_speed, ray_f, ray_b, ray_l, ray_r, ray_fl, ray_fr, ray_up, ray_down, num_wheel, fwd, bak, turn_left, turn_right, leap, collision) values (\'%d\',\'%d\',\'%d\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\')\0",
            //"insert into original (level_id, attempt_id, time, step, motor_speed, fwd, bak, left, right, leap, num_col_wheel, collision) values (\'%d\',\'%d\',\'%f\',\'%f\',\'%f\',\'%d\',\'%d\', \'%d\',\'%d\',\'%d\',\'%d\',\'%d\')\0",
            //"insert into original (level_id, attempt_id, time, step, motor_speed) values (\'%d\',\'%d\',\'%f\',\'%f\',\'%f\')\0",
            //"insert into original (level_id, attempt_id, num_wheel, fwd, bak, left, right, leap, collision) values (\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\')\0",
            //"insert into original (level_id, attempt_id, turn_left, turn_right) values (\'%d\',\'%d\',\'%d\',\'%d\')\0",
            pOriginal->nLevel_id,
            pOriginal->nAttempt_id,
            pOriginal->nAttempIndex,
            pOriginal->nOriginalIndex,
            pOriginal->pLinearSpeed[0],
            pOriginal->pLinearSpeed[1],
            pOriginal->pLinearSpeed[2],
            pOriginal->pAngSpeed[0],
            pOriginal->pAngSpeed[1],
            pOriginal->pAngSpeed[2],
            pOriginal->pRotate[0],
            pOriginal->pRotate[1],
            pOriginal->pRotate[2],
            pOriginal->pDirection[0],
            pOriginal->pDirection[1],
            pOriginal->pDirection[2],
            pOriginal->pPosition[0],
            pOriginal->pPosition[1],
            pOriginal->pPosition[2],

            pOriginal->fTime,
            pOriginal->fStep,
            pOriginal->fMotorSpeed,

            pOriginal->ray_f,
            pOriginal->ray_b,
            pOriginal->ray_l,
            pOriginal->ray_r,
            pOriginal->ray_fl,
            pOriginal->ray_fr,
            pOriginal->ray_up,
            pOriginal->ray_down,

            pOriginal->nColWhell,
            pOriginal->bForward,
            pOriginal->bBack,

            pOriginal->bTurnLeft,
            pOriginal->bTurnRight,

            pOriginal->bLeap,
            pOriginal->bCollision

           );
    if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
    {
        if(g_bConnected == 1)
        {
            //raydium_log("db_error:\n%s", sqlInsert);
            fprintf(stderr, "db_error: %s", mysql_error(&mysql));
            return(0);
        }
    }
    return(0);
}

int CMySQLSaver::db_save_original_info_transaction(c2s_driveinfo_attempt_original* pOriginal)
{
    char sqlInsert[1000];
    memset(sqlInsert, 0, sizeof(sqlInsert));
    sprintf(sqlInsert,
            "insert into original (level_id, attempt_id, attempt_index, ori_index, lspdX, lspdY, lspdZ, aspdX, aspdY, aspdZ, rotX, rotY, rotZ, dirX, dirY, dirZ, posX, posY, posZ, time, step, motor_speed, ray_f, ray_b, ray_l, ray_r, ray_fl, ray_fr, ray_up, ray_down, num_wheel, fwd, bak, turn_left, turn_right, leap, collision) values (\'%d\',\'%d\',\'%d\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\')\0",
            pOriginal->nLevel_id,
            pOriginal->nAttempt_id,
            pOriginal->nAttempIndex,
            pOriginal->nOriginalIndex,
            pOriginal->pLinearSpeed[0],
            pOriginal->pLinearSpeed[1],
            pOriginal->pLinearSpeed[2],
            pOriginal->pAngSpeed[0],
            pOriginal->pAngSpeed[1],
            pOriginal->pAngSpeed[2],
            pOriginal->pRotate[0],
            pOriginal->pRotate[1],
            pOriginal->pRotate[2],
            pOriginal->pDirection[0],
            pOriginal->pDirection[1],
            pOriginal->pDirection[2],
            pOriginal->pPosition[0],
            pOriginal->pPosition[1],
            pOriginal->pPosition[2],

            pOriginal->fTime,
            pOriginal->fStep,
            pOriginal->fMotorSpeed,

            pOriginal->ray_f,
            pOriginal->ray_b,
            pOriginal->ray_l,
            pOriginal->ray_r,
            pOriginal->ray_fl,
            pOriginal->ray_fr,
            pOriginal->ray_up,
            pOriginal->ray_down,

            pOriginal->nColWhell,
            pOriginal->bForward,
            pOriginal->bBack,

            pOriginal->bTurnLeft,
            pOriginal->bTurnRight,

            pOriginal->bLeap,
            pOriginal->bCollision

           );
    if (mysql_query(&mysql, sqlInsert)!=0)
    {
        if(g_bConnected == 1)
        {
            //raydium_log("db_error:\n%s", sqlInsert);
            fprintf(stderr, "db_error: %s", mysql_error(&mysql));
            //回滚操作
            mysql_query(&mysql, "ROLLBACK ");
            return(0);
        }
    }
    return(0);
}

int CMySQLSaver::db_save_original_info_stmt(c2s_driveinfo_attempt_original* pOriginal)
{
    memcpy(&m_original, pOriginal, sizeof(c2s_driveinfo_attempt_original));
    if(mysql_stmt_execute(m_stInsertOrig))
    {
        fprintf(stderr, "Original插入stmt失败: %s", mysql_error(&mysql));
        return -1;
    }

    int affected_rows = mysql_stmt_affected_rows(m_stInsertOrig);
    if(affected_rows != 1)
    {
        fprintf(stderr, "insert_original_stmt影响了错误的行数");
        return -1;
    }

    return 0;
}

//保存偏好结果
int CMySQLSaver::db_save_prefer_result(int sid, int type_id, int result)
{
    assert(sid > 0 && "sid错误");

    int pid = 0;
    char sqlInsert[1000];
    memset(sqlInsert, 0, sizeof(sqlInsert));
    sprintf(sqlInsert,
            "insert into prefer_result (sid, prefer_type, prefer_result) values (\'%d\',\'%d\',\'%d\')\0",
            sid,
            type_id,
            result);
    if (mysql_real_query(&mysql, sqlInsert, strlen(sqlInsert))!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "db_error: %s", mysql_error(&mysql));
            return(0);
        }
    }
}

int CMySQLSaver::db_save_prefer_result_stmt(c2s_4afc_result &prefer)
{
    memcpy(&m_prefer, &prefer, sizeof(c2s_4afc_result));

    if(mysql_stmt_execute(m_stInsertPrefer))
    {
        fprintf(stderr, "执行m_stInsertPrefer失败: %s", mysql_stmt_error(m_stInsertPrefer));
        return -1;
    }
    else
    {
        return 0;
    }
}

int CMySQLSaver::save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                                   std::vector<c2s_driveinfo_level> &levelStore,
                                   std::vector<c2s_driveinfo_attempt_original> &originalStore,
                                   c2s_4afc_result &prefer,
                                   c2s_user_info &user)
{
    assert(true && "没有实现");
    return -1;
}
//将所有信息存入mysql中
int CMySQLSaver::save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                                   std::vector<c2s_driveinfo_level> &levelStore,
                                   std::vector<c2s_driveinfo_attempt_original> &originalStore,
                                   c2s_4afc_result &prefer,
                                   c2s_user_info &user,
                                   session_info &sessionTmp)
{
    if(levelStore.size() != 2)
        return PRE_DB_ERROR;

    bool bSuccess = true;
    //用事务来存储
    if(mysql_query(&mysql, "BEGIN "))
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "mysql建立事务失败: %s\n", mysql_error(&mysql));
            //return(0);
        }
    }
    else
    {
        //fprintf(stderr, "mysql事务开启成功");
    }

    //int user_id = db_save_get_uid(user);
    int user_id = db_save_get_uid_stmt(user);
    if(user_id < 0)
    {
        bSuccess = false;
    }
    //assert(user_id != 0 && "用户ID获取错误");

    //写入level信息
    //int level_id1 = db_save_get_level_id(levelStore[0]);
    //int level_id2 = db_save_get_level_id(levelStore[1]);
    int level_id1 = db_save_get_level_id_stmt(levelStore[0]);
    int level_id2 = db_save_get_level_id_stmt(levelStore[1]);

    if(level_id1 < 0 || level_id2 < 0)
    {
        bSuccess = false;
    }

    //写入attempt信息
    std::vector<c2s_driveinfo_attempt>::iterator ita;
    for(ita = attemptStore.begin(); ita != attemptStore.end(); ++ita)
    {
        c2s_driveinfo_attempt *pAttempt = &(*ita);
        if(pAttempt->session_id != 0)
        {
            //出错
            continue;
        }

        if(pAttempt->level_id == 0)
        {
            //db_save_attemp_info(level_id1, pAttempt);
            pAttempt->level_id = level_id1;
        }
        else if(pAttempt->level_id == 1)
        {
            pAttempt->level_id = level_id2;
        }
        else
        {
            //出错
            continue;
        }

        //db_save_attemp_info(pAttempt);
        if(db_save_attempt_info_stmt(*pAttempt) < 0)
        {
            bSuccess = false;
            break;
        }
    }

    COriAnalyserMgr level1Analyser;
    COriAnalyserMgr level2Analyser;

    ///修改original中的level_id和attempt_id，以便后续集体存入数据库
    std::vector<c2s_driveinfo_attempt_original>::iterator it;
    for(it = originalStore.begin(); it != originalStore.end(); ++it)
    {
        c2s_driveinfo_attempt_original *pOrig = &(*it);
        //int curLID;

        if(pOrig->nLevel_id == 0)
        {
            //curLID = level_id1;
            pOrig->nLevel_id = level_id1;

            level1Analyser.DoAnalyse(*pOrig);
        }
        else if(pOrig->nLevel_id == 1)
        {
            //curLID = level_id2;
            pOrig->nLevel_id = level_id2;

            level2Analyser.DoAnalyse(*pOrig);
        }

        //由于这一步骤的存在，很难用于事务
        //pOrig->nAttempIndex = db_get_attempt_id_cache(pOrig->nLevel_id, pOrig->nAttempIndex);
        pOrig->nAttempt_id = db_get_attempt_id_stmt(*pOrig);

        if(pOrig->nAttempt_id < 0)
        {
            bSuccess = false;
            break;
        }
    }


    CAnalyseTypeResult result1;
    level1Analyser.GetStaticResult(result1);
    level1Analyser.Reset();
    result1.id = level_id1;
    /*
    result1.partytime = levelStore[0].partytime;
    result1.finishProportionaTotal = levelStore[0].partytime;
    result1.num_attempt = levelStore[0].num_attempt;
    result1.num_finished = levelStore[0].num_finished;
    result1.difficult = levelStore[0].difficult;
    result1.skill = levelStore[0].skill;
    result1.emotion = levelStore[0].emotion;
    result1.fun = levelStore[0].fun;
    result1.emotion_degree = levelStore[0].emotion_degree;
    */

    int size1 = sizeof(result1);
    memset(&m_result, 0, sizeof(m_result));
    memcpy(&m_result, &result1, sizeof(CAnalyseTypeResult));
    if(!db_save_statistic_stmt())
        fprintf(stderr, "保存赛道1的统计结果失败");

    CAnalyseTypeResult result2;
    level2Analyser.GetStaticResult(result2);
    level2Analyser.Reset();
    result2.id = level_id2;
    /*
    result2.partytime = levelStore[1].partytime;
    result2.finishProportionaTotal = levelStore[1].partytime;
    result2.num_attempt = levelStore[1].num_attempt;
    result2.num_finished = levelStore[1].num_finished;
    result2.difficult = levelStore[1].difficult;
    result2.skill = levelStore[1].skill;
    result2.emotion = levelStore[1].emotion;
    result2.fun = levelStore[1].fun;
    result2.emotion_degree = levelStore[1].emotion_degree;
    */

    int size2 = sizeof(result2);
    memset(&m_result, 0, sizeof(m_result));
    memcpy(&m_result, &result2, sizeof(CAnalyseTypeResult));
    if(!db_save_statistic_stmt())
        fprintf(stderr, "保存赛道2的统计结果失败");

    ///original的批存储
    for(it = originalStore.begin(); it != originalStore.end(); ++it)
    {
        c2s_driveinfo_attempt_original *pOrig = &(*it);
        //db_save_original_info_transaction(pOrig);
        if(db_save_original_info_stmt(pOrig) < 0)
        {
            bSuccess = false;
            break;
        }
    }

    //存入session
    session_info session;
    memset(&session, 0, sizeof(session_info));

    session.user_id = user_id;
    session.level_id1 = level_id1;
    session.level_id2 = level_id2;
    /*
    struct tm *newtime;
    time_t ltime;
    time (&ltime);
    newtime = localtime(&ltime);
    sprintf(session.time, "%s\0", asctime(newtime));
    */
    sprintf(session.time, "%s\0", sessionTmp.time);
    session.time_length = strlen(session.time);

    //int sid = db_save_get_session_id(user_id, level_id1, level_id2);
    prefer.sid = db_save_get_session_id_stmt(session);

    if(prefer.sid == 0)
    {
        fprintf(stderr, "获取sid错误");
        bSuccess = false;
    }
    //db_save_prefer_result(sid, prefer.prefer_index, prefer.result);
    if(db_save_prefer_result_stmt(prefer) < 0)
    {
        bSuccess = false;
    }

    //提交事务
    if(bSuccess)
    {
        if(mysql_query(&mysql, "COMMIT "))
        {
            if(g_bConnected == 1)
            {
                fprintf(stderr, "mysql提交事务失败: %s\n", mysql_error(&mysql));
                //return(0);
            }
        }
        else
        {
            //fprintf(stderr, "mysql事务提交成功");
        }
    }
    else
    {
        if(mysql_query(&mysql, "ROLLBACK "))
        {
            if(g_bConnected == 1)
            {
                fprintf(stderr, "mysql回滚事务失败: %s\n", mysql_error(&mysql));
            }
        }
        else
        {
            fprintf(stderr, "mysql回滚事务成功: %s\n", mysql_error(&mysql));
        }
        return false;
    }


    return PRE_DB_OK;
}

bool CMySQLSaver::db_bind_insert_original(void)
{
    ///insert original
    {
        static const char *insertSQL = "insert into original (level_id, attempt_id, attempt_index, ori_index,\
                                                        lspdX, lspdY, lspdZ, \
                                                        aspdX, aspdY, aspdZ, \
                                                        rotX, rotY, rotZ, \
                                                        dirX, dirY, dirZ, \
                                                        posX, posY, posZ, \
                                                        time, step, motor_speed, \
                                                        ray_f, ray_b, ray_l, ray_r, ray_fl, ray_fr, ray_up, ray_down, \
                                                        num_wheel, \
                                                        fwd, bak, turn_left, turn_right, leap, collision, track_type) values (\
                                                        ?,?,?,?,\
                                                        ?,?,?,\
                                                        ?,?,?,\
                                                        ?,?,?,\
                                                        ?,?,?,\
                                                        ?,?,?,\
                                                        ?,?,?,\
                                                        ?,?,?,?,?,?,?,?,\
                                                        ?,\
                                                        ?,?,?,?,?,?, ?)\0";
        db_mysql_stmt_init(&m_stInsertOrig, insertSQL, "insertOriginal");

        const int PARAM_NUM = 38;


        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///level_id
        bind[0].buffer_type = MYSQL_TYPE_LONG;
        bind[0].buffer = (char*)&m_original.nLevel_id;
        bind[0].is_null = 0;
        bind[0].length = 0;

        ///attempt_id
        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char*)&m_original.nAttempt_id;
        bind[1].is_null = 0;
        bind[1].length = 0;

        ///attempt_index
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_original.nAttempIndex;
        bind[2].is_null = 0;
        bind[2].length = 0;

        ///ori_index
        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = (char*)&m_original.nOriginalIndex;
        bind[3].is_null = 0;
        bind[3].length = 0;

        ///lspdX
        bind[4].buffer_type = MYSQL_TYPE_FLOAT;
        bind[4].buffer = (char*)(&(m_original.pLinearSpeed[0]));
        bind[4].is_null = 0;
        bind[4].length = 0;

        ///lspdY
        bind[5].buffer_type = MYSQL_TYPE_FLOAT;
        bind[5].buffer = (char*)(&(m_original.pLinearSpeed[1]));
        bind[5].is_null = 0;
        bind[5].length = 0;

        ///lspdZ
        bind[6].buffer_type = MYSQL_TYPE_FLOAT;
        bind[6].buffer = (char*)(&(m_original.pLinearSpeed[2]));
        bind[6].is_null = 0;
        bind[6].length = 0;

        ///aspdX
        bind[7].buffer_type = MYSQL_TYPE_FLOAT;
        bind[7].buffer = (char*)(&(m_original.pAngSpeed[0]));
        bind[7].is_null = 0;
        bind[7].length = 0;

        ///aspdY
        bind[8].buffer_type = MYSQL_TYPE_FLOAT;
        bind[8].buffer = (char*)(&(m_original.pAngSpeed[1]));
        bind[8].is_null = 0;
        bind[8].length = 0;

        ///aspdZ
        bind[9].buffer_type = MYSQL_TYPE_FLOAT;
        bind[9].buffer = (char*)(&(m_original.pAngSpeed[2]));
        bind[9].is_null = 0;
        bind[9].length = 0;

        ///rotX
        bind[10].buffer_type = MYSQL_TYPE_FLOAT;
        bind[10].buffer = (char*)(&(m_original.pRotate[0]));
        bind[10].is_null = 0;
        bind[10].length = 0;

        ///rotY
        bind[11].buffer_type = MYSQL_TYPE_FLOAT;
        bind[11].buffer = (char*)(&(m_original.pRotate[1]));
        bind[11].is_null = 0;
        bind[11].length = 0;

        ///rotY
        bind[12].buffer_type = MYSQL_TYPE_FLOAT;
        bind[12].buffer = (char*)(&(m_original.pRotate[2]));
        bind[12].is_null = 0;
        bind[12].length = 0;

        ///dirX
        bind[13].buffer_type = MYSQL_TYPE_FLOAT;
        bind[13].buffer = (char*)(&(m_original.pDirection[0]));
        bind[13].is_null = 0;
        bind[13].length = 0;

        ///dirY
        bind[14].buffer_type = MYSQL_TYPE_FLOAT;
        bind[14].buffer = (char*)(&(m_original.pDirection[1]));
        bind[14].is_null = 0;
        bind[14].length = 0;

        ///dirZ
        bind[15].buffer_type = MYSQL_TYPE_FLOAT;
        bind[15].buffer = (char*)(&(m_original.pDirection[2]));
        bind[15].is_null = 0;
        bind[15].length = 0;

        ///posX
        bind[16].buffer_type = MYSQL_TYPE_FLOAT;
        bind[16].buffer = (char*)(&(m_original.pPosition[0]));
        bind[16].is_null = 0;
        bind[16].length = 0;

        ///posY
        bind[17].buffer_type = MYSQL_TYPE_FLOAT;
        bind[17].buffer = (char*)(&(m_original.pPosition[1]));
        bind[17].is_null = 0;
        bind[17].length = 0;

        ///posZ
        bind[18].buffer_type = MYSQL_TYPE_FLOAT;
        bind[18].buffer = (char*)(&(m_original.pPosition[2]));
        bind[18].is_null = 0;
        bind[18].length = 0;

        ///time
        bind[19].buffer_type = MYSQL_TYPE_FLOAT;
        bind[19].buffer = (char*)(&(m_original.fTime));
        bind[19].is_null = 0;
        bind[19].length = 0;

        ///step
        bind[20].buffer_type = MYSQL_TYPE_FLOAT;
        bind[20].buffer = (char*)(&(m_original.fStep));
        bind[20].is_null = 0;
        bind[20].length = 0;

        ///motor_speed
        bind[21].buffer_type = MYSQL_TYPE_FLOAT;
        bind[21].buffer = (char*)(&(m_original.fMotorSpeed));
        bind[21].is_null = 0;
        bind[21].length = 0;

        ///ray_f
        bind[22].buffer_type = MYSQL_TYPE_FLOAT;
        bind[22].buffer = (char*)(&(m_original.ray_f));
        bind[22].is_null = 0;
        bind[22].length = 0;

        ///ray_b
        bind[23].buffer_type = MYSQL_TYPE_FLOAT;
        bind[23].buffer = (char*)(&(m_original.ray_b));
        bind[23].is_null = 0;
        bind[23].length = 0;

        ///ray_l
        bind[24].buffer_type = MYSQL_TYPE_FLOAT;
        bind[24].buffer = (char*)(&(m_original.ray_l));
        bind[24].is_null = 0;
        bind[24].length = 0;

        ///ray_r
        bind[25].buffer_type = MYSQL_TYPE_FLOAT;
        bind[25].buffer = (char*)(&(m_original.ray_r));
        bind[25].is_null = 0;
        bind[25].length = 0;

        ///ray_fl
        bind[26].buffer_type = MYSQL_TYPE_FLOAT;
        bind[26].buffer = (char*)(&(m_original.ray_fl));
        bind[26].is_null = 0;
        bind[26].length = 0;

        ///ray_fr
        bind[27].buffer_type = MYSQL_TYPE_FLOAT;
        bind[27].buffer = (char*)(&(m_original.ray_fr));
        bind[27].is_null = 0;
        bind[27].length = 0;

        ///ray_up
        bind[28].buffer_type = MYSQL_TYPE_FLOAT;
        bind[28].buffer = (char*)(&(m_original.ray_up));
        bind[28].is_null = 0;
        bind[28].length = 0;

        ///ray_down
        bind[29].buffer_type = MYSQL_TYPE_FLOAT;
        bind[29].buffer = (char*)(&(m_original.ray_down));
        bind[29].is_null = 0;
        bind[29].length = 0;

        ///num_wheel
        bind[30].buffer_type = MYSQL_TYPE_LONG;
        bind[30].buffer = (char*)(&(m_original.nColWhell));
        bind[30].is_null = 0;
        bind[30].length = 0;

        ///fwd
        bind[31].buffer_type = MYSQL_TYPE_LONG;
        bind[31].buffer = (char*)(&(m_original.bForward));
        bind[31].is_null = 0;
        bind[31].length = 0;

        ///bak
        bind[32].buffer_type = MYSQL_TYPE_LONG;
        bind[32].buffer = (char*)(&(m_original.bBack));
        bind[32].is_null = 0;
        bind[32].length = 0;

        ///turn_left
        bind[33].buffer_type = MYSQL_TYPE_LONG;
        bind[33].buffer = (char*)(&(m_original.bTurnLeft));
        bind[33].is_null = 0;
        bind[33].length = 0;

        ///turn_right
        bind[34].buffer_type = MYSQL_TYPE_LONG;
        bind[34].buffer = (char*)(&(m_original.bTurnRight));
        bind[34].is_null = 0;
        bind[34].length = 0;

        ///leap
        bind[35].buffer_type = MYSQL_TYPE_LONG;
        bind[35].buffer = (char*)(&(m_original.bLeap));
        bind[35].is_null = 0;
        bind[35].length = 0;

        ///collision
        bind[36].buffer_type = MYSQL_TYPE_LONG;
        bind[36].buffer = (char*)(&(m_original.bCollision));
        bind[36].is_null = 0;
        bind[36].length = 0;

        ///track_type
        bind[37].buffer_type = MYSQL_TYPE_LONG;
        bind[37].buffer = (char*)(&(m_original.track_type));
        bind[37].is_null = 0;
        bind[37].length = 0;

        db_mysql_stmt_bind_param(&m_stInsertOrig, PARAM_NUM, bind, "insertOriginal");
    }

    ///select UID
    {
        static const char* sqlSelectUID = "select uid from user where name=? and \
                                                    age=? and \
                                                    player_type=? and\
                                                    sex=? and\
                                                    game_frequncy=?\0";

        db_mysql_stmt_init(&m_stSelectUID, sqlSelectUID, "selectUID");

        const int PARAM_NUM = 5;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///name
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (char*)&m_user.name;
        bind[0].is_null = 0;
        bind[0].length = &m_user.name_length;
        bind[0].buffer_length = NAME_LEN;

        ///age
        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char*)&m_user.age;
        bind[1].is_null = 0;
        bind[1].length = 0;

        ///player_type
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_user.type;
        bind[2].is_null = 0;
        bind[2].length = 0;

        ///sex
        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = (char*)&m_user.sex;
        bind[3].is_null = 0;
        bind[3].length = 0;

        ///game_frequency
        bind[4].buffer_type = MYSQL_TYPE_LONG;
        bind[4].buffer = (char*)&m_user.frequency;
        bind[4].is_null = 0;
        bind[4].length = 0;

        db_mysql_stmt_bind_param(&m_stSelectUID, PARAM_NUM, bind, "selectUID");

        ///绑定返回的结果
        MYSQL_BIND res_bind;
        memset(&res_bind, 0, sizeof(res_bind));
        res_bind.buffer_type = MYSQL_TYPE_LONG;
        res_bind.buffer = (char*)&m_user.user_id;
        res_bind.is_null = 0;
        res_bind.length = 0;

        db_mysql_stmt_bind_result(&m_stSelectUID, &m_rsSelectUID, 1, &res_bind, "selectUID");
    }

    ///insert UID
    {
        static const char* sqlInsertUID = "insert into user (name, age, player_type, sex, game_frequncy) values (\
                                                            ?, ?, ?, ?, ?)";
        db_mysql_stmt_init(&m_stInsertUID, sqlInsertUID, "insertUID");

        const int PARAM_NUM = 5;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///name
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (char*)&m_user.name;
        bind[0].is_null = 0;
        bind[0].length = &m_user.name_length;
        bind[0].buffer_length = NAME_LEN;

        ///age
        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char*)&m_user.age;
        bind[1].is_null = 0;
        bind[1].length = 0;

        ///player_type
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_user.type;
        bind[2].is_null = 0;
        bind[2].length = 0;

        ///sex
        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = (char*)&m_user.sex;
        bind[3].is_null = 0;
        bind[3].length = 0;

        ///game_frequency
        bind[4].buffer_type = MYSQL_TYPE_LONG;
        bind[4].buffer = (char*)&m_user.frequency;
        bind[4].is_null = 0;
        bind[4].length = 0;

        db_mysql_stmt_bind_param(&m_stInsertUID, PARAM_NUM, bind, "insertUID");
    }


    ///insert LID
    {
        static const char *sqlInsertLID = "insert into level (party_time, finish_proportion, num_attempt, num_finished, record_time, track_name, difficult, skill, feel, fun, feel_degree) values (\
                                                                ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        //static const char *sqlInsertLID = "insert into level (num_attempt, num_finished, record_time, track_name) values (\
        //                                                        ?, ?, ?, ?)";

        db_mysql_stmt_init(&m_stInsertLID, sqlInsertLID, "insertLID");

        const int PARAM_NUM = 11;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///party time
        bind[0].buffer_type = MYSQL_TYPE_FLOAT;
        bind[0].buffer = (char*)&m_level.partytime;
        bind[0].is_null = 0;
        bind[0].length = 0;

        /// FINISH PROPORTION
        bind[1].buffer_type = MYSQL_TYPE_FLOAT;
        bind[1].buffer = (char*)&m_level.finishProportionaTotal;
        bind[1].is_null = 0;
        bind[1].length = 0;

        ///num_attempt
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_level.num_attempt;
        bind[2].is_null = 0;
        bind[2].length = 0;

        ///num_finished
        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = (char*)&m_level.num_finished;
        bind[3].is_null = 0;
        bind[3].length = 0;

        ///record time
        bind[4].buffer_type = MYSQL_TYPE_LONG;
        bind[4].buffer = (char*)&m_level.time_record;
        bind[4].is_null = 0;
        bind[4].length = 0;

        ///track_name
        bind[5].buffer_type = MYSQL_TYPE_STRING;
        bind[5].buffer = (char*)&m_level.trackname;
        bind[5].is_null = 0;
        bind[5].length = &m_level.trackname_length;
        bind[5].buffer_length = NAME_LEN;

        ///difficult
        bind[6].buffer_type = MYSQL_TYPE_LONG;
        bind[6].buffer = (char*)&m_level.difficult;
        bind[6].is_null = 0;
        bind[6].length = 0;

        ///skill
        bind[7].buffer_type = MYSQL_TYPE_LONG;
        bind[7].buffer = (char*)&m_level.skill;
        bind[7].is_null = 0;
        bind[7].length = 0;

        ///feel
        bind[8].buffer_type = MYSQL_TYPE_LONG;
        bind[8].buffer = (char*)&m_level.emotion;
        bind[8].is_null = 0;
        bind[8].length = 0;

        ///fun
        bind[9].buffer_type = MYSQL_TYPE_LONG;
        bind[9].buffer = (char*)&m_level.fun;
        bind[9].is_null = 0;
        bind[9].length = 0;

        ///feel_degree
        bind[10].buffer_type = MYSQL_TYPE_LONG;
        bind[10].buffer = (char*)&m_level.emotion_degree;
        bind[10].is_null = 0;
        bind[10].length = 0;

        /*
                const int PARAM_NUM = 4;

                MYSQL_BIND bind[PARAM_NUM];
                memset(&bind, 0, sizeof(bind));

                ///num_attempt
                bind[0].buffer_type = MYSQL_TYPE_LONG;
                bind[0].buffer = (char*)&m_level.num_attempt;
                bind[0].is_null = 0;
                bind[0].length = 0;

                ///num_finished
                bind[1].buffer_type = MYSQL_TYPE_LONG;
                bind[1].buffer = (char*)&m_level.num_finished;
                bind[1].is_null = 0;
                bind[1].length = 0;

                ///record time
                bind[2].buffer_type = MYSQL_TYPE_LONG;
                bind[2].buffer = (char*)&m_level.time_record;
                bind[2].is_null = 0;
                bind[2].length = 0;

                ///track_name
                bind[3].buffer_type = MYSQL_TYPE_STRING;
                bind[3].buffer = (char*)&m_level.trackname;
                bind[3].is_null = 0;
                bind[3].length = &m_level.trackname_length;
                bind[3].buffer_length = NAME_LEN;
        */
        db_mysql_stmt_bind_param(&m_stInsertLID, PARAM_NUM, bind, "insertLID");
    }

    ///select LID
    {
        static const char *sqlSelectLID = "select level_id from level where record_time=? and num_attempt=? and num_finished=?";
        db_mysql_stmt_init(&m_stSelectLID, sqlSelectLID, "selectLID");

        const int PARAM_NUM = 3;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///partytime
        bind[0].buffer_type = MYSQL_TYPE_LONG;
        bind[0].buffer = (char*)&m_level.time_record;
        bind[0].is_null = 0;
        bind[0].length = 0;

        ///finish_proportion
        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char*)&m_level.num_attempt;
        bind[1].is_null = 0;
        bind[1].length = 0;

        ///num_attempt
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_level.num_finished;
        bind[2].is_null = 0;
        bind[2].length = 0;

        db_mysql_stmt_bind_param(&m_stSelectLID, PARAM_NUM, bind, "selectLID");

        MYSQL_BIND res_bind;
        memset(&res_bind, 0, sizeof(res_bind));
        res_bind.buffer_type = MYSQL_TYPE_LONG;
        res_bind.buffer = (char*)&m_level.level_id;
        res_bind.is_null = 0;
        res_bind.length = 0;

        db_mysql_stmt_bind_result(&m_stSelectLID, &m_rsSelectLID, 1, &res_bind, "selectLID");
    }

    ///insert SID
    {
        static const char *sqlInsertAID = "insert into attempt (attempt_time, finish_proportion, attempt_index, finished, level_id, record_time, ori_total) values (\
                                                                ?, ?, ?, ?, ?, ?, ?)";

        db_mysql_stmt_init(&m_stInsertAID, sqlInsertAID, "insertAID");

        const int PARAM_NUM = 7;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///attempt time
        bind[0].buffer_type = MYSQL_TYPE_FLOAT;
        bind[0].buffer = (char*)&m_attempt.fTotaltime;
        bind[0].is_null = 0;
        bind[0].length = 0;

        /// FINISH PROPORTION
        bind[1].buffer_type = MYSQL_TYPE_FLOAT;
        bind[1].buffer = (char*)&m_attempt.finishProportion;
        bind[1].is_null = 0;
        bind[1].length = 0;

        ///attempt index
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_attempt.attemp_index;
        bind[2].is_null = 0;
        bind[2].length = 0;

        ///bfinished
        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = (char*)&m_attempt.bFinished;
        bind[3].is_null = 0;
        bind[3].length = 0;

        ///level_id
        bind[4].buffer_type = MYSQL_TYPE_LONG;
        bind[4].buffer = (char*)&m_attempt.level_id;
        bind[4].is_null = 0;
        bind[4].length = 0;

        ///record_time
        bind[5].buffer_type = MYSQL_TYPE_LONG;
        bind[5].buffer = (char*)&m_attempt.time_record;
        bind[5].is_null = 0;
        bind[5].length = 0;

        ///ori_total
        bind[6].buffer_type = MYSQL_TYPE_LONG;
        bind[6].buffer = (char*)&m_attempt.nOriginalTotal;
        bind[6].is_null = 0;
        bind[6].length = 0;

        db_mysql_stmt_bind_param(&m_stInsertAID, PARAM_NUM, bind, "insertAID");
    }

    ///select AID
    {
        static const char* sqlSelectAID = "select attempt_id from attempt where level_id=? and attempt_index=?";
        db_mysql_stmt_init(&m_stSelectAID, sqlSelectAID, "selectAID");

        const int PARAM_NUM = 2;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///level_id
        bind[0].buffer_type = MYSQL_TYPE_LONG;
        bind[0].buffer = (char*)&m_original.nLevel_id;
        bind[0].is_null = 0;
        bind[0].length = 0;

        ///attempt index
        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char*)&m_original.nAttempIndex;
        bind[1].is_null = 0;
        bind[1].length = 0;
        db_mysql_stmt_bind_param(&m_stSelectAID, PARAM_NUM, bind, "selectAID");

        MYSQL_BIND res_bind;
        memset(&res_bind, 0, sizeof(res_bind));
        res_bind.buffer_type = MYSQL_TYPE_LONG;
        res_bind.buffer = (char*)&m_attempt.attempt_id;
        res_bind.is_null = 0;
        res_bind.length = 0;

        db_mysql_stmt_bind_result(&m_stSelectAID, &m_rsSelectAID, 1, &res_bind, "selectAID");
    }

    ///insert SID
    {
        static const char *sqlInsertSID = "insert into session (uid, time, level_id_A, level_id_B) values (?, ?, ?, ?)";
        db_mysql_stmt_init(&m_stInsertSID, sqlInsertSID, "insertSID");

        const int PARAM_NUM = 4;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///uid
        bind[0].buffer_type = MYSQL_TYPE_LONG;
        bind[0].buffer = (char*)&m_session.user_id;
        bind[0].is_null = 0;
        bind[0].length = 0;

        ///time
        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (char*)&m_session.time;
        bind[1].is_null = 0;
        bind[1].length = &m_session.time_length;
        bind[1].buffer_length = NAME_LEN;

        ///level_id_A
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_session.level_id1;
        bind[2].is_null = 0;
        bind[2].length = 0;

        ///level_id_B
        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = (char*)&m_session.level_id2;
        bind[3].is_null = 0;
        bind[3].length = 0;

        db_mysql_stmt_bind_param(&m_stInsertSID, PARAM_NUM, bind, "insertSID");

    }

    ///select SID
    {
        static const char *sqlSelectSID = "select sid from session where time = ? and level_id_A = ? and level_id_B = ?";
        db_mysql_stmt_init(&m_stSelectSID, sqlSelectSID, "selectSID");

        const int PARAM_NUM = 3;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///time
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (char*)&m_session.time;
        bind[0].is_null = 0;
        bind[0].length = &m_session.time_length;
        bind[0].buffer_length = NAME_LEN;

        ///level_id_A
        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char*)&m_session.level_id1;
        bind[1].is_null = 0;
        bind[1].length = 0;

        ///level_id_B
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_session.level_id2;
        bind[2].is_null = 0;
        bind[2].length = 0;

        db_mysql_stmt_bind_param(&m_stSelectSID, PARAM_NUM, bind, "selectSID");

        MYSQL_BIND res_bind;
        memset(&res_bind, 0, sizeof(res_bind));
        res_bind.buffer_type = MYSQL_TYPE_LONG;
        res_bind.buffer = (char*)&m_session.session_id;
        res_bind.is_null = 0;
        res_bind.length = 0;

        db_mysql_stmt_bind_result(&m_stSelectSID, &m_rsSelectSID, 1, &res_bind, "selectSID");
    }

    ///insert PREFER
    {
        static const char *sqlInsertPrefer = "insert into prefer_result (sid, prefer_type, prefer_result) values (?, ?, ?)";
        db_mysql_stmt_init(&m_stInsertPrefer, sqlInsertPrefer, "insertPrefer");

        const int PARAM_NUM = 3;

        MYSQL_BIND bind[PARAM_NUM];
        memset(&bind, 0, sizeof(bind));

        ///sid
        bind[0].buffer_type = MYSQL_TYPE_LONG;
        bind[0].buffer = (char*)&m_prefer.sid;
        bind[0].is_null = 0;
        bind[0].length = 0;

        ///prefer_type
        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char*)&m_prefer.prefer_type;
        bind[1].is_null = 0;
        bind[1].length = 0;
        bind[1].buffer_length = 0;

        ///prefer_result
        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char*)&m_prefer.prefer_result;
        bind[2].is_null = 0;
        bind[2].length = 0;

        db_mysql_stmt_bind_param(&m_stInsertPrefer, PARAM_NUM, bind, "insertPrefer");
    }
    return true;
}

bool CMySQLSaver::db_mysql_stmt_init(MYSQL_STMT **pStmt, const char *sql, const char* szName)
{
    *pStmt = mysql_stmt_init(&mysql);
    if(!*pStmt)
    {
        fprintf(stderr, "初始化%s失败: %s\n", szName, mysql_stmt_error(*pStmt));
        return false;
    }

    if(mysql_stmt_prepare(*pStmt, sql, strlen(sql)))
    {
        fprintf(stderr, "准备%s失败: %s\n", szName, mysql_stmt_error(*pStmt));
        return false;
    }

    //fprintf(stdout, "建立%s成功\n", szName);
    return true;
}

bool CMySQLSaver::db_mysql_stmt_bind_param(MYSQL_STMT **pStmt, const int num_param, MYSQL_BIND *bind, const char* szName)
{
    int param_count = mysql_stmt_param_count(*pStmt);

    if(param_count != num_param)
    {
        fprintf(stderr, "%s参数数量问题\n", szName);
        return false;
    }

    if(mysql_stmt_bind_param(*pStmt, bind))
    {
        fprintf(stderr, "%s 绑定参数失败: %s", szName, mysql_stmt_error(*pStmt));
        return false;
    }

    //fprintf(stdout, "绑定%s参数成功\n", szName);
    return true;
}

bool CMySQLSaver::db_mysql_stmt_bind_result(MYSQL_STMT **pStmt, MYSQL_RES **pRes, const int num_result, MYSQL_BIND *bind, const char *szName)
{
    *pRes = mysql_stmt_result_metadata(*pStmt);
    if(!*pRes)
    {
        fprintf(stderr, "准备%s失败: %s", szName, mysql_stmt_error(*pStmt));
        return false;
    }

    int nColRes = mysql_num_fields(*pRes);
    if(nColRes != num_result)
    {
        fprintf(stderr, "错误的结果列数: %s", szName);
        return false;
    }

    if(mysql_stmt_bind_result(*pStmt, bind))
    {
        fprintf(stderr, "%s绑定结果失败: %s", szName, mysql_stmt_error(*pStmt));
        return false;
    }

    //fprintf(stdout, "绑定%s结果集成功\n", szName);
    return true;
}

bool CMySQLSaver::db_mysql_init_statistictable()
{
    char sqlItem[500];

    /*
    char *sqlDropTable = "drop table if exists level_statistic;";
    int len = strlen(sqlDropTable);
//    raydium_log(sqlInsert);
    if (mysql_real_query(&mysql, sqlDropTable, strlen(sqlDropTable))!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "%sdb error: %s", prefix, mysql_error(&mysql));
            return 0;
        }
    }
    */

    std::string sqlCreateTable = "CREATE TABLE IF NOT EXISTS level_statistic(id int NOT NULL AUTO_INCREMENT PRIMARY KEY,level_id int NOT NULL";
    memset(sqlItem, 0, sizeof(sqlItem));

    for(int i=0; i< TRACK_TYPE_NUM; ++i)
    {
        std::string szTmp1 = szTrackType[i];
        for(int j=0; j<ANALYSE_TYPE_MAX; ++j)
        {
            std::string szTmp2 = szAnalyseType[j];
            for(int k=0; k<STATISTIC_TYPE; ++k)
            {
                std::string szTmp3 = szStatistic[k];

                if(k==0)
                    sprintf(sqlItem, ",%s_%s_%s int", szTmp1.c_str(), szTmp2.c_str(), szTmp3.c_str());
                else
                    sprintf(sqlItem, ",%s_%s_%s float", szTmp1.c_str(), szTmp2.c_str(), szTmp3.c_str());

                sqlCreateTable += sqlItem;
            }
        }
    }

    sqlCreateTable += ");";

    if (mysql_real_query(&mysql, sqlCreateTable.c_str(), sqlCreateTable.size())!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "%sdb error: %s", prefix, mysql_error(&mysql));
            return 0;
        }
    }



    std::string sqlInsertLS = "insert into level_statistic (level_id";

    for(int i=0; i< TRACK_TYPE_NUM; ++i)
    {
        std::string szTmp1 = szTrackType[i];
        for(int j=0; j<ANALYSE_TYPE_MAX; ++j)
        {
            std::string szTmp2 = szAnalyseType[j];
            for(int k=0; k<STATISTIC_TYPE; ++k)
            {
                std::string szTmp3 = szStatistic[k];

                sprintf(sqlItem, ",%s_%s_%s", szTmp1.c_str(), szTmp2.c_str(), szTmp3.c_str());

                sqlInsertLS += sqlItem;
            }
        }
    }

    sqlInsertLS += ")values(?";

    int nTotal = (TRACK_TYPE_NUM) * (ANALYSE_TYPE_MAX) * (STATISTIC_TYPE);
    for(int i = 0; i < nTotal; ++i)
        sqlInsertLS += ",?";
    sqlInsertLS += ")";

    db_mysql_stmt_init(&m_stInsertLS, sqlInsertLS.c_str(), "insertLS");

    int PARAM_NUM = nTotal + 1;

    MYSQL_BIND *bind = new MYSQL_BIND[PARAM_NUM];
    memset(bind, 0, sizeof(MYSQL_BIND)*PARAM_NUM);

    int index = 0;
    bind[index].buffer_type = MYSQL_TYPE_LONG;
    bind[index].buffer = (char*)&m_result.id;
    bind[index].is_null = 0;
    bind[index].length = 0;

    ++index;

    for(int i=0; i< TRACK_TYPE_NUM; ++i)
    {
        for(int j=0; j<ANALYSE_TYPE_MAX; ++j)
        {

            int pos = 1 + i*ANALYSE_TYPE_MAX*STATISTIC_TYPE + j*STATISTIC_TYPE;

            if(index != pos || index > PARAM_NUM)
            {
                fprintf(stderr, "%sdb error: %s", prefix, "wrong index");
                return false;
            }

            //static std::string szStatistic[] = { "cnt", "sum", "avg", "min", "max", "var"};
            bind[index].buffer_type = MYSQL_TYPE_LONG;
            bind[index].buffer = (char*)&(m_result.analyse_result[i][j].nCount);
            bind[index].is_null = 0;
            bind[index].length = 0;
            ++index;

            bind[index].buffer_type = MYSQL_TYPE_FLOAT;
            bind[index].buffer = (char*)&(m_result.analyse_result[i][j].fSum);
            bind[index].is_null = 0;
            bind[index].length = 0;
            ++index;

            bind[index].buffer_type = MYSQL_TYPE_FLOAT;
            bind[index].buffer = (char*)&(m_result.analyse_result[i][j].fAvg);
            bind[index].is_null = 0;
            bind[index].length = 0;
            ++index;

            bind[index].buffer_type = MYSQL_TYPE_FLOAT;
            bind[index].buffer = (char*)&(m_result.analyse_result[i][j].fEntropy);
            bind[index].is_null = 0;
            bind[index].length = 0;
            ++index;
/*
            bind[index].buffer_type = MYSQL_TYPE_FLOAT;
            bind[index].buffer = (char*)&(m_result.analyse_result[i][j].fMin);
            bind[index].is_null = 0;
            bind[index].length = 0;
            ++index;

            bind[index].buffer_type = MYSQL_TYPE_FLOAT;
            bind[index].buffer = (char*)&(m_result.analyse_result[i][j].fMax);
            bind[index].is_null = 0;
            bind[index].length = 0;
            ++index;
*/
            bind[index].buffer_type = MYSQL_TYPE_FLOAT;
            bind[index].buffer = (char*)&(m_result.analyse_result[i][j].fVariance);
            bind[index].is_null = 0;
            bind[index].length = 0;
            ++index;
        }
    }

    db_mysql_stmt_bind_param(&m_stInsertLS, PARAM_NUM, bind, "insertLS");
    delete [] bind;


    return false;
}

bool CMySQLSaver::db_save_statistic_stmt()
{
    if(mysql_stmt_execute(m_stInsertLS))
    {
        fprintf(stderr, "%sm_stInsertLS执行失败: %s\n", prefix, mysql_stmt_error(m_stInsertUID));
        return false;
    }
    return true;
}

bool CMySQLSaver::db_save_trackresult_stmt(CTrackResult& result)
{
    memcpy(&m_trackresult, &result, sizeof(CTrackResult));

    if(mysql_stmt_execute(m_stInsertTS))
    {
        fprintf(stderr, "%sm_stInsertTS执行失败: %s", prefix, mysql_stmt_error(m_stInsertUID));
        return false;
    }
    return true;
}

bool CMySQLSaver::db_mysql_init_trackresult()
{
    char sqlItem[1024];

/*
    char *sqlDropTable = "drop table if exists track_statistic;";
    int len = strlen(sqlDropTable);
//    raydium_log(sqlInsert);
    if (mysql_real_query(&mysql, sqlDropTable, strlen(sqlDropTable))!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "%sdb error: %s", prefix, mysql_error(&mysql));
            return 0;
        }
    }
*/
    std::string sqlCreateTable = "CREATE TABLE IF NOT EXISTS track_statistic(id int NOT NULL AUTO_INCREMENT PRIMARY KEY,track_name VARCHAR(100), time FLOAT, length int";
    memset(sqlItem, 0, sizeof(sqlItem));

    for(int i=0; i< TRACK_TYPE_NUM; ++i)
    {
        //sprintf(sqlItem, ",%s_cnt int,%s_sum float,%s_avg float,%s_ent float", szTrackType[i].c_str());
        sprintf(sqlItem, ",%s_cnt int", szTrackType[i].c_str());
        sqlCreateTable += sqlItem;

        sprintf(sqlItem, ",%s_sum float", szTrackType[i].c_str());
        sqlCreateTable += sqlItem;

        sprintf(sqlItem, ",%s_avg float", szTrackType[i].c_str());
        sqlCreateTable += sqlItem;

        sprintf(sqlItem, ",%s_ent float", szTrackType[i].c_str());
        sqlCreateTable += sqlItem;
    }

    sqlCreateTable += ");";

    if (mysql_real_query(&mysql, sqlCreateTable.c_str(), sqlCreateTable.size())!=0)
    {
        if(g_bConnected == 1)
        {
            fprintf(stderr, "%sdb error: %s", prefix, mysql_error(&mysql));
            return 0;
        }
    }



    std::string sqlInsertLS = "insert into track_statistic (track_name, time, length";

    for(int i=0; i< TRACK_TYPE_NUM; ++i)
    {
        const char* szType = szTrackType[i].c_str();
        sprintf(sqlItem, ",%s_cnt", szTrackType[i].c_str());
        sqlInsertLS += sqlItem;

        sprintf(sqlItem, ",%s_sum", szTrackType[i].c_str());
        sqlInsertLS += sqlItem;

        sprintf(sqlItem, ",%s_avg", szTrackType[i].c_str());
        sqlInsertLS += sqlItem;

        sprintf(sqlItem, ",%s_ent", szTrackType[i].c_str());
        sqlInsertLS += sqlItem;
    }

    sqlInsertLS += ")values(?,?,?";

    int nTotal = (TRACK_TYPE_NUM) * (TRACK_STATISTIC_TYPE);
    for(int i = 0; i < nTotal; ++i)
        sqlInsertLS += ",?";
    sqlInsertLS += ")";

    if(!db_mysql_stmt_init(&m_stInsertTS, sqlInsertLS.c_str(), "insertLS"))
        return false;

    int PARAM_NUM = nTotal + 3;

    MYSQL_BIND *bind = new MYSQL_BIND[PARAM_NUM];
    memset(bind, 0, sizeof(MYSQL_BIND)*PARAM_NUM);

    int index = 0;

    ///track_name
    bind[index].buffer_type = MYSQL_TYPE_STRING;
    bind[index].buffer = (char*)&m_trackresult.szName;
    bind[index].is_null = 0;
    bind[index].length = &m_trackresult.nNameLength;
    bind[index].buffer_length = NAME_LEN;
    ++index;

    bind[index].buffer_type = MYSQL_TYPE_FLOAT;
    bind[index].buffer = (char*)&(m_trackresult.fTime);
    bind[index].is_null = 0;
    bind[index].length = 0;
    ++index;

    bind[index].buffer_type = MYSQL_TYPE_LONG;
    bind[index].buffer = (char*)&(m_trackresult.nTotal);
    bind[index].is_null = 0;
    bind[index].length = 0;
    ++index;

    for(int i=0; i< TRACK_TYPE_NUM; ++i)
    {
        int pos = 3 + i*TRACK_STATISTIC_TYPE;
        if(index != pos || index > PARAM_NUM)
        {
            fprintf(stderr, "%sdb error: %s", prefix, "wrong index");
            return false;
        }

        bind[index].buffer_type = MYSQL_TYPE_LONG;
        bind[index].buffer = (char*)&(m_trackresult.track_result[i].nCount);
        bind[index].is_null = 0;
        bind[index].length = 0;
        ++index;

        bind[index].buffer_type = MYSQL_TYPE_FLOAT;
        bind[index].buffer = (char*)&(m_trackresult.track_result[i].fSum);
        bind[index].is_null = 0;
        bind[index].length = 0;
        ++index;

        bind[index].buffer_type = MYSQL_TYPE_FLOAT;
        bind[index].buffer = (char*)&(m_trackresult.track_result[i].fAvg);
        bind[index].is_null = 0;
        bind[index].length = 0;
        ++index;

        bind[index].buffer_type = MYSQL_TYPE_FLOAT;
        bind[index].buffer = (char*)&(m_trackresult.track_result[i].fEntropy);
        bind[index].is_null = 0;
        bind[index].length = 0;
        ++index;
    }

    if(!db_mysql_stmt_bind_param(&m_stInsertTS, PARAM_NUM, bind, "insertTS"))
    {
        delete [] bind;
        return false;
    }

    delete [] bind;

    return true;
}


