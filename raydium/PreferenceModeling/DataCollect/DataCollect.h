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

extern int attempttime;                    //当前关卡的尝试次数
extern CRITICAL_SECTION cs;                 //传输临界区控制量

extern c2s_driveinfo_attempt attempinfo;
extern c2s_driveinfo_level levelinfo;
extern c2s_user_info userinfo;

void prelearn_collect_info(float step);     //收集统计信息

///玩家信息相关
//读取玩家信息
void prelearn_read_user_info();
//读取并存储用户信息界面
void prelearn_handle_user_info();


///局域网模式下的收集处理
//发送每次尝试的信息
void prelearn_gameinfo_attempt_send();
//重置尝试信息
void prelearn_gameinfo_attempt_reset();
//一次尝试完成
void prelearn_gameinfo_attempt_finished();
//一次尝试结束，但没完成
void prelearn_gameinfo_attempt_over(int total, int passed);
//总结统计信息
void prelearn_summary();
//发送关卡信息
void prelearn_gameinfo_level_send();
//重置关卡信息
void prelearn_gameinfo_level_reset(char* mni);
//直接发送原始数据
void prelearn_gameinfo_original_send(c2s_driveinfo_attempt_original *pTmpOrg);
//发送偏好结果
void prelearn_gameinfo_prefer_send(void);
//发送玩家信息
void prelearn_gameinfo_userinfo_send(void);
//发送关卡问卷结果
void prelearn_gameinfo_level_result_send(void);


///单机模式下的收集处理
//保存偏好结果
void prelearn_gameinfo_prefer_store(void);
//保存level信息
void prelearn_gameinfo_level_store(void);
//保存attempt信息
void prelearn_gameinfo_attempt_store(void);
//保存原始数据
void prelearn_gameinfo_original_store(c2s_driveinfo_attempt_original *pOrig);
//收集用户信息
void prelearn_gameinfo_userinfo_store();
//清理
void prelearn_gameinfo_clean_local(void);
//存储关卡问卷结果
void prelearn_gameinfo_level_result_store(void);


///安装射线
void prelearn_set_rays(char *name);

///记录赛道类型
void prelearn_record_track_type(int type);
bool prelearn_check_track_type();


///辅助函数
//返回session中玩家回答到的emotion
int prelearn_get_session_emotion();

//由偏好ID得到偏好字符串
//void getEmotionStr(char* szEmotion);

//测试回调函数
//void btnTest4AFC(raydium_gui_Object *w);
//游戏session结束，填写报告
//void prelearn_session_end(int type, char *buff);
#endif // DATACOLLECT_H_INCLUDED
