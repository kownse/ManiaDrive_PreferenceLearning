#include "CSQLiteLoader.h"
#include <assert.h>
#include <memory.h>
#include <cstdio>

#define SAFE_FINALIZE_STMT(stmt)  if(stmt) sqlite3_finalize(stmt)

CSQLiteLoader::CSQLiteLoader(const char* szFileName)
    : m_szFileName(szFileName),
    m_bEnable(false),
    m_bChangeUser(true)
{
    //ctor
    Init();
}

CSQLiteLoader::~CSQLiteLoader()
{
    //dtor
    if(db)
    {
        SAFE_FINALIZE_STMT(m_stSelectUser);
        SAFE_FINALIZE_STMT(m_stSelectLevel);
        SAFE_FINALIZE_STMT(m_stSelectSession);
        SAFE_FINALIZE_STMT(m_stSelectAttempt);
        SAFE_FINALIZE_STMT(m_stSelectOriginal);
        SAFE_FINALIZE_STMT(m_stSelectPrefer);

        sqlite3_close(db);
    }
}

bool CSQLiteLoader::Init(void)
{
    assert(m_szFileName.size() != 0 && "数据库文件地址不能为空");

    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(m_szFileName.c_str(), &db);
    if( rc )
    {
        fprintf(stderr, "不能打开数据库文件: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        m_bEnable = false;
        return m_bEnable;
    }

    m_bEnable = prepare_stmt();

    if(m_bEnable)
    {
        fprintf(stdout, "SQLITE准备成功\n");
    }

    return m_bEnable;
}

bool CSQLiteLoader::prepare_stmt(void)
{
    int rc;

    ///select user
    {
        static const char *sqlSelectUser = "select * from user;";
        rc = sqlite3_prepare_v2(db, sqlSelectUser, -1, &m_stSelectUser, 0);
        if(rc != SQLITE_OK)
        {
            fprintf(stderr, "准备m_stSelectUser失败\n");
            return false;
        }
    }

    ///select level
    {
        static const char *sqlSelectLevel = "select * from level where level_id = ?";
        rc = sqlite3_prepare_v2(db, sqlSelectLevel, -1, &m_stSelectLevel, 0);
        if(rc != SQLITE_OK)
        {
            fprintf(stderr, "准备m_stSelectLevel失败\n");
            return false;
        }
    }

    ///select session
    {
        static const char *sqlSelectSession = "select * from session where uid = ?";
        rc = sqlite3_prepare_v2(db, sqlSelectSession, -1, &m_stSelectSession, 0);
        if(rc != SQLITE_OK)
        {
            fprintf(stderr, "准备m_stSelectSession失败\n");
            return false;
        }
    }

    ///select attempt
    {
        static const char *sqlSelectAttempt = "select * from attempt where level_id = ?";
        rc = sqlite3_prepare_v2(db, sqlSelectAttempt, -1, &m_stSelectAttempt, 0);
        if(rc != SQLITE_OK)
        {
            fprintf(stderr, "准备m_stSelectAttempt失败\n");
            return false;
        }
    }

    ///select original
    {
        static const char *sqlSelectOriginal = "select * from original where attempt_id = ?";
        rc = sqlite3_prepare_v2(db, sqlSelectOriginal, -1, &m_stSelectOriginal, 0);
        if(rc != SQLITE_OK)
        {
            fprintf(stderr, "准备m_stSelectOriginal失败\n");
            return false;
        }
    }

    ///select prefer
    {
        static const char *sqlSelectPrefer = "select * from prefer_result where sid = ?";
        rc = sqlite3_prepare_v2(db, sqlSelectPrefer, -1, &m_stSelectPrefer, 0);
        if(rc != SQLITE_OK)
        {
            fprintf(stderr, "准备m_StSelectPrefer失败\n");
            return false;
        }
    }

    return true;
}



int CSQLiteLoader::load_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                                      std::vector<c2s_driveinfo_level> &levelStore,
                                      std::vector<c2s_driveinfo_attempt_original> &originalStore,
                                      c2s_4afc_result &prefer,
                                      c2s_user_info &user,
                                      session_info &session)
{
    assert( attemptStore.size() == 0 && "attemptStore不为空");
    assert( levelStore.size() == 0 && "levelStore不为空");
    assert( originalStore.size() == 0 && "originalStore不为空");

    /*    std::vector<c2s_driveinfo_attempt> attemptStoreTmp;
        std::vector<c2s_driveinfo_level> levelStoreTmp;
        std::vector<c2s_driveinfo_attempt_original> originalStoreTmp;
    */
    int rc;

    if(m_bChangeUser)
    {
        if((rc = db_get_user_data(user)) == PRE_DB_ROWEND)
        {
            fprintf(stdout, "用户遍历完成\n");
            return PRE_DB_ROWEND;
        }
        else
        {
            //重置
            if(!db_bind_select_session_uid(user.user_id))
                return PRE_DB_ERROR;
        }
    }

    //用uid读取session
    //session_info sessionTmp;

    if((rc = db_get_session_data_by_uid(user.user_id, session)) == PRE_DB_OK)
    {
        //现在不需要重新读取用户
        m_bChangeUser = false;
        //用session读取level

        //读取偏好结果
        if(!db_bind_select_prefer_sid(session.session_id))
            return PRE_DB_ERROR;

        c2s_4afc_result preferTmp;
        if(db_get_prefer_data_by_sid(session.session_id, preferTmp) == PRE_DB_OK)
        {
            memcpy(&prefer, &preferTmp, sizeof(c2s_4afc_result));
        }
        else
        {
            return PRE_DB_USER_END;
        }
        session.session_id = 0;

        c2s_driveinfo_level levelTmp;

        if(db_get_level_data_by_lid(session.level_id1, levelTmp) == PRE_DB_OK)
        {
            levelTmp.session_id = 0;
            levelTmp.level_id = 0;
            //用level读取attempt
            if(!db_bind_select_attempt_lid(session.level_id1))
                return PRE_DB_ERROR;

            c2s_driveinfo_attempt attemptTmp;
            while((rc = db_get_attempt_data_by_lid(session.level_id1, attemptTmp)) == PRE_DB_OK)
            {
                attemptTmp.session_id = levelTmp.session_id;
                attemptTmp.level_id = levelTmp.level_id;
                if(!db_bind_select_original_aid(attemptTmp.attempt_id))
                    return PRE_DB_ERROR;

                c2s_driveinfo_attempt_original originalTmp;
                while((rc = db_get_original_data_by_aid(attemptTmp.attempt_id, originalTmp)) == PRE_DB_OK)
                {
                    originalTmp.nLevel_id = levelTmp.level_id;
                    originalStore.push_back(originalTmp);
                }

                if(rc != PRE_DB_ROWEND)
                    return PRE_DB_ERROR;

                attemptStore.push_back(attemptTmp);
            }
            if(rc != PRE_DB_ROWEND)
                return PRE_DB_ERROR;

            levelStore.push_back(levelTmp);
        }
        else
        {
            return PRE_DB_ERROR;
        }

        if(db_get_level_data_by_lid(session.level_id2, levelTmp) == PRE_DB_OK)
        {
            levelTmp.session_id = 0;
            levelTmp.level_id = 1;
            if(!db_bind_select_attempt_lid(session.level_id2))
                return PRE_DB_ERROR;

            c2s_driveinfo_attempt attemptTmp;
            while((rc = db_get_attempt_data_by_lid(session.level_id2, attemptTmp)) == PRE_DB_OK)
            {
                attemptTmp.session_id = levelTmp.session_id;
                attemptTmp.level_id = levelTmp.level_id;
                if(!db_bind_select_original_aid(attemptTmp.attempt_id))
                    return PRE_DB_ERROR;

                c2s_driveinfo_attempt_original originalTmp;
                while((rc = db_get_original_data_by_aid(attemptTmp.attempt_id, originalTmp)) == PRE_DB_OK)
                {
                    originalTmp.nLevel_id = levelTmp.level_id;
                    originalStore.push_back(originalTmp);
                }
                if(rc != PRE_DB_ROWEND)
                    return PRE_DB_ERROR;

                attemptStore.push_back(attemptTmp);
            }
            if(rc != PRE_DB_ROWEND)
                return PRE_DB_ERROR;

            levelStore.push_back(levelTmp);
        }
        else
        {
            return PRE_DB_ERROR;
        }
    }
    else if(rc = PRE_DB_ROWEND)
    {
        m_bChangeUser = true;
        return PRE_DB_SESSION_END;
    }
    else
    {
        return PRE_DB_ERROR;
    }

    return PRE_DB_OK;
}

int CSQLiteLoader::db_execute_stmt(sqlite3_stmt *stmt)
{
    int rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        rc = sqlite3_reset(stmt);
    }

    return rc;
}

bool CSQLiteLoader::LoadDataRecursiveBySID(int sid,
        std::vector<c2s_driveinfo_attempt> &attemptStore,
        std::vector<c2s_driveinfo_level> &levelStore,
        std::vector<c2s_driveinfo_attempt_original> &originalStore,
        c2s_4afc_result &prefer)
{
    assert( attemptStore.size() == 0 && "attemptStore不为空");
    assert( levelStore.size() == 0 && "levelStore不为空");
    assert( originalStore.size() == 0 && "originalStore不为空");

    std::vector<c2s_driveinfo_attempt> attemptTmp;
    std::vector<c2s_driveinfo_level> levelTmp;
    std::vector<c2s_driveinfo_attempt_original> originalTmp;

    session_info sessionTmp;
    memset(&sessionTmp, 0, sizeof(session_info));
    if(!db_get_session_data_by_sid(sid, sessionTmp))
    {
        return false;
    }

    if(LoadDataRecursiveByLID(sessionTmp.level_id1, attemptTmp, originalTmp))
    {
        c2s_driveinfo_level level;
        memset(&level, 0, sizeof(c2s_driveinfo_level));
        db_get_level_data_by_lid(sessionTmp.level_id1, level);
        levelTmp.push_back(level);
    }
    else
    {
        return false;
    }

    if(LoadDataRecursiveByLID(sessionTmp.level_id2, attemptTmp, originalTmp))
    {
        c2s_driveinfo_level level;
        memset(&level, 0, sizeof(c2s_driveinfo_level));
        db_get_level_data_by_lid(sessionTmp.level_id2, level);
        levelTmp.push_back(level);
    }
    else
    {
        return false;
    }

    std::copy_backward(levelTmp.begin(), levelTmp.end(), levelStore.end());


    return false;
}

bool CSQLiteLoader::LoadDataRecursiveByLID(int lid,
        std::vector<c2s_driveinfo_attempt> &attemptStore,
        std::vector<c2s_driveinfo_attempt_original> &originalStore)
{
    return false;
}

//用户信息相关
bool CSQLiteLoader::db_get_user_count(void)
{
    return -1;
}

int CSQLiteLoader::db_get_user_data(c2s_user_info& user)
{
    int rc;

    rc = db_execute_stmt(m_stSelectUser);
    if(rc == SQLITE_ROW)
    {
        //读得一个记录
        int row = sqlite3_column_count(m_stSelectUser);
        if(row != 6)
            return PRE_DB_ERROR;

        memset(&m_user, 0, sizeof(c2s_user_info));

        m_user.user_id = sqlite3_column_int(m_stSelectUser, 0);
        const char* szName = (const char*)(sqlite3_column_text(m_stSelectUser, 1));
        strcpy(m_user.name, szName);
        m_user.age = sqlite3_column_int(m_stSelectUser, 2);
        m_user.type = sqlite3_column_int(m_stSelectUser, 3);
        m_user.sex = sqlite3_column_int(m_stSelectUser, 4);
        m_user.frequency = sqlite3_column_int(m_stSelectUser, 5);

        memcpy(&user, &m_user, sizeof(c2s_user_info));

        //按照uid读取session

        return PRE_DB_OK;
    }
    else if(rc == SQLITE_DONE)
    {
        return PRE_DB_ROWEND;
    }
    else
    {
        return PRE_DB_ERROR;
    }
}

//session信息相关
int CSQLiteLoader::db_get_session_data_by_uid(int uid, session_info &session)
{
    int rc;
    rc = db_execute_stmt(m_stSelectSession);
    if(rc == SQLITE_ROW)
    {
        //读得一个记录
        int row = sqlite3_column_count(m_stSelectSession);
        if(row != 5)
            return PRE_DB_ERROR;

        memset(&m_session, 0, sizeof(session_info));

        m_session.session_id = sqlite3_column_int(m_stSelectSession, 0);
        m_session.user_id = sqlite3_column_int(m_stSelectSession, 1);
        const char* szTime = (const char*)(sqlite3_column_text(m_stSelectSession, 2));
        strcpy(m_session.time, szTime);
        m_session.level_id1 = sqlite3_column_int(m_stSelectSession, 3);
        m_session.level_id2 = sqlite3_column_int(m_stSelectSession, 4);

        memcpy(&session, &m_session, sizeof(session_info));

        return PRE_DB_OK;
    }
    else if(rc == SQLITE_DONE)
    {
        return PRE_DB_ROWEND;
    }
    else
    {
        return PRE_DB_ERROR;
    }
}

bool CSQLiteLoader::db_get_session_data_by_sid(int sid, session_info &session)
{
    return -1;
}

//level信息相关
bool CSQLiteLoader::db_get_level_data_by_lid(int lid, c2s_driveinfo_level &level)
{
    if(!db_bind_select_level_lid(lid))
        return PRE_DB_ERROR;

    int rc;
    rc = db_execute_stmt(m_stSelectLevel);
    if(rc == SQLITE_ROW)
    {
        //读得一个记录
        int row = sqlite3_column_count(m_stSelectLevel);
        if(row != 12)
            return PRE_DB_ERROR;

        memset(&m_level, 0, sizeof(c2s_driveinfo_level));

        m_level.level_id = sqlite3_column_int(m_stSelectLevel, 0);
        m_level.partytime = sqlite3_column_double(m_stSelectLevel, 1);
        m_level.finishProportionaTotal = sqlite3_column_double(m_stSelectLevel, 2);
        m_level.num_attempt = sqlite3_column_int(m_stSelectLevel, 3);
        m_level.num_finished = sqlite3_column_int(m_stSelectLevel, 4);
        m_level.time_record = sqlite3_column_int(m_stSelectLevel, 5);
        const char* szTrackName = (const char*)(sqlite3_column_text(m_stSelectLevel, 6));
        strcpy(m_level.trackname, szTrackName);

        m_level.difficult = sqlite3_column_int(m_stSelectLevel, 7);
        m_level.skill = sqlite3_column_int(m_stSelectLevel, 8);
        m_level.emotion = sqlite3_column_int(m_stSelectLevel, 9);
        m_level.fun = sqlite3_column_int(m_stSelectLevel, 10);
        m_level.emotion_degree = sqlite3_column_int(m_stSelectLevel, 11);

        memcpy(&level, &m_level, sizeof(c2s_driveinfo_level));

        return PRE_DB_OK;
    }
    else if(rc == SQLITE_DONE)
    {
        return PRE_DB_ROWEND;
    }
    else
    {
        return PRE_DB_ERROR;
    }
}

//attempt信息相关
bool CSQLiteLoader::db_get_attempt_count_by_lid(int lid)
{
    return -1;
}

int CSQLiteLoader::db_get_attempt_data_by_lid(int lid, c2s_driveinfo_attempt &attempt)
{
    int rc;
    rc = db_execute_stmt(m_stSelectAttempt);
    if(rc == SQLITE_ROW)
    {
        //读得一个记录
        int row = sqlite3_column_count(m_stSelectAttempt);
        if(row != 8)
            return PRE_DB_ERROR;

        memset(&m_attempt, 0, sizeof(c2s_driveinfo_attempt));

        m_attempt.attempt_id = sqlite3_column_int(m_stSelectAttempt, 0);
        m_attempt.fTotaltime = sqlite3_column_double(m_stSelectAttempt, 1);
        m_attempt.finishProportion = sqlite3_column_double(m_stSelectAttempt, 2);
        m_attempt.attemp_index = sqlite3_column_int(m_stSelectAttempt, 3);
        m_attempt.bFinished = sqlite3_column_int(m_stSelectAttempt, 4);
        m_attempt.level_id = sqlite3_column_int(m_stSelectAttempt, 5);
        m_attempt.time_record = sqlite3_column_int(m_stSelectAttempt, 6);
        m_attempt.nOriginalTotal = sqlite3_column_int(m_stSelectAttempt, 7);

        memcpy(&attempt, &m_attempt, sizeof(c2s_driveinfo_attempt));

        return PRE_DB_OK;
    }
    else if(rc == SQLITE_DONE)
    {
        return PRE_DB_ROWEND;
    }
    else
    {
        return PRE_DB_ERROR;
    }
}

//original信息相关
bool CSQLiteLoader::db_get_original_count_by_sid(int sid)
{
    return -1;
}

int CSQLiteLoader::db_get_original_data_by_aid(int aid, c2s_driveinfo_attempt_original &original)
{
    int rc;
    rc = db_execute_stmt(m_stSelectOriginal);
    if(rc == SQLITE_ROW)
    {
        //读得一个记录
        int row = sqlite3_column_count(m_stSelectOriginal);
        if(row != 39)
            return PRE_DB_ERROR;

        memset(&m_original, 0, sizeof(c2s_driveinfo_attempt_original));

        m_original.ori_id = sqlite3_column_int(m_stSelectOriginal, 0);
        m_original.nLevel_id = sqlite3_column_int(m_stSelectOriginal, 1);
        m_original.nAttempt_id = sqlite3_column_int(m_stSelectOriginal, 2);
        m_original.nAttempIndex = sqlite3_column_int(m_stSelectOriginal, 3);
        m_original.nOriginalIndex = sqlite3_column_int(m_stSelectOriginal, 4);

        m_original.pLinearSpeed[0] = sqlite3_column_double(m_stSelectOriginal, 5);
        m_original.pLinearSpeed[1] = sqlite3_column_double(m_stSelectOriginal, 6);
        m_original.pLinearSpeed[2] = sqlite3_column_double(m_stSelectOriginal, 7);

        m_original.pAngSpeed[0] = sqlite3_column_double(m_stSelectOriginal, 8);
        m_original.pAngSpeed[1] = sqlite3_column_double(m_stSelectOriginal, 9);
        m_original.pAngSpeed[2] = sqlite3_column_double(m_stSelectOriginal, 10);

        m_original.pRotate[0] = sqlite3_column_double(m_stSelectOriginal, 11);
        m_original.pRotate[1] = sqlite3_column_double(m_stSelectOriginal, 12);
        m_original.pRotate[2] = sqlite3_column_double(m_stSelectOriginal, 13);

        m_original.pDirection[0] = sqlite3_column_double(m_stSelectOriginal, 14);
        m_original.pDirection[1] = sqlite3_column_double(m_stSelectOriginal, 15);
        m_original.pDirection[2] = sqlite3_column_double(m_stSelectOriginal, 16);

        m_original.pPosition[0] = sqlite3_column_double(m_stSelectOriginal, 17);
        m_original.pPosition[1] = sqlite3_column_double(m_stSelectOriginal, 18);
        m_original.pPosition[2] = sqlite3_column_double(m_stSelectOriginal, 19);

        m_original.fTime = sqlite3_column_double(m_stSelectOriginal, 20);
        m_original.fStep = sqlite3_column_double(m_stSelectOriginal, 21);

        m_original.fMotorSpeed = sqlite3_column_double(m_stSelectOriginal, 22);

        m_original.ray_f = sqlite3_column_double(m_stSelectOriginal, 23);
        m_original.ray_b = sqlite3_column_double(m_stSelectOriginal, 24);
        m_original.ray_l = sqlite3_column_double(m_stSelectOriginal, 25);
        m_original.ray_r = sqlite3_column_double(m_stSelectOriginal, 26);
        m_original.ray_fl = sqlite3_column_double(m_stSelectOriginal, 27);
        m_original.ray_fr = sqlite3_column_double(m_stSelectOriginal, 28);
        m_original.ray_up = sqlite3_column_double(m_stSelectOriginal, 29);
        m_original.ray_down = sqlite3_column_double(m_stSelectOriginal, 30);

        m_original.nColWhell = sqlite3_column_int(m_stSelectOriginal, 31);

        m_original.bForward = sqlite3_column_int(m_stSelectOriginal, 32);
        m_original.bBack = sqlite3_column_int(m_stSelectOriginal, 33);
        m_original.bTurnLeft = sqlite3_column_int(m_stSelectOriginal, 34);
        m_original.bTurnRight = sqlite3_column_int(m_stSelectOriginal, 35);
        m_original.bLeap = sqlite3_column_int(m_stSelectOriginal, 36);
        m_original.bCollision = sqlite3_column_int(m_stSelectOriginal, 37);
        m_original.track_type = sqlite3_column_int(m_stSelectOriginal, 38);


        memcpy(&original, &m_original, sizeof(c2s_driveinfo_attempt_original));

        return PRE_DB_OK;
    }
    else if(rc == SQLITE_DONE)
    {
        return PRE_DB_ROWEND;
    }
    else
    {
        return PRE_DB_ERROR;
    }
}

//偏好相关
bool CSQLiteLoader::db_get_prefer_data_by_sid(int sid, c2s_4afc_result &prefer)
{
    int rc;
    rc = db_execute_stmt(m_stSelectPrefer);
    if(rc == SQLITE_ROW)
    {
        //读得一个记录
        int row = sqlite3_column_count(m_stSelectPrefer);
        if(row != 4)
            return PRE_DB_ERROR;

        memset(&m_prefer, 0, sizeof(c2s_4afc_result));

        m_prefer.pid = sqlite3_column_int(m_stSelectPrefer, 0);
        m_prefer.sid = sqlite3_column_int(m_stSelectPrefer, 1);
        m_prefer.prefer_type = sqlite3_column_int(m_stSelectPrefer, 2);
        m_prefer.prefer_result = sqlite3_column_int(m_stSelectPrefer, 3);

        memcpy(&prefer, &m_prefer, sizeof(c2s_4afc_result));

        return PRE_DB_OK;
    }
    else if(rc == SQLITE_DONE)
    {
        return PRE_DB_ROWEND;
    }
    else
    {
        return PRE_DB_ERROR;
    }
}

bool CSQLiteLoader::db_bind_select_level_lid(int lid)
{
    int rc = sqlite3_reset(m_stSelectLevel);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置m_stSelectLevel参数失败\n");
        return false;
    }

    rc = sqlite3_clear_bindings(m_stSelectLevel);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚m_stSelectLevel参数失败\n");
        return false;
    }

    if(sqlite3_bind_int(m_stSelectLevel, 1, lid))
    {
        fprintf(stderr, "绑定m_stSelectLevel参数失败\n");
        return false;
    }

    return true;
}

bool CSQLiteLoader::db_bind_select_session_uid(int uid)
{
    int rc = sqlite3_reset(m_stSelectSession);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置m_stSelectSession参数失败\n");
        return false;
    }

    rc = sqlite3_clear_bindings(m_stSelectSession);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚m_stSelectSession参数失败\n");
        return false;
    }

    if(sqlite3_bind_int(m_stSelectSession, 1, uid))
    {
        fprintf(stderr, "绑定m_stSelectSession参数失败\n");
        return false;
    }

    return true;
}

bool CSQLiteLoader::db_bind_select_attempt_lid(int lid)
{
    int rc = sqlite3_reset(m_stSelectAttempt);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置m_stSelectAttempt参数失败\n");
        return false;
    }

    rc = sqlite3_clear_bindings(m_stSelectAttempt);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚m_stSelectAttempt参数失败\n");
        return false;
    }

    if(sqlite3_bind_int(m_stSelectAttempt, 1, lid))
    {
        fprintf(stderr, "绑定m_stSelectAttempt参数失败\n");
        return false;
    }

    return true;
}

bool CSQLiteLoader::db_bind_select_original_aid(int aid)
{
    int rc = sqlite3_reset(m_stSelectOriginal);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置m_stSelectOriginal参数失败\n");
        return false;
    }

    rc = sqlite3_clear_bindings(m_stSelectOriginal);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚m_stSelectOriginal参数失败\n");
        return false;
    }

    if(sqlite3_bind_int(m_stSelectOriginal, 1, aid))
    {
        fprintf(stderr, "绑定m_stSelectOriginal参数失败\n");
        return false;
    }

    return true;
}

bool CSQLiteLoader::db_bind_select_prefer_sid(int sid)
{
    int rc = sqlite3_reset(m_stSelectPrefer);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "重置m_stSelectPrefer参数失败\n");
        return false;
    }

    rc = sqlite3_clear_bindings(m_stSelectPrefer);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "清楚m_stSelectPrefer参数失败\n");
        return false;
    }

    if(sqlite3_bind_int(m_stSelectPrefer, 1, sid))
    {
        fprintf(stderr, "绑定m_stSelectPrefer参数失败\n");
        return false;
    }

    return true;
}
