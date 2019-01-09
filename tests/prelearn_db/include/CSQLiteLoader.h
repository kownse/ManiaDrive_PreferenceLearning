#ifndef CSQLITELOADER_H
#define CSQLITELOADER_H

#include "IDBLoader.h"
#include "sqlite3.h"
#include <string>

class DLL_EXPORT CSQLiteLoader : public IDBLoader
{
public:
    CSQLiteLoader(const char* szFileName);
    ~CSQLiteLoader();

    DLL_EXPORT int load_session_data (std::vector<c2s_driveinfo_attempt> &attemptStore,
                                       std::vector<c2s_driveinfo_level> &levelStore,
                                       std::vector<c2s_driveinfo_attempt_original> &originalStore,
                                       c2s_4afc_result &prefer,
                                       c2s_user_info &user,
                                       session_info &session);

    void SetFileName(const char *szFileName) { m_szFileName = szFileName; }

private:
    bool Init(void);
    bool prepare_stmt(void);

    ///按照sid读出数据
    bool LoadDataRecursiveBySID(int sid,
                                std::vector<c2s_driveinfo_attempt> &attemptStore,
                                std::vector<c2s_driveinfo_level> &levelStore,
                                std::vector<c2s_driveinfo_attempt_original> &originalStore,
                                c2s_4afc_result &prefer);

    bool LoadDataRecursiveByLID(int lid,
                        std::vector<c2s_driveinfo_attempt> &attemptStore,
                        std::vector<c2s_driveinfo_attempt_original> &originalStore);

    ///读取相关函数
    //用户信息相关
    bool db_get_user_count(void);
    int db_get_user_data(c2s_user_info& user);

    //session信息相关
    int db_get_session_data_by_uid(int uid, session_info &session);
    bool db_get_session_data_by_sid(int sid, session_info &session);

    //level信息相关
    bool db_get_level_data_by_lid(int lid, c2s_driveinfo_level &level);

    //attempt信息相关
    bool db_get_attempt_count_by_lid(int lid);
    int db_get_attempt_data_by_lid(int lid, c2s_driveinfo_attempt &attempt);

    //original信息相关
    bool db_get_original_count_by_sid(int sid);
    int db_get_original_data_by_aid(int aid, c2s_driveinfo_attempt_original &original);

    //偏好相关
    bool db_get_prefer_data_by_sid(int sid, c2s_4afc_result &prefer);

private:
    int db_execute_stmt(sqlite3_stmt *stmt);

    bool db_bind_select_level_lid(int sid);
    bool db_bind_select_session_uid(int uid);
    bool db_bind_select_attempt_lid(int lid);
    bool db_bind_select_original_aid(int aid);
    bool db_bind_select_prefer_sid(int sid);

private:
    c2s_user_info m_user;
    c2s_driveinfo_level m_level;
    c2s_driveinfo_attempt m_attempt;
    c2s_4afc_result m_prefer;
    c2s_driveinfo_attempt_original m_original;
    session_info m_session;

    std::string m_szFileName;

    sqlite3 *db;

    sqlite3_stmt *m_stSelectUser;
    sqlite3_stmt *m_stSelectLevel;
    sqlite3_stmt *m_stSelectSession;
    sqlite3_stmt *m_stSelectAttempt;
    sqlite3_stmt *m_stSelectOriginal;
    sqlite3_stmt *m_stSelectPrefer;


    bool m_bEnable;

    bool m_bChangeUser;
    bool m_bRestSession;

};

#endif // CSQLITELOADER_H
