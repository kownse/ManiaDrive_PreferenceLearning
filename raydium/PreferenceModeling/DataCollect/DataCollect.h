#ifndef _DATACOLLECT_H_
#define _DATACOLLECT_H_

#include "../raydium/raydium/common.h"
#include "../../raydium/PreferenceModeling/globle.h"

extern char server_ip[512];
extern u_short server_port;

extern char player_type[32];
extern char player_sex[16];
extern char player_age_buf[16];
extern char player_game_frequency[32];
extern int player_age;

//extern CSqlite3 sqlite;
typedef void (*void_f_void)(void);
typedef void (*void_f_2int)(int, int);
typedef void (*void_f_orig)(c2s_driveinfo_attempt_original *pTmpOrg);

extern void_f_void func_attempt_collect;
extern void_f_void func_level_collect;
extern void_f_void func_prefer_collect;
extern void_f_orig func_original_collect;
extern void_f_void func_userinfo_collect;
extern void_f_void func_level_result_collect;

extern int user_id;
extern int level_id;
extern int session_id;

extern int preference_index;
extern int preference_result;

extern int body_id;
extern int bForward;
extern int bBack;
extern int bTurnLeft;
extern int bTurnRight;
extern int bNoControl;
extern int bLeap;
extern int nColWhell;
extern int bCollision;

extern int attempttime;                    //��ǰ�ؿ��ĳ��Դ���
extern CRITICAL_SECTION cs;                 //�����ٽ���������

extern c2s_driveinfo_attempt attempinfo;
extern c2s_driveinfo_level levelinfo;
extern c2s_user_info userinfo;

void prelearn_collect_info(float step);     //�ռ�ͳ����Ϣ

///�����Ϣ���
//��ȡ�����Ϣ
void prelearn_read_user_info();
//��ȡ���洢�û���Ϣ����
void prelearn_handle_user_info();


///������ģʽ�µ��ռ�����
//����ÿ�γ��Ե���Ϣ
void prelearn_gameinfo_attempt_send();
//���ó�����Ϣ
void prelearn_gameinfo_attempt_reset();
//һ�γ������
void prelearn_gameinfo_attempt_finished();
//һ�γ��Խ�������û���
void prelearn_gameinfo_attempt_over(int total, int passed);
//�ܽ�ͳ����Ϣ
void prelearn_summary();
//���͹ؿ���Ϣ
void prelearn_gameinfo_level_send();
//���ùؿ���Ϣ
void prelearn_gameinfo_level_reset(char* mni);
//ֱ�ӷ���ԭʼ����
void prelearn_gameinfo_original_send(c2s_driveinfo_attempt_original *pTmpOrg);
//����ƫ�ý��
void prelearn_gameinfo_prefer_send(void);
//���������Ϣ
void prelearn_gameinfo_userinfo_send(void);
//���͹ؿ��ʾ���
void prelearn_gameinfo_level_result_send(void);


///����ģʽ�µ��ռ�����
//����ƫ�ý��
void prelearn_gameinfo_prefer_store(void);
//����level��Ϣ
void prelearn_gameinfo_level_store(void);
//����attempt��Ϣ
void prelearn_gameinfo_attempt_store(void);
//����ԭʼ����
void prelearn_gameinfo_original_store(c2s_driveinfo_attempt_original *pOrig);
//�ռ��û���Ϣ
void prelearn_gameinfo_userinfo_store();
//����
void prelearn_gameinfo_clean_local(void);
//�洢�ؿ��ʾ���
void prelearn_gameinfo_level_result_store(void);


///��װ����
void prelearn_set_rays(char *name);

///��¼��������
void prelearn_record_track_type(int type);
bool prelearn_check_track_type();


///��������
//����session����һش𵽵�emotion
int prelearn_get_session_emotion();

//��ƫ��ID�õ�ƫ���ַ���
//void getEmotionStr(char* szEmotion);

//���Իص�����
//void btnTest4AFC(raydium_gui_Object *w);
//��Ϸsession��������д����
//void prelearn_session_end(int type, char *buff);
#endif // DATACOLLECT_H_INCLUDED
