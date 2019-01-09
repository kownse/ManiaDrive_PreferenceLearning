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

///ÿ�γ��Ե�ԭʼ����
typedef struct c2s_driveinfo_attempt_original
{
    float pLinearSpeed[3];      //�ٶ�����
    float pAngSpeed[3];         //���ٶ�����
    float pRotate[3];           //��ת����
    float pDirection[3];        //��ת��������
    float pPosition[3];         //λ������

    float fTime;                //��ǰʱ��
    float fStep;                //ʱ����

    float fMotorSpeed;          //�����ٶ�

    float ray_f;                //���߽Ӵ�����
    float ray_b;
    float ray_l;
    float ray_r;
    float ray_fl;
    float ray_fr;
    float ray_up;
    float ray_down;

    int bForward;               //ǰ����־
    int bBack;                  //���˱�־
    int bTurnLeft;              //��ת��־
    int bTurnRight;             //��ת��־

    int bLeap;                  //���ձ�־
    int nColWhell;              //�Ӵ��������̥����
    int bCollision;             //������ײ��־

    int nLevel_id;              //�ؿ����
    int nAttempt_id;
    int nAttempIndex;           //���γ��Ե�Index
    int nOriginalIndex;         //ԭ�����ݵ�Index
    int track_type;             //��������
    int ori_id;
} _PACKED c2s_driveinfo_attempt_original;

///ͳ����Ϣ���ÿ�γ�������
typedef struct c2s_driveinfo_attempt
{
    time_t time_record;             //��¼ʱ��

    float fTotaltime;            //���γ��Ե�ʱ��
    float finishProportion;        //��ɱ���
    int attemp_index;
    int bFinished;                  //ͨ�ر�־
    int level_id;
    int session_id;
    int nOriginalTotal;         //ԭʼ���ݵ�����
    int attempt_id;
} _PACKED c2s_driveinfo_attempt;

typedef struct c2s_driveinfo_level
{
    char  trackname[NAME_LEN];           //����������
    float partytime;            //�ܳ��Ե�ʱ��
    float finishProportionaTotal;  //��ɱ���
    int num_attempt;                 //���Դ���
    int num_finished;                //ͨ�ش���

    int level_id;
    int session_id;
    time_t time_record;             //��¼ʱ��
    unsigned long trackname_length;

    int difficult;                  //������е����ѳ̶�
    int skill;                      //��Ҷ��Լ��ڸ������еļ�������
    int emotion;                       //����
    int fun;                        //���
    int emotion_degree;                  //��г̶�
} _PACKED c2s_driveinfo_level;

typedef struct c2s_4afc_result
{
    int pid;
    int sid;
    int prefer_type;
    int prefer_result;
} _PACKED c2s_4afc_result;

typedef struct c2s_user_info        //�����Ϣ
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
