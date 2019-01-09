#define _WIN32_WINNT 0x0500

#include "SQLiteSaver.h"
#include "WinBase.h"
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include "utility.h"
#include <assert.h>
#include <algorithm>

#define SAFE_FINALIZE_STMT(stmt)  if(stmt) sqlite3_finalize(stmt)

CSqlite3Saver::CSqlite3Saver(void) : m_bEnable(false)
{
    m_bEnable = Init();
    if(m_bEnable)
    {
        prepare_stmt();
    }
}

CSqlite3Saver::~CSqlite3Saver(void)
{
    if(db)
    {
        SAFE_FINALIZE_STMT(m_stInsertUsrInfo);
        SAFE_FINALIZE_STMT(m_stSelectUID);
        SAFE_FINALIZE_STMT(m_stInsertLID);
        SAFE_FINALIZE_STMT(m_stSelectLID);
        SAFE_FINALIZE_STMT(m_stInsertAID);
        SAFE_FINALIZE_STMT(m_stSelectAID);
        SAFE_FINALIZE_STMT(m_stInsertOID);
        SAFE_FINALIZE_STMT(m_stSelectSID);
        SAFE_FINALIZE_STMT(m_stInsertSID);
        SAFE_FINALIZE_STMT(m_stInsertPrefer);

        sqlite3_close(db);
    }
}

bool CSqlite3Saver::Init(void)
{
    GetPCName(m_szName);
    char buf[128];

    char szDir[100] = "./collect/\0";
    if(!CreateDir(szDir))
        return false;

    sprintf(buf, "%s%s.data", szDir, m_szName);

    return InitDB(buf);
}

bool CSqlite3Saver::InitDB(char *db_name)
{
    //连接数据库
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(db_name, &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    //建立表格
    static char* createSQL[] =
    {
        "CREATE TABLE IF NOT EXISTS user \
        (\
            uid INTEGER PRIMARY KEY ASC AUTOINCREMENT,\
            name VARCHAR(128) NOT NULL,\
            age INTEGER DEFAULT NULL,\
            player_type INTEGER DEFAULT NULL,\
            sex INTEGER DEFAULT NULL,\
            game_frequncy INTEGER DEFAULT NULL\
        )",

        "CREATE TABLE IF NOT EXISTS session \
        (\
            sid INTEGER PRIMARY KEY ASC AUTOINCREMENT,\
            uid INTEGER NOT NULL,\
            time VARCHAR(50) NOT NULL,\
            level_id_A INTEGER NOT NULL,\
            level_id_B INTEGER NOT NULL\
        )",

        "CREATE TABLE IF NOT EXISTS prefer_result \
        (\
            pid INTEGER PRIMARY KEY ASC AUTOINCREMENT,\
            sid INTEGER NOT NULL,\
            prefer_type INTEGER DEFAULT NULL,\
            prefer_result INTEGER DEFAULT '-1'\
        )",

        "CREATE TABLE IF NOT EXISTS level \
        (\
            level_id INTEGER PRIMARY KEY ASC AUTOINCREMENT,\
            party_time FLOAT DEFAULT NULL,\
            finish_proportion FLOAT NOT NULL,\
            num_attempt INTEGER NOT NULL,\
            num_finished INTEGER NOT NULL,\
            record_time INTEGER NOT NULL,\
            track_name VARCHAR(100) DEFAULT NULL,\
            difficult INTEGER DEFAULT NULL,\
            skill INTEGER DEFAULT NULL,\
            feel INTEGER DEFAULT NULL,\
            fun INTEGER DEFAULT NULL,\
            feel_degree INTEGER DEFAULT NULL\
        )",

        "CREATE TABLE IF NOT EXISTS attempt \
        (\
            attempt_id INTEGER PRIMARY KEY ASC AUTOINCREMENT,\
            attempt_time FLOAT DEFAULT NULL,\
            finish_proportion FLOAT DEFAULT NULL,\
            attempt_index INTEGER NOT NULL,\
            finished BOOLEAN DEFAULT NULL,\
            level_id INTEGER NOT NULL,\
            record_time INTEGER NOT NULL,\
            ori_total INTEGER DEFAULT NULL\
        )",

        "CREATE TABLE IF NOT EXISTS original \
        (\
            ori_id INTEGER PRIMARY KEY ASC AUTOINCREMENT,\
            level_id INTEGER NOT NULL,\
            attempt_id INTEGER NOT NULL,\
            attempt_index INTEGER DEFAULT NULL,\
            ori_index INTEGER DEFAULT NULL,\
            lspdX FLOAT DEFAULT NULL,\
            lspdY FLOAT DEFAULT NULL,\
            lspdZ FLOAT DEFAULT NULL,\
            aspdX FLOAT DEFAULT NULL,\
            aspdY FLOAT DEFAULT NULL,\
            aspdZ FLOAT DEFAULT NULL,\
            rotX FLOAT DEFAULT NULL,\
            rotY FLOAT DEFAULT NULL,\
            rotZ FLOAT DEFAULT NULL,\
            dirX FLOAT DEFAULT NULL,\
            dirY FLOAT DEFAULT NULL,\
            dirZ FLOAT DEFAULT NULL,\
            posX FLOAT DEFAULT NULL,\
            posY FLOAT DEFAULT NULL,\
            posZ FLOAT DEFAULT NULL,\
            time FLOAT DEFAULT NULL,\
            step FLOAT DEFAULT NULL,\
            motor_speed FLOAT DEFAULT NULL,\
            ray_f FLOAT DEFAULT NULL,\
            ray_b FLOAT DEFAULT NULL,\
            ray_l FLOAT DEFAULT NULL,\
            ray_r FLOAT DEFAULT NULL,\
            ray_fl FLOAT DEFAULT NULL,\
            ray_fr FLOAT DEFAULT NULL,\
            ray_up FLOAT DEFAULT NULL,\
            ray_down FLOAT DEFAULT NULL,\
            num_wheel INTEGER DEFAULT NULL,\
            fwd INTEGER DEFAULT NULL,\
            bak INTEGER DEFAULT NULL,\
            turn_left INTEGER DEFAULT NULL,\
            turn_right INTEGER DEFAULT NULL,\
            leap INTEGER DEFAULT NULL,\
            collision INTEGER DEFAULT NULL,\
            track_type INTEGER DEFAULT NULL\
        )"
    };

    int size = sizeof(createSQL) / sizeof(createSQL[0]);

    for(int i=0; i<size; ++i)
    {
        rc = sqlite3_exec(db, createSQL[i], 0, 0, &zErrMsg);
        if( rc!=SQLITE_OK )
        {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return false;
        }
    }

    fprintf(stdout, "SQLITE启动成功\n");
//    sqlite3_close(db);
    return true;
}

///准备各种statement
bool CSqlite3Saver::prepare_stmt(void)
{
    int rc;

    ///selectUID
    {
        static const char *sqlSelectUID = "select uid from user where name = ? and \
                                                                age = ? and \
                                                                player_type = ? and \
                                                                sex = ? and \
                                                                game_frequncy = ?";
        rc = sqlite3_prepare_v2(db, sqlSelectUID, -1, &m_stSelectUID, 0);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备selectUID失败\n");
    }

    ///insertUID
    {
        static const char *sqlInsertUser = "insert into user ( name, age, player_type, sex, game_frequncy) values (?, ?, ?, ?, ?)";
        rc = sqlite3_prepare_v2(db, sqlInsertUser, -1, &m_stInsertUsrInfo, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备insertUID失败\n");
    }

    ///insertLID
    {
        static const char *sqlInsertLID = "insert into level (party_time, \
                                                                finish_proportion, \
                                                                num_attempt, \
                                                                num_finished, \
                                                                record_time, \
                                                                track_name,\
                                                                difficult,\
                                                                skill,\
                                                                feel,\
                                                                fun,\
                                                                feel_degree) \
                                                                values (?,?,?,?,?,?,?,?,?,?,?)";
        rc = sqlite3_prepare_v2(db, sqlInsertLID, -1, &m_stInsertLID, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备insertLID失败");
    }

    ///selectLID
    {
        static const char *sqlSelectLID = "select level_id from level where record_time = ? and \
                                                                            num_attempt = ? and \
                                                                            num_finished= ?";
        rc = sqlite3_prepare_v2(db, sqlSelectLID, -1, &m_stSelectLID, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备selectLID失败");
    }

    ///insertAID
    {
        static const char *sqlInsertAID = "insert into attempt (attempt_time, \
                                                                finish_proportion, \
                                                                attempt_index, \
                                                                finished, \
                                                                level_id, \
                                                                record_time, \
                                                                ori_total) values \
                                                                (?,?,?,?,?,?,?)";
        rc = sqlite3_prepare_v2(db, sqlInsertAID, -1, &m_stInsertAID, NULL);

        if(rc != SQLITE_OK)
            fprintf(stderr, "准备insertAID失败");
    }

    ///selectAID
    {
        static const char *sqlSelectAID = "select attempt_id from attempt where level_id = ? and attempt_index = ?";

        rc = sqlite3_prepare_v2(db, sqlSelectAID, -1, &m_stSelectAID, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备selectAID失败");
    }

    ///insertOID
    {
        static const char *sqlInsertOID = "insert into original (level_id, attempt_id, attempt_index, ori_index,\
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
                                                        ?,?,?,?,?,?, ?)";
        rc = sqlite3_prepare_v2(db, sqlInsertOID, -1, &m_stInsertOID, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备insertOID失败");
    }

    ///insertSID
    {
        static const char *sqlInsertSID = "insert into session (uid, time, level_id_A, level_id_B) values (\
                                                                ?, ?, ?, ?)";
        rc = sqlite3_prepare_v2(db, sqlInsertSID, -1, &m_stInsertSID, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备insertSID失败");
    }

    ///selectSID
    {
        static const char *sqlSelectSID = "select sid from session where time = ? and level_id_A = ? and level_id_B = ?";

        rc = sqlite3_prepare_v2(db, sqlSelectSID, -1, &m_stSelectSID, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备selectSID失败");
    }

    ///insertPrefer
    {
        static const char *sqlInsertPrefer = "insert into prefer_result (sid, prefer_type, prefer_result) values (\
                                                                        ?, ?, ?)";
        rc = sqlite3_prepare_v2(db, sqlInsertPrefer, -1, &m_stInsertPrefer, NULL);
        if(rc != SQLITE_OK)
            fprintf(stderr, "准备inserPrefer失败");
    }

}

int CSqlite3Saver::save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                                     std::vector<c2s_driveinfo_level> &levelStore,
                                     std::vector<c2s_driveinfo_attempt_original> &originalStore,
                                     c2s_4afc_result &prefer,
                                     c2s_user_info &user,
                                     session_info &session)
{
    assert(true && "没有实现");
    return -1;
}

int CSqlite3Saver::save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                                     std::vector<c2s_driveinfo_level> &levelStore,
                                     std::vector<c2s_driveinfo_attempt_original> &originalStore,
                                     c2s_4afc_result &prefer,
                                     c2s_user_info &user)
{
    if(levelStore.size() != 2)
        return PRE_DB_ERROR;


    int user_id = db_get_uid(user);

    assert(user_id != 0 && "用户ID获取错误");

    //写入level信息
    int level_id1 = db_save_get_level_id(&levelStore[0]);
    int level_id2 = db_save_get_level_id(&levelStore[1]);

    //写入attempt信息
    std::vector<c2s_driveinfo_attempt>::iterator ita;
    for(ita = attemptStore.begin(); ita != attemptStore.end(); ++ita)
    {
        c2s_driveinfo_attempt *pAttempt = &(*ita);
        if(pAttempt->level_id == 0)
        {
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
        db_save_attemp_info(pAttempt);
    }

    ///修改original中的level_id和attempt_id，以便后续集体存入数据库
    std::vector<c2s_driveinfo_attempt_original>::iterator ito;
    for(ito = originalStore.begin(); ito != originalStore.end(); ++ito)
    {
        c2s_driveinfo_attempt_original *pOrig = &(*ito);
        //int curLID;

        if(pOrig->nLevel_id == 0)
        {
            //curLID = level_id1;
            pOrig->nLevel_id = level_id1;
        }
        else if(pOrig->nLevel_id == 1)
        {
            //curLID = level_id2;
            pOrig->nLevel_id = level_id2;
        }

        //由于这一步骤的存在，很难用于事务
        pOrig->nAttempt_id = db_get_attempt_id_cache(pOrig->nLevel_id, pOrig->nAttempIndex);
    }

    ///original的批存储
    bool bSucced = true;
    sqlite3_exec(db, "BEGIN;", 0, 0, 0);

    for(ito = originalStore.begin(); ito != originalStore.end(); ++ito)
    {
        c2s_driveinfo_attempt_original *pOrig = &(*ito);
        //db_save_original_info_transaction(pOrig);
        if(db_save_original_info_stmt(pOrig) < 0)
        {
            fprintf(stderr,"original存储出错\n");
            bSucced = false;
            break;
        }
    }

    if(bSucced)
    {
        sqlite3_exec(db, "COMMIT;", 0, 0, 0);
        fprintf(stderr,"original事务批存储成功\n");
    }
    else
    {
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        fprintf(stderr, "original事务批存储失败，回滚...\n");
    }

    //存入session
    int sid = db_save_get_session_id(user_id, level_id1, level_id2);
    prefer.sid = sid;
    db_save_prefer_result(prefer);

    return PRE_DB_OK;
}

int CSqlite3Saver::db_get_attempt_id_cache(int level_id, int attempt_index)
{
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

//保存并得到玩家id
int CSqlite3Saver::db_get_uid(c2s_user_info &info)
{
    //char sqlGetUID[512];
    char *zErrMsg = 0;
    int rc;
    int usr_id = -1;

    memcpy(&m_userinfo, &info, sizeof(c2s_user_info));

    db_bind_selectUID();
    rc = db_execute_stmt(m_stSelectUID);
    if(rc == SQLITE_ROW)
    {
        usr_id = sqlite3_column_int(m_stSelectUID, 0);
    }
    else if(rc == SQLITE_DONE)
    {
        //应该再这里插入新的用户信息
        db_bind_insertUID();
        rc = db_execute_stmt(m_stInsertUsrInfo);
        if(rc == SQLITE_DONE)
        {
            //插入成功再获取
            usr_id = sqlite3_last_insert_rowid(db);
            return usr_id;
            /*
            rc = db_execute_stmt(m_stSelectUID);
            if(rc == SQLITE_ROW)
            {
                usr_id = sqlite3_column_int(m_stSelectUID, 0);
                return usr_id;
            }
            else if(rc == SQLITE_DONE)
            {
                fprintf(stderr, "insert uid失效");
                return -1;
            }
            else if(rc == SQLITE_ERROR)
            {
                fprintf(stderr, "select uid 出错");
                return -1;
            }
            */
        }
        else if(rc == SQLITE_ERROR)
        {
            fprintf(stderr, "insert user_info 失败");
            return -1;
        }
    }
    else if(rc == SQLITE_ERROR)
    {
        fprintf(stderr, "select uid 失败");
        return -1;
    }

    return usr_id;
}
//保存并得到level_id
int CSqlite3Saver::db_save_get_level_id(c2s_driveinfo_level* pLevel)
{
    int lid = 0;
    int rc;

    memcpy(&m_level, pLevel, sizeof(c2s_driveinfo_level));

    db_bind_insertLID();
    rc = db_execute_stmt(m_stInsertLID);
    if(rc == SQLITE_DONE)
    {
        //获取LID
        lid = sqlite3_last_insert_rowid(db);
        return lid;

        /*
        db_bind_selectLID();
        rc = db_execute_stmt(m_stSelectLID);
        if(rc == SQLITE_ROW)
        {
            lid = sqlite3_column_int(m_stSelectLID, 0);
            return lid;
        }
        else if(rc == SQLITE_DONE)
        {
            fprintf(stderr, "没有找到lid");
            return -1;
        }
        */
    }
    else
    {
        fprintf(stderr, "插入level失败");
        return -1;
    }

    return(0);
}
//得到attempt_id
int CSqlite3Saver::db_get_attempt_id(int level_id, int attempt_index)
{
    int rc;
    int aid;

    m_attempt.level_id = level_id;
    m_attempt.attemp_index = attempt_index;

    db_bind_selectAID();
    rc = db_execute_stmt(m_stSelectAID);
    if(rc == SQLITE_ROW)
    {
        aid = sqlite3_column_int(m_stSelectAID, 0);
        return aid;
    }
    else
    {
        fprintf(stderr, "selectAID失败");
        return -1;
    }
}

//保存并得到session_id
int CSqlite3Saver::db_save_get_session_id(int uid, int level_id1, int level_id2)
{
    int sid;
    int rc;

    m_session.user_id = uid;
    m_session.level_id1 = level_id1;
    m_session.level_id2 = level_id2;
    struct tm *newtime;
    time_t ltime;
    time (&ltime);
    newtime = localtime(&ltime);
    sprintf(m_session.time, "%s", asctime(newtime));

    db_bind_insertSID();
    rc = db_execute_stmt(m_stInsertSID);
    if(rc == SQLITE_DONE)
    {
        sid = sqlite3_last_insert_rowid(db);
        return sid;
        /*
        db_bind_selectSID();
        rc = db_execute_stmt(m_stSelectSID);
        if(rc == SQLITE_ROW)
        {
            sid = sqlite3_column_int(m_stSelectSID, 0);
            return sid;
        }
        else
        {
            fprintf(stderr, "获取sid失败");
            return -1;
        }
        */
    }
    else
    {
        fprintf(stderr, "插入session_info失败");
        return -1;
    }
}
//保存attempt信息
int CSqlite3Saver::db_save_attemp_info(c2s_driveinfo_attempt* pAttempt)
{
    int rc;

    memcpy(&m_attempt, pAttempt, sizeof(c2s_driveinfo_attempt));

    db_bind_insertAID();
    rc = db_execute_stmt(m_stInsertAID);
    if(rc == SQLITE_DONE)
    {
        return 0;
    }
    else
    {
        fprintf(stderr, "插入attempt失败");
        return -1;
    }
}
//保存原始数据
int CSqlite3Saver::db_save_original_info(int level_id, c2s_driveinfo_attempt_original* pOriginal)
{
    return 0;
}

int CSqlite3Saver::db_save_original_info_stmt(c2s_driveinfo_attempt_original* pOriginal)
{
    int rc;

    memcpy(&m_original, pOriginal, sizeof(c2s_driveinfo_attempt_original));

    db_bind_insertOID();
    rc = db_execute_stmt(m_stInsertOID);
    if(rc == SQLITE_DONE)
        return 0;
    else
    {
        fprintf(stderr, "insert original失败");
        return -1;
    }
}

//保存偏好结果
int CSqlite3Saver::db_save_prefer_result(c2s_4afc_result &prefer)
{
    assert(prefer.sid > 0 && "sid错误");

    int pid = 0;
    int rc;

    memcpy(&m_prefer, &prefer, sizeof(c2s_4afc_result));

    db_bind_insertPrefer();
    rc = db_execute_stmt(m_stInsertPrefer);
    if(rc == SQLITE_DONE)
        return 1;
    else
    {
        fprintf(stderr, "insert prefer失败");
        return -1;
    }
}

int CSqlite3Saver::db_execute_stmt(sqlite3_stmt *stmt)
{
    int rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        rc = sqlite3_reset(stmt);
    }

    return rc;
}

void CSqlite3Saver::db_bind_selectUID(void)
{
    int rc = sqlite3_reset(m_stSelectUID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置selectUID参数失败");
        return;
    }
    rc = sqlite3_clear_bindings(m_stSelectUID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚selectUID参数失败");
        return;
    }

    if(sqlite3_bind_blob(m_stSelectUID, 1, m_userinfo.name, sizeof(m_userinfo.name)/sizeof(m_userinfo.name[0]), NULL))
        fprintf(stderr, "绑定selectUID参数失败\n");
    if(sqlite3_bind_int(m_stSelectUID, 2, m_userinfo.age))
        fprintf(stderr, "绑定selectUID参数失败\n");
    if(sqlite3_bind_int(m_stSelectUID, 3, m_userinfo.type))
        fprintf(stderr, "绑定selectUID参数失败\n");
    if(sqlite3_bind_int(m_stSelectUID, 4, m_userinfo.sex))
        fprintf(stderr, "绑定selectUID参数失败\n");
    if(sqlite3_bind_int(m_stSelectUID, 5, m_userinfo.frequency))
        fprintf(stderr, "绑定selectUID参数失败\n");
}

void CSqlite3Saver::db_bind_insertUID(void)
{
    int rc = sqlite3_reset(m_stInsertUsrInfo);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置insertUID参数失败");
        return;
    }

    rc = sqlite3_clear_bindings(m_stInsertUsrInfo);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚insertUID参数失败");
        return;
    }

    if(sqlite3_bind_blob(m_stInsertUsrInfo, 1, m_userinfo.name, sizeof(m_userinfo.name)/sizeof(m_userinfo.name[0]), NULL))
        fprintf(stderr, "绑定insertUID参数失败\n");
    if(sqlite3_bind_int(m_stInsertUsrInfo, 2, m_userinfo.age))
        fprintf(stderr, "绑定insertUID参数失败\n");
    if(sqlite3_bind_int(m_stInsertUsrInfo, 3, m_userinfo.type))
        fprintf(stderr, "绑定insertUID参数失败\n");
    if(sqlite3_bind_int(m_stInsertUsrInfo, 4, m_userinfo.sex))
        fprintf(stderr, "绑定insertUID参数失败\n");
    if(sqlite3_bind_int(m_stInsertUsrInfo, 5, m_userinfo.frequency))
        fprintf(stderr, "绑定insertUID参数失败\n");
}

void CSqlite3Saver::db_bind_selectLID(void)
{
    int rc = sqlite3_reset(m_stSelectLID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置selectLID参数失败");
        return;
    }
    rc = sqlite3_clear_bindings(m_stSelectLID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚selectLID参数失败");
        return;
    }

    if(sqlite3_bind_int(m_stSelectLID, 1, m_level.time_record))
        fprintf(stderr, "绑定selectLID参数失败\n");
    if(sqlite3_bind_int(m_stSelectLID, 2, m_level.num_attempt))
        fprintf(stderr, "绑定selectLID参数失败\n");
    if(sqlite3_bind_int(m_stSelectLID, 3, m_level.num_finished))
        fprintf(stderr, "绑定selectLID参数失败\n");
}

void CSqlite3Saver::db_bind_insertLID(void)
{
    int rc = sqlite3_reset(m_stInsertLID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置insertLID参数失败");
        return;
    }

    rc = sqlite3_reset(m_stInsertLID);
    rc = sqlite3_clear_bindings(m_stInsertLID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚insertLID参数失败");
        return;
    }

    if(rc = sqlite3_bind_double(m_stInsertLID, 1, m_level.partytime) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_double(m_stInsertLID, 2, m_level.finishProportionaTotal) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 3, m_level.num_attempt) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 4, m_level.num_finished) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 5, m_level.time_record) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_blob(m_stInsertLID, 6, m_level.trackname, sizeof(m_level.trackname)/sizeof(m_level.trackname[0]), NULL) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 7, m_level.difficult) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 8, m_level.skill) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 9, m_level.emotion) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 10, m_level.fun) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
    if(rc = sqlite3_bind_int(m_stInsertLID, 11, m_level.emotion_degree) != SQLITE_OK)
        fprintf(stderr, "绑定insertLID参数失败\n");
}

void CSqlite3Saver::db_bind_selectAID(void)
{
    int rc = sqlite3_reset(m_stSelectAID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置selectAID参数失败");
        return;
    }

    rc = sqlite3_clear_bindings(m_stSelectAID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚selectAID参数失败");
        return;
    }

    if(sqlite3_bind_int(m_stSelectAID, 1, m_attempt.level_id))
        fprintf(stderr, "绑定selectAID参数失败\n");
    if(sqlite3_bind_int(m_stSelectAID, 2, m_attempt.attemp_index))
        fprintf(stderr, "绑定selectAID参数失败\n");
}

void CSqlite3Saver::db_bind_insertAID(void)
{
    int rc = sqlite3_reset(m_stInsertAID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置insertAID参数失败");
        return;
    }

    rc = sqlite3_clear_bindings(m_stInsertAID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚insertAID参数失败");
        return;
    }

    if(sqlite3_bind_double(m_stInsertAID, 1, m_attempt.fTotaltime))
        fprintf(stderr, "绑定insertAID参数失败\n");
    if(sqlite3_bind_double(m_stInsertAID, 2, m_attempt.finishProportion))
        fprintf(stderr, "绑定insertAID参数失败\n");
    if(sqlite3_bind_int(m_stInsertAID, 3, m_attempt.attemp_index))
        fprintf(stderr, "绑定insertAID参数失败\n");
    if(sqlite3_bind_int(m_stInsertAID, 4, m_attempt.bFinished))
        fprintf(stderr, "绑定insertAID参数失败\n");
    if(sqlite3_bind_int(m_stInsertAID, 5, m_attempt.level_id))
        fprintf(stderr, "绑定insertAID参数失败\n");
    if(sqlite3_bind_int(m_stInsertAID, 6, m_attempt.time_record))
        fprintf(stderr, "绑定insertAID参数失败\n");
    if(sqlite3_bind_int(m_stInsertAID, 7, m_attempt.nOriginalTotal))
        fprintf(stderr, "绑定insertAID参数失败\n");
}

void CSqlite3Saver::db_bind_insertOID(void)
{
    int rc = sqlite3_reset(m_stInsertOID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置insertOID参数失败");
        return;
    }

    rc = sqlite3_clear_bindings(m_stInsertOID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚insertOID参数失败");
        return;
    }

    if(sqlite3_bind_int(m_stInsertOID, 1, m_original.nLevel_id))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 2, m_original.nAttempt_id))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 3, m_original.nAttempIndex))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 4, m_original.nOriginalIndex))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_double(m_stInsertOID, 5, m_original.pLinearSpeed[0]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 6, m_original.pLinearSpeed[1]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 7, m_original.pLinearSpeed[2]))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_double(m_stInsertOID, 8, m_original.pAngSpeed[0]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 9, m_original.pAngSpeed[1]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 10, m_original.pAngSpeed[2]))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_double(m_stInsertOID, 11, m_original.pRotate[0]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 12, m_original.pRotate[1]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 13, m_original.pRotate[2]))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_double(m_stInsertOID, 14, m_original.pDirection[0]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 15, m_original.pDirection[1]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 16, m_original.pDirection[2]))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_double(m_stInsertOID, 17, m_original.pPosition[0]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 18, m_original.pPosition[1]))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 19, m_original.pPosition[2]))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_double(m_stInsertOID, 20, m_original.fTime))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 21, m_original.fStep))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 22, m_original.fMotorSpeed))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_double(m_stInsertOID, 23, m_original.ray_f))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 24, m_original.ray_b))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 25, m_original.ray_l))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 26, m_original.ray_r))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 27, m_original.ray_fl))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 28, m_original.ray_fr))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 29, m_original.ray_up))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_double(m_stInsertOID, 30, m_original.ray_down))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_int(m_stInsertOID, 31, m_original.nColWhell))
        fprintf(stderr, "绑定insertOID参数失败");

    if(sqlite3_bind_int(m_stInsertOID, 32, m_original.bForward))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 33, m_original.bBack))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 34, m_original.bTurnLeft))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 35, m_original.bTurnRight))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 36, m_original.bLeap))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 37, m_original.bCollision))
        fprintf(stderr, "绑定insertOID参数失败");
    if(sqlite3_bind_int(m_stInsertOID, 38, m_original.track_type))
        fprintf(stderr, "绑定insertOID参数失败");
}

void CSqlite3Saver::db_bind_selectSID(void)
{
    int rc = sqlite3_reset(m_stSelectSID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置insertSID参数失败");
        return;
    }

    rc = sqlite3_clear_bindings(m_stSelectSID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚insertSID参数失败");
        return;
    }

    if(sqlite3_bind_blob(m_stSelectSID, 1, m_session.time, sizeof(m_session.time)/sizeof(m_session.time[0]), NULL))
        fprintf(stderr, "绑定selectSID参数失败\n");
    if(sqlite3_bind_int(m_stSelectSID, 2, m_session.level_id1))
        fprintf(stderr, "绑定selectSID参数失败\n");
    if(sqlite3_bind_int(m_stSelectSID, 3, m_session.level_id2))
        fprintf(stderr, "绑定selectSID参数失败\n");
}

void CSqlite3Saver::db_bind_insertSID(void)
{
    int rc = sqlite3_reset(m_stInsertSID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置insertSID参数失败");
        return;
    }

    rc = sqlite3_clear_bindings(m_stInsertSID);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚insertSID参数失败");
        return;
    }

    if(sqlite3_bind_int(m_stInsertSID, 1, m_session.user_id))
        fprintf(stderr, "绑定insertSID参数失败\n");
    if(sqlite3_bind_blob(m_stInsertSID, 2, m_session.time, sizeof(m_session.time)/sizeof(m_session.time[0]), NULL))
        fprintf(stderr, "绑定insertSID参数失败\n");
    if(sqlite3_bind_int(m_stInsertSID, 3, m_session.level_id1))
        fprintf(stderr, "绑定insertSID参数失败\n");
    if(sqlite3_bind_int(m_stInsertSID, 4, m_session.level_id2))
        fprintf(stderr, "绑定insertSID参数失败\n");
}

void CSqlite3Saver::db_bind_insertPrefer(void)
{
    int rc = sqlite3_reset(m_stInsertPrefer);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置insertPrefer参数失败");
        return;
    }

    rc = sqlite3_clear_bindings(m_stInsertPrefer);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚insertPrefer参数失败");
        return;
    }

    if(sqlite3_bind_int(m_stInsertPrefer, 1, m_prefer.sid))
        fprintf(stderr, "绑定inserPrefer参数失败\n");
    if(sqlite3_bind_int(m_stInsertPrefer, 2, m_prefer.prefer_type))
        fprintf(stderr, "绑定inserPrefer参数失败\n");
    if(sqlite3_bind_int(m_stInsertPrefer, 3, m_prefer.prefer_result))
        fprintf(stderr, "绑定inserPrefer参数失败\n");
}
