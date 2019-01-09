#ifndef _MYSQLCONNECT_H_
#define _MYSQLCONNECT_H_

#include <winsock.h>
#include <vector>
#include <string>
#include "mysql.h"
#include "IDBSaver.h"
#include "pre_analyse/define.h"


class DLL_EXPORT CMySQLSaver : public IDBSaver
{
public:
    CMySQLSaver();
    ~CMySQLSaver();

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

    DLL_EXPORT bool db_save_trackresult_stmt(CTrackResult& result);

private:
    unsigned char connect_db(void);
    unsigned char disconnect_db(void);

    //���沢�õ����id
    int db_get_uid(char* name, char* ip);
    int db_save_get_uid(c2s_user_info &info);
    int db_save_get_uid_stmt(c2s_user_info &info);
    int db_get_uid_stmt(c2s_user_info &info);

    //���沢�õ�level_id
    int db_save_get_level_id(c2s_driveinfo_level &level);
    int db_save_get_level_id_stmt(c2s_driveinfo_level &lLevel);
    int db_save_level_id_stmt(c2s_driveinfo_level &lLevel);
    int db_get_level_id_stmt(c2s_driveinfo_level &lLevel);


    //�õ�attempt_id
    int db_get_attempt_id(int level_id, int attempt_index);
    int db_get_attempt_id_stmt(c2s_driveinfo_attempt_original &original);
    //�����湦�ܵĵõ�ID��ͨ��������жϼ������ݿ�����Ĵ���
    int db_get_attempt_id_cache(int level_id, int attempt_index);
//����attempt��Ϣ
    int db_save_attemp_info(int level_id, c2s_driveinfo_attempt* pAttempt);
    //����attempt��Ϣ
    int db_save_attemp_info(c2s_driveinfo_attempt* pAttempt);
    int db_save_attempt_info_stmt(c2s_driveinfo_attempt &attempt);

    //���沢�õ�session_id
    int db_save_get_session_id(int uid, int level_id1, int level_id2);
    int db_save_get_session_id_stmt(session_info &session);
    int db_save_session_id_stmt(session_info &session);
    int db_get_session_id_stmt(session_info &session);


    //����ƫ�ý��
    int db_save_prefer_result(int sid, int type_id, int result);
    int db_save_prefer_result_stmt(c2s_4afc_result &prefer);

//����ԭʼ����
    int db_save_original_info(c2s_driveinfo_attempt_original* pOriginal);
//�������б���ԭʼ����
    int db_save_original_info_transaction(c2s_driveinfo_attempt_original* pOriginal);
    //��prepared_statement�洢
    int db_save_original_info_stmt(c2s_driveinfo_attempt_original* pOriginal);

    //bindԭʼ���ݴ洢��stmt
    bool db_bind_insert_original(void);


    ///��������
    //��ʼ��+�󶨲���
    bool db_mysql_stmt_init(MYSQL_STMT **pStmt, const char *sql, const char* szName);
    //�󶨽��
    bool db_mysql_stmt_bind_result(MYSQL_STMT **pStmt, MYSQL_RES **pRes, const int num_result, MYSQL_BIND *bind, const char *szName);
    //�󶨲���
    bool db_mysql_stmt_bind_param(MYSQL_STMT **pStmt, const int num_param, MYSQL_BIND *bind, const char* szName);


    ///levelͳ�ƽ��
    bool db_mysql_init_statistictable();
    //�洢ͳ�Ʒ������
    bool db_save_statistic_stmt();

    ///����ͳ�ƽ��
    bool db_mysql_init_trackresult();

private:
    c2s_driveinfo_attempt_original m_original;      //�������stmt�������ݵĽṹ
    c2s_user_info m_user;
    c2s_driveinfo_level m_level;
    c2s_driveinfo_attempt m_attempt;
    c2s_4afc_result m_prefer;
    session_info m_session;

    CAnalyseTypeResult m_result;
    CTrackResult m_trackresult;

    MYSQL mysql;
    MYSQL_STMT *m_stInsertOrig;
    MYSQL_STMT *m_stSelectUID;
    MYSQL_STMT *m_stInsertUID;
    MYSQL_STMT *m_stSelectLID;
    MYSQL_STMT *m_stInsertLID;
    MYSQL_STMT *m_stInsertAID;
    MYSQL_STMT *m_stSelectAID;
    MYSQL_STMT *m_stSelectSID;
    MYSQL_STMT *m_stInsertSID;
    MYSQL_STMT *m_stInsertPrefer;

    MYSQL_RES *m_rsSelectUID;
    MYSQL_RES *m_rsSelectLID;
    MYSQL_RES *m_rsSelectAID;
    MYSQL_RES *m_rsSelectSID;

    MYSQL_STMT *m_stInsertLS;

    MYSQL_STMT *m_stInsertTS;

    std::string m_szServer;
    std::string m_szUser;
    std::string m_szPass;
    std::string m_szDB;

    unsigned char g_bConnected;
    long unsigned int TEX_LENGTH;
};

#endif // MYSQLCONNECT_H_INCLUDED
