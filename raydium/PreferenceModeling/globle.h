#ifndef GLOBLE_H_INCLUDED
#define GLOBLE_H_INCLUDED

#include <time.h>

//#include "../raydium/raydium/common.h"
#ifdef __GNUC__
#define _PACKED __attribute__((packed))
#else
#define _PACKED
#endif

#define C2S_GAME_MSG            (100)
#define C2S_GAMEINFO_ATTEMPT    (C2S_GAME_MSG+5)
#define C2S_GAMEINFO_LEVEL      (C2S_GAME_MSG+6)
#define C2S_NEW_COLLECTION      (C2S_GAME_MSG+7)
#define C2S_NEW_SESSION         (C2S_GAME_MSG+8)
#define C2S_PREFER_RESULT       (C2S_GAME_MSG+9)
#define C2S_GAMEINFO_ORIGINAL   (C2S_GAME_MSG+10)
#define C2S_USER_INFO           (C2S_GAME_MSG+11)

#define S2C_SEND_LEVELID        (C2S_GAME_MSG+20)
#define S2C_SEND_SESSIONID      (C2S_GAME_MSG+21)
#define S2C_SEND_SERVER_PORT    (C2S_GAME_MSG+22)
#define S2C_SEND_COLLECT_INFO   (C2S_GAME_MSG+23)
#define S2C_SESSION_END         (C2S_GAME_MSG+24)
#define S2C_READY_NEW_SESSION   (C2S_GAME_MSG+25)

#define PREFERENCE_ANXIETY          0
#define PREFERENCE_SATISFACTION     1
#define PREFERENCE_BORING           2
#define PREFERENCE_EXCITMENT        3
#define PREFERENCE_FRUSTRATION      4
#define PREFERENCE_CHANLENGE        5
#define PREFERENCE_FUN              6
#define PREFERENCE_MAX              6

#define PREFERENCE_RESULT_A     6
#define PREFERENCE_RESULT_B     7
#define PREFERENCE_RESULT_BOTH  8
#define PREFERENCE_RESULT_NONE  9

#define PLAYER_TYPE_LIST "  \323 \326\n  \324 \326\n  \325 \326"
//#define PLAYER_SEX_LIST "Male\nFemale"
#define PLAYER_SEX_LIST "  \321\n  \322"
#define PLAYER_FREQUENCY_LIST "<2 \334 \335/\336 \337\n2-8 \334 \335/\336 \337\n>8 \334 \335/\336 \337"
//#define LEVEL_DIFFICULT_LIST "Very Easy\nEasy\nNormal\nHard\nVery Hard"
#define LEVEL_DIFFICULT_LIST "    \262 \261\n    \261\n    \264\n    \263\n    \262 \263"
/*
#define LEVEL_SKILL_LIST "Very Low\nLow\nNormal\nHigh\nVery High"
#define LEVEL_FUN_LIST "No Fun\nLittle Fun\nNormal Fun\nMore Fun\nVery Fun"
#define DEGREE_LIST "None\nA Little\nSome\nEnough\nVery"
*/
#define LEVEL_SKILL_LIST "    \262 \261\n    \261\n    \264\n    \263\n    \262 \263"
#define LEVEL_FUN_LIST "    \262 \261\n    \261\n    \264\n    \263\n    \262 \263"
#define DEGREE_LIST "    \262 \261\n    \261\n    \264\n    \263\n    \262 \263"

#define LEVEL_FEEL_LIST "    \271 \272\n    \273 \274\n    \275 \276\n    \251 \252\n    \253 \254\n    \255 \256"

#define NAME_LEN 128

///每次尝试的原始数据
typedef struct c2s_driveinfo_attempt_original
{
    float pLinearSpeed[3];      //速度向量
    float pAngSpeed[3];         //角速度向量
    float pRotate[3];           //旋转向量
    float pDirection[3];        //旋转朝向向量
    float pPosition[3];         //位置向量

    float fTime;                //当前时间
    float fStep;                //时间间隔

    float fMotorSpeed;          //引擎速度

    float ray_f;                //射线接触长度
    float ray_b;
    float ray_l;
    float ray_r;
    float ray_fl;
    float ray_fr;
    float ray_up;
    float ray_down;

    int bForward;               //前进标志
    int bBack;                  //后退标志
    int bTurnLeft;              //左转标志
    int bTurnRight;             //右转标志

    int bLeap;                  //悬空标志
    int nColWhell;              //接触地面的轮胎数量
    int bCollision;             //车身碰撞标志

    int nLevel_id;              //关卡标记
    int nAttempt_id;
    int nAttempIndex;           //单次尝试的Index
    int nOriginalIndex;         //原生数据的Index
    int track_type;             //赛道类型
    int ori_id;
} _PACKED c2s_driveinfo_attempt_original;

///统计信息后的每次尝试数据
typedef struct c2s_driveinfo_attempt
{
    time_t time_record;             //记录时间

    float fTotaltime;            //单次尝试的时间
    float finishProportion;        //完成比例
    int attemp_index;
    int bFinished;                  //通关标志
    int level_id;
    int session_id;
    int nOriginalTotal;         //原始数据的总数
    int attempt_id;
} _PACKED c2s_driveinfo_attempt;

typedef struct c2s_driveinfo_level
{
    char  trackname[NAME_LEN];           //赛道的名字
    float partytime;            //总尝试的时间
    float finishProportionaTotal;  //完成比例
    int num_attempt;                 //尝试次数
    int num_finished;                //通关次数

    int level_id;
    int session_id;
    time_t time_record;             //记录时间
    unsigned long trackname_length;

    int difficult;                  //玩家评判的困难程度
    int skill;                      //玩家对自己在该赛道中的技术评价
    int emotion;                       //感受
    int fun;                        //情感
    int emotion_degree;                  //情感程度
} _PACKED c2s_driveinfo_level;

typedef struct c2s_4afc_result
{
    int pid;
    int sid;
    int prefer_type;
    int prefer_result;
} _PACKED c2s_4afc_result;

typedef struct c2s_user_info        //玩家信息
{
    char name[NAME_LEN];
    int sex;
    int age;
    int frequency;
    int type;
    int user_id;
    unsigned long name_length;
} _PACKED c2s_user_info;


typedef struct session_info
{
    char time[NAME_LEN];
    int user_id;
    int level_id1;
    int level_id2;
    int session_id;
    unsigned long time_length;
} _PACKED session_info;

#endif // GLOBLE_H_INCLUDED
