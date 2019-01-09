#ifndef PRE_SQLITE_H_INCLUDED
#define PRE_SQLITE_H_INCLUDED

#include "windows.h"
#include "WinNT.h"
//#include "WinDef.h"
#include "sqlite3.h"
#include <vector>
//#include "../raydium/PreferenceModeling/globle.h"
//#include "../globle.h"
#include "IDBSaver.h"

class DLL_EXPORT CSqlite3Saver : public IDBSaver
{
public:
    CSqlite3Saver(void);
    ~CSqlite3Saver(void);

    ///存储数据
    DLL_EXPORT int save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                        std::vector<c2s_driveinfo_level> &levelStore,
                        std::vector<c2s_driveinfo_attempt_original> &originalStore,
                        c2s_4afc_result &prefer,
                        c2s_user_info &user);

    DLL_EXPORT int save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                        std::vector<c2s_driveinfo_level> &levelStore,
                        std::vector<c2s_driveinfo_attempt_original> &originalStore,
                        c2s_4afc_result &prefer,
                        c2s_user_info &user,
                        session_info &session);

private:
    bool Init(void);
    bool InitDB(char *db_name);

    ///存储相关函数
    bool prepare_stmt(void);
    //保存并得到玩家id
    int db_get_uid(c2s_user_info &info);
    //保存并得到level_id
    int db_save_get_level_id(c2s_driveinfo_level* pLevel);
    //得到attempt_id
    int db_get_attempt_id(int level_id, int attempt_index);
    //带缓存的得到AID
    int db_get_attempt_id_cache(int level_id, int attempt_index);

    //保存并得到session_id
    int db_save_get_session_id(int uid, int level_id1, int level_id2);
    //保存attempt信息
    int db_save_attemp_info(c2s_driveinfo_attempt* pAttempt);
    //保存原始数据
    int db_save_original_info(int level_id, c2s_driveinfo_attempt_original* pOriginal);
    //用prepared_statement存储
    int db_save_original_info_stmt(c2s_driveinfo_attempt_original* pOriginal);

    //保存偏好结果
    int db_save_prefer_result(c2s_4afc_result &prefer);


    ///辅助函数
    //自动reset的执行
    int db_execute_stmt(sqlite3_stmt *stmt);

    ///绑定各种参数
    void db_bind_selectUID(void);
    void db_bind_insertUID(void);
    void db_bind_selectLID(void);
    void db_bind_insertLID(void);
    void db_bind_selectAID(void);
    void db_bind_insertAID(void);
    void db_bind_insertOID(void);
    void db_bind_selectSID(void);
    void db_bind_insertSID(void);
    void db_bind_insertPrefer(void);

private:
    TCHAR m_szName[128];
    c2s_user_info m_userinfo;       //缓存的用户信息
    c2s_driveinfo_level m_level;
    c2s_driveinfo_attempt m_attempt;
    c2s_driveinfo_attempt_original m_original;
    session_info m_session;
    c2s_4afc_result m_prefer;

    sqlite3 *db;
    sqlite3_stmt *m_stSelectUID;
    sqlite3_stmt *m_stInsertUsrInfo;
    sqlite3_stmt *m_stSelectLID;
    sqlite3_stmt *m_stInsertLID;
    sqlite3_stmt *m_stInsertAID;
    sqlite3_stmt *m_stSelectAID;
    sqlite3_stmt *m_stInsertOID;
    sqlite3_stmt *m_stSelectSID;
    sqlite3_stmt *m_stInsertSID;
    sqlite3_stmt *m_stInsertPrefer;

    bool m_bEnable;
};


#endif // PRE_SQLITE_H_INCLUDED
