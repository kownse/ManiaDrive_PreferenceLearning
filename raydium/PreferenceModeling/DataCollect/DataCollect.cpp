#include "raydium/index.h"
#include "libintl.h"
#include "mania_drive.h"
#include "PreferenceModeling/DataCollect/DataCollect.h"
#include "PreferenceModeling/DataCollect/DataAnalyse.h"
#include "PreferenceModeling/DataCollect/MathHelper.h"
#include "PreferenceModeling/DataCollect/CollectSingle.h"
#include "PreferenceModeling/utility.h"
//#include "../raydium/PreferenceModeling/DataBase/pre_sqlite.h"
#include "pre_db/pre_db.h"
#include "pre_analyse/pre_analyse.h"
#include "../raydium/PreferenceModeling/DataCollect/CollectGUI.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <windows.h>
#include <WinNT.h>
#include <WinDef.h>

char server_ip[512];
u_short server_port;
char player_age_buf[16];
char player_type[32];
char player_sex[16];
char player_game_frequency[32];
int player_age;

CSqlite3Saver sqlite;        //SQLITE3���ݿ�

///����ָ��ӿ�
//typedef void (*void_func)(void);
void_f_void func_attempt_collect = prelearn_gameinfo_attempt_send;
void_f_void func_level_collect = prelearn_gameinfo_level_send;
void_f_void func_prefer_collect = prelearn_gameinfo_prefer_send;
void_f_orig func_original_collect = prelearn_gameinfo_original_send;
void_f_void func_userinfo_collect = prelearn_gameinfo_userinfo_send;
void_f_void func_level_result_collect = prelearn_gameinfo_level_result_send;

///������vector����˫���巢�͡���
std::vector<c2s_driveinfo_attempt_original> vecOriInfo1;   //������vector������
std::vector<c2s_driveinfo_attempt_original> vecOriInfo2;    //
std::vector<c2s_driveinfo_attempt_original> *pCurVecOriInfo = &vecOriInfo1; //��ǰʹ�õ�


CRITICAL_SECTION cs;                                        //�ٽ�����Դ
float fStepPile = 0.0f;
const float ORIGE_FREQUENCY = 20;
const float ORIGE_STEP = 1.0f / ORIGE_FREQUENCY;            //֡���ۻ�ʱ��

int nAttemptIndex = 0;
c2s_driveinfo_attempt attempinfo;                           //���γ��Ե�����
c2s_driveinfo_level levelinfo;                              //���ιؿ�������
c2s_driveinfo_attempt_original originalInfo;                //ÿ֡��ԭʼ����
c2s_user_info userinfo;
//int nOriginalIndex = 0;                                     //ԭʼ���ݼ���

std::vector<c2s_driveinfo_attempt> vecAttemptStore;
std::vector<c2s_driveinfo_level> vecLevelStore;
std::vector<c2s_driveinfo_attempt_original> vecOriginalStore;

int user_id = -1;
int level_id = -1;
int session_id = -1;

int preference_index;
int preference_result;

//�������
int body_id;
int ray_f_id = -1;
int ray_b_id = -1;
int ray_l_id = -1;
int ray_r_id = -1;
int ray_fr_id = -1;
int ray_fl_id = -1;
int ray_up_id = -1;
int ray_down_id = -1;
const int RAY_LENGTH = 5;

int bForward;
int bBack;
int bTurnLeft;
int bTurnRight;
int bNoControl;

///�������
std::vector<float> vecLeap;
int bLeap = 0;
int nColWhell = 0;
const float tLeapThread = 0.2f;            //��������Ҫ������ô�òſ�ʼ��ʱ
float tLeapPile = 0.0f;

///��ײ����
std::vector<float> vecColLeap;
int bColLeap = 0;
const float tColLeapThredhold = 0.2f;            //��������Ҫ������ô�òſ�ʼ��ʱ
float tColLeapPile = 0.0f;

///��ײ���
std::vector<float> vecSlightCollision;
std::vector<float> vecContinueCollision;
int bCollision;
const float tColIntervalThread = 0.5f;        //��ײ�����
const float tColContinueThread = 0.5f;            //������ײ��ֵ
float tColPile = 0.0f;
float tColStart = 0.0f;             //��ʼ������ײ��ʱ��
float tColEnd = 0.0f;               //��ײ������ʱ��
unsigned char bColFlag = 0;
int nColCount = 0;                  //��ײ��������

COriAnalyserMgr analyser;

///������ٶ�
std::vector<float> vecBigAglSpeed;        //������ٶ�
const float tBAIntervalThredHold = 0.5f;
float tBigAglPile = 0.0f;
float tBigAglStart = 0.0f;
float tBigAglEnd = 0.0f;
unsigned char bBigAglFlag = 0;

///�߸�
std::vector<float> vecOverTurn;
const float tOTIntervalThredhold = 0.5f;       //�߸������ֵ
float tOTStart = 0.0f;
float tOTEnd = 0.0f;
float tOTPile = 0.0f;
unsigned char bOTFlag = 0;

///�ٶ�
std::vector<float> vecMotorSpeed;    //���γ����е��ٶ�ͳ��
std::vector<float> vecDifNegSpeed;   //ʵ���ٶ��������ٶȵĸ������
std::vector<float> vecDifPosSpeed;   //ʵ���ٶ��������ٶȵ��������
std::vector<float> vecLinearRapid;    //�Լ�����ʻ��ʱ��
const float tRapidThredhold = 0.3f;
float tRapidPile = 0.0f;

int attempttime;                    //��ǰ�ؿ��ĳ��Դ���
float last_step = 0.0f;



///�ռ�����ʱ��
void col_leap_time(float tPile)
{
    /*    attempinfo.leaptime += tPile;
        if(bNoControl)
        {
            attempinfo.leapnocontroltime += tPile;
        }
        vecLeap.push_back(tPile);
    */
    raydium_log("���� %f", tPile);
}

///�ռ���ײ����
void col_col_leap_time(float tPile)
{
    raydium_log("��ײ����: %f", tPile);
//    vecColLeap.push_back(tPile);
}


///�ռ���ײʱ��
void col_collision_time(float tTotal)
{
    if(tTotal > tColContinueThread)
    {
        raydium_log("������ײ:\t%f", tTotal);
//        vecContinueCollision.push_back(tTotal);
    }
    else
    {
        raydium_log("żȻ��ײ:\t%f", tTotal);
//        vecSlightCollision.push_back(tTotal);
    }
    /*
        attempinfo.collisiontime += tTotal;
        if(bNoControl)
            attempinfo.collisionnocontroltime += tTotal;
    */
}


//�ռ�����ʱ��
void col_rapid_time(float tPile)
{
    raydium_log("���� %f", tPile);
//    vecLinearRapid.push_back(tPile);
}

void col_bigangular_time(float tPile)
{
    raydium_log("�쳣ת��:\t %f ", tPile);
}

void col_ot_time(float tPile)
{
    raydium_log("����:\t %f", tPile);
}

void prelearn_collect_info(float step)
{
    attempinfo.fTotaltime += step;

    if(attempinfo.fTotaltime < 1.0f)
        return;

    //ֻ����һ��֡����¼����
    fStepPile += step;
    if(fStepPile < ORIGE_STEP)
    {
        return;
    }

    /*
        if(step != last_step)
        {
            last_step = step;
        }
    */
    ///����״̬ͳ����(���ں���ļ�������õ�״̬������жϣ����Է�������ط�ͳһ���)

    ///�����ٶ�
    float fMotorSpeed = raydium_ode_motor_speed_get_name("moteur", 1);

    ///�����ٶ�
    dReal *pSpeed;
    pSpeed = raydium_ode_element_linearvelocity_get_name("corps");
    //raydium_log("linearvelocity: %f %f %f",pSpeed[0],pSpeed[1],pSpeed[2]);
    float fSpeed = sqrt(pSpeed[0]*pSpeed[0] + pSpeed[1]*pSpeed[1] + pSpeed[2]*pSpeed[2]);
    float fAdjustSpeed = fSpeed * 10;       //�������ٶȺ�motor�ٶ���һ����������
    int bRapid = (fAdjustSpeed > 33.99f) ? 1 : 0;

    ///���ٶ�
    dReal *pAngSpeed;
    pAngSpeed = raydium_ode_element_angularvelocity_get_name("corps");
    float fAngularSpeed = sqrt(pAngSpeed[0]*pAngSpeed[0] + pAngSpeed[1]*pAngSpeed[1] + pAngSpeed[2]*pAngSpeed[2]);
    int bBigAgl = (fAngularSpeed > 1.5f) ? 1 : 0;

    ///��ת�Ƕ�
    dReal pRot[3];
    raydium_ode_element_rot_get_name ("corps", &pRot[0], &pRot[1], &pRot[2]);
    int bOverTurn = (abs(pRot[0]) > 0.3f || abs(pRot[1]) > 0.3f)? 1 : 0;

    ///����
    dReal pDir[3];
    raydium_ode_element_rot_forward_get_name("corps", &pDir[0], &pDir[1], &pDir[2]);

    ///λ��
    dReal pPosition[3];
    dReal *pPos;
    pPos=raydium_ode_element_pos_get_name("corps");
    memcpy(pPosition,pPos,sizeof(dReal)*3);

    //������Ϣ
    raydium_ode_Ray rayInfo_f;
    raydium_ode_element_ray_get_name("corps", ray_f_id, &rayInfo_f);
    if(rayInfo_f.min_elem == -1)
        rayInfo_f.min_dist = RAY_LENGTH;

    raydium_ode_Ray rayInfo_b;
    raydium_ode_element_ray_get_name("corps", ray_b_id, &rayInfo_b);
    if(rayInfo_b.min_elem == -1)
        rayInfo_b.min_dist = RAY_LENGTH;

    raydium_ode_Ray rayInfo_l;
    raydium_ode_element_ray_get_name("corps", ray_l_id, &rayInfo_l);
    if(rayInfo_l.min_elem == -1)
        rayInfo_l.min_dist = RAY_LENGTH;

    raydium_ode_Ray rayInfo_r;
    raydium_ode_element_ray_get_name("corps", ray_r_id, &rayInfo_r);
    if(rayInfo_r.min_elem == -1)
        rayInfo_r.min_dist = RAY_LENGTH;

    raydium_ode_Ray rayInfo_fl;
    raydium_ode_element_ray_get_name("corps", ray_fl_id, &rayInfo_fl);
    if(rayInfo_fl.min_elem == -1)
        rayInfo_fl.min_dist = RAY_LENGTH;

    raydium_ode_Ray rayInfo_fr;
    raydium_ode_element_ray_get_name("corps", ray_fr_id, &rayInfo_fr);
    if(rayInfo_fr.min_elem == -1)
        rayInfo_fr.min_dist = RAY_LENGTH;

    raydium_ode_Ray rayInfo_up;
    raydium_ode_element_ray_get_name("corps", ray_up_id, &rayInfo_up);
    if(rayInfo_up.min_elem == -1)
        rayInfo_up.min_dist = RAY_LENGTH;

    raydium_ode_Ray rayInfo_down;
    raydium_ode_element_ray_get_name("corps", ray_down_id, &rayInfo_down);
    if(rayInfo_down.min_elem == -1)
        rayInfo_down.min_dist = RAY_LENGTH;
    /*
        raydium_log("ǰ:%f, ��%f, ��%f, ��%f, ��ǰ:%f, ��ǰ:%f, ��:%f, ��:%f",
                    rayInfo_f.min_dist,
                    rayInfo_b.min_dist,
                    rayInfo_l.min_dist,
                    rayInfo_r.min_dist,
                    rayInfo_fl.min_dist,
                    rayInfo_fr.min_dist,
                    rayInfo_up.min_dist,
                    rayInfo_down.min_dist);
    */
    //ÿ֡�ļ�¼����
//    c2s_driveinfo_attempt_original tmpOriginal;

    memcpy(&(originalInfo.pLinearSpeed[0]), pSpeed, sizeof(dReal)*3);
    memcpy(&(originalInfo.pAngSpeed[0]), pAngSpeed, sizeof(dReal)*3);
    memcpy(&(originalInfo.pRotate[0]), &pRot, sizeof(dReal)*3);
    memcpy(&(originalInfo.pDirection[0]), &pDir, sizeof(dReal)*3);
    memcpy(&originalInfo.pPosition, &pPosition, sizeof(dReal)*3);
    originalInfo.fTime = attempinfo.fTotaltime;
    originalInfo.fStep = fStepPile;
    originalInfo.fMotorSpeed = fMotorSpeed;
    originalInfo.bForward = bForward;
    originalInfo.bBack = bBack;
    originalInfo.bTurnLeft = bTurnLeft;
    originalInfo.bTurnRight = bTurnRight;
    originalInfo.bLeap = bLeap;
    originalInfo.nColWhell = (nColWhell >= 4) ? ((nColWhell%4 == 0) ? 4 : nColWhell % 4): nColWhell ; //�ж������������պ�մ��
    originalInfo.bCollision = bCollision;
    originalInfo.nAttempIndex = levelinfo.num_attempt;
    originalInfo.nLevel_id = levelinfo.level_id;
    originalInfo.ray_f = rayInfo_f.min_dist;
    originalInfo.ray_b = rayInfo_b.min_dist;
    originalInfo.ray_l = rayInfo_l.min_dist;
    originalInfo.ray_r = rayInfo_r.min_dist;
    originalInfo.ray_fl = rayInfo_fl.min_dist;
    originalInfo.ray_fr = rayInfo_fr.min_dist;
    originalInfo.ray_up = rayInfo_up.min_dist;
    originalInfo.ray_down = rayInfo_down.min_dist;
    originalInfo.nOriginalIndex = attempinfo.nOriginalTotal;

    //if(tmpOriginal.nColWhell != 4)
    //    raydium_log("��̥�Ӵ���������:%d, ԭʼ����:%d", tmpOriginal.nColWhell, nColWhell);
    //raydium_log("��¼�ٶ�: %f %f %f", pSpeed[0], pSpeed[1], pSpeed[2]);

    //pCurVecOriInfo->push_back(tmpOriginal);
    ++attempinfo.nOriginalTotal;

//    if(raydium_network_mode==RAYDIUM_NETWORK_MODE_CLIENT)
//        prelearn_gameinfo_original_send(&tmpOriginal);
    assert(func_original_collect && "ԭʼ���ݴ�����Ϊ��");
    func_original_collect(&originalInfo);

    analyser.DoAnalyse(originalInfo);
///��������ͳ��
    /*    {

            if(bForward)
            {
                attempinfo.forwardtime += fStepPile;
                if(bTurnLeft)
                {
                    attempinfo.forwardlefttime += fStepPile;
                }
                if(bTurnRight)
                {
                    attempinfo.forwardrighttime += fStepPile;
                }
            }

            if(bBack)
            {
                attempinfo.backtime += fStepPile;
                if(bTurnLeft)
                {
                    attempinfo.backlefttime += fStepPile;
                }
                if(bTurnRight)
                {
                    attempinfo.backrighttime += fStepPile;
                }
            }

            if(bTurnLeft)
            {
                attempinfo.turnlefttime += fStepPile;
            }

            if(bTurnRight)
            {
                attempinfo.turnrighttime += fStepPile;
            }

        }

        if(fAdjustSpeed > 1.0f)
        {
            float fDirDif = inner_product_3f(pSpeed, pDir);
            //raydium_log("�ǶȲ�:\t %f", fDirDif);
        }

        float fDirDif = inner_product_3f(pDir, pSpeed);


    ///��ײʱ��ͳ��
    {
        col_tw_info(bCollision,
                    bColFlag,
                    tColStart,
                    tColEnd,
                    tColPile,
                    attempinfo.fTotaltime,
                    fStepPile,
                    tColIntervalThread,
                    1,
                    &col_collision_time
                   );
    }

    ///�ٶ�
    {
        col_con_info(bRapid,
                     tRapidPile,
                     fStepPile,
                     tRapidThredhold,
                     0,
                     col_rapid_time
                    );
    }

    ///�����ٶ��������ٶȵĲ�ֵ
    {
        if(!bLeap)
        {
//           vecMotorSpeed.push_back(fMotorSpeed);

            ///ʵ���ٶ��������ٶȵĲ�ֵ��ֻ�ڼ���ʱ���㣩
            if(bForward)
            {
                //vecLinearSpeed.push_back(fAdjustSpeed);
                //raydium_log("velocity: %f", fAdjustSpeed);
                float fDifference = fAdjustSpeed - fMotorSpeed;
                if(fDifference > 3.0f)
                {
                    //              raydium_log("�����ٲ�: %f", fDifference);
                    //                  vecDifPosSpeed.push_back(fDifference);
                }
                else if(fDifference < -5.0f)
                {
                    //              raydium_log("�����ٲ�: %f", fDifference);
//                   vecDifNegSpeed.push_back(fDifference);
                }
            }
        }
    }

    ///����ʱ��ͳ��
    {
        if(!bColFlag)
        {
            //���Ӧ���Ƿ�Ծ
            col_con_info(bLeap,
                         tLeapPile,
                         fStepPile,
                         tLeapThread,
                         1,
                         &col_leap_time);
        }
        else
        {
            //���Ӧ������ײ����
            col_con_info(bLeap,
                         tColLeapPile,
                         fStepPile,
                         tColLeapThredhold,
                         1,
                         &col_col_leap_time);
        }
    }

    ///����ת��
    {
        col_tw_info(bBigAgl,
                    bBigAglFlag,
                    tBigAglStart,
                    tBigAglEnd,
                    tBigAglPile,
                    attempinfo.fTotaltime,
                    fStepPile,
                    tBAIntervalThredHold,
                    0,
                    &col_bigangular_time
                   );
    }

    ///�߸�
    {
        col_tw_info(bOverTurn,
                    bOTFlag,
                    tOTStart,
                    tOTEnd,
                    tOTPile,
                    attempinfo.fTotaltime,
                    fStepPile,
                    tOTIntervalThredhold,
                    0,
                    &col_ot_time
                   );
    }
*/
    fStepPile = 0;


//    nColWhell = 0;
    /*        dReal quaternion[4];
            raydium_ode_element_rotq_get_name ("corps", quaternion);
            raydium_log("rotate: %f %f %f %f", quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
    */
    bLeap = 1;
    bCollision = 0;
}

//�ܽ�ͳ����Ϣ
void prelearn_summary()
{
    /*    float maxSpeed = *(std::max_element(vecMotorSpeed.begin(), vecMotorSpeed.end()));

        float totSpeed = 0.0f;
        std::vector<float>::iterator it;
        for(it = vecMotorSpeed.begin(); it != vecMotorSpeed.end(); ++it)
        {
            totSpeed += *it;
        }

        float avgSpeed = totSpeed / vecMotorSpeed.size();
        float variance = 0.0f;
        for(it = vecMotorSpeed.begin(); it != vecMotorSpeed.end(); ++it)
        {
            variance += (*it - avgSpeed) * (*it - avgSpeed);
        }
        float stdVariance = sqrt(variance);

        attempinfo.maxSpeed = maxSpeed;
        attempinfo.avgSpeed = avgSpeed;
        attempinfo.stdVarSpeed = stdVariance;
    */
}

void prelearn_gameinfo_original_send(c2s_driveinfo_attempt_original *pTmpOrg)
{
    static int nOrigSend = 0;
    char data[RAYDIUM_NETWORK_PACKET_SIZE];
    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, pTmpOrg, sizeof(c2s_driveinfo_attempt_original));
    raydium_network_write(NULL,raydium_network_uid,C2S_GAMEINFO_ORIGINAL,data);
    int size = sizeof(c2s_driveinfo_attempt_original);
    //raydium_log("�����ٶ�: %f, %f, %f", pTmpOri->pLinearSpeed[0], pTmpOri->pLinearSpeed[1], pTmpOri->pLinearSpeed[2]);
    //raydium_log("����ԭʼ����: %d", nOrigSend++);
}

///���͵��γ��Ե�ԭʼ����
DWORD WINAPI prelearn_gameinfo_original_send_total(void* arg)
{
    if(raydium_network_mode!=RAYDIUM_NETWORK_MODE_CLIENT)
        return(-1);

    EnterCriticalSection(&cs);  //�����ٽ���

    unsigned long tStart = raydium_timecall_clock();
    std::vector<c2s_driveinfo_attempt_original> *pVecOriInfo = (std::vector<c2s_driveinfo_attempt_original> *)arg;
    ///��Ȼ���������﷢������֡�����ݣ�
    ///����Raydium������ģ����л�������С���ơ�
    ///�ڷ�������ֻ�ܽ���һ����֡������
    std::vector<c2s_driveinfo_attempt_original>::iterator it;
    for(it = pVecOriInfo->begin(); it != pVecOriInfo->end(); ++it)
    {
        c2s_driveinfo_attempt_original *pTmpOri = &(*it);
//        char data[RAYDIUM_NETWORK_PACKET_SIZE];
//        memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, pTmpOri, sizeof(c2s_driveinfo_attempt_original));
//        raydium_network_write(NULL,raydium_network_uid,C2S_GAMEINFO_ORIGINAL,data);
        prelearn_gameinfo_original_send(pTmpOri);
        Sleep(1);           ///�������Sleep���ڱ����ϲ���ʱ�ǶԵ�
    }
    unsigned long tEnd = raydium_timecall_clock();
    raydium_log("��������: %d, ��ʱ: %ld", pVecOriInfo->size(), tEnd - tStart);
    pVecOriInfo->clear();

    LeaveCriticalSection(&cs); //�˳��ٽ���

    return(0);
}

///��һ���̰߳�װ����ԭʼ���ݵ�����
void prelearn_thread_original_send()
{
    DWORD dThreadID;
    //                 ::CreateThread(NULL, 0, &CIAAnalyser::yyTranslateMessage, m_pAnalyser, 0, &m_dAnalyseThreadID);
    HANDLE hThread = ::CreateThread(NULL, 0, &prelearn_gameinfo_original_send_total, (void*)pCurVecOriInfo, 0, &dThreadID);
    if(hThread == NULL)
    {
        raydium_log("���������߳�ʧ��");
    }
    else
    {
        raydium_log("�����̴߳����ɹ�");
        //������ǰʹ�õļ�¼vector
        pCurVecOriInfo = (pCurVecOriInfo == &vecOriInfo1) ? &vecOriInfo2 : &vecOriInfo1;
    }
}

void prelearn_gameinfo_attempt_send()
{
    if(raydium_network_mode!=RAYDIUM_NETWORK_MODE_CLIENT)
        return;

//    prelearn_gameinfo_original_send();

    attempinfo.time_record = time(0);
    char data[RAYDIUM_NETWORK_PACKET_SIZE];

    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &attempinfo, sizeof(attempinfo));
    raydium_network_write(NULL,raydium_network_uid,C2S_GAMEINFO_ATTEMPT,data);

    pCurVecOriInfo->clear();

    raydium_log("����attempt��Ϣ");
//    thread_original_send();
}

void prelearn_gameinfo_attempt_reset()
{
    memset(&attempinfo, 0, sizeof(attempinfo));
    vecMotorSpeed.clear();

    //clAnlser.Reset();
    analyser.GetStaticResult();
    analyser.Reset();
}

void prelearn_gameinfo_attempt_finished(void)
{
    //����ͳ����Ϣ
    prelearn_summary();
    //attempinfo.bFinished = 1;
    attempinfo.finishProportion = 1.0f;
    attempinfo.session_id = session_id;
    attempinfo.level_id = level_id;
    attempinfo.attemp_index = levelinfo.num_attempt;

    levelinfo.num_finished += 1;
    levelinfo.num_attempt += 1;
    levelinfo.finishProportionaTotal += 1.0f;

//    if(raydium_network_mode==RAYDIUM_NETWORK_MODE_CLIENT)
//        prelearn_gameinfo_attempt_send();
    assert(func_attempt_collect && "�������ݴ�����Ϊ��");
    func_attempt_collect();

    prelearn_gameinfo_attempt_reset();

}

void prelearn_gameinfo_attempt_over(int total, int passed)
{
    prelearn_summary();
    attempinfo.finishProportion = (float)passed / total;
    attempinfo.bFinished = 0;
    attempinfo.session_id = session_id;
    attempinfo.level_id = level_id;
    attempinfo.attemp_index = levelinfo.num_attempt;

    levelinfo.num_finished += 0;
    levelinfo.num_attempt += 1;
    levelinfo.finishProportionaTotal += attempinfo.finishProportion;

//    if(raydium_network_mode==RAYDIUM_NETWORK_MODE_CLIENT)
//        prelearn_gameinfo_attempt_send();
    assert(func_attempt_collect && "�������ݴ�����Ϊ��");
    func_attempt_collect();
    prelearn_gameinfo_attempt_reset();
}

//���͹ؿ���Ϣ
void prelearn_gameinfo_level_send()
{
    if(raydium_network_mode!=RAYDIUM_NETWORK_MODE_CLIENT)
        return;

    levelinfo.time_record = time(0);
    char data[RAYDIUM_NETWORK_PACKET_SIZE];
    c2s_driveinfo_level set;

    int dec;
    int i;

    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &levelinfo, sizeof(levelinfo));

    raydium_network_write(NULL,raydium_network_uid,C2S_GAMEINFO_LEVEL,data);

    raydium_log("����level��Ϣ");
    //prelearn_gameinfo_original_send();
//    thread_original_send();
}

//���ùؿ���Ϣ
void prelearn_gameinfo_level_reset(char* mni)
{
    memset(&levelinfo, 0, sizeof(levelinfo));
    strcpy(levelinfo.trackname, mni);
    levelinfo.difficult = -1;
    levelinfo.skill = -1;
    levelinfo.emotion = -1;
    levelinfo.fun = -1;
    levelinfo.emotion_degree = -1;
    bPassed = false;
}

//������session�ź�
void prelearn_send_new_session()
{
    char data[RAYDIUM_NETWORK_PACKET_SIZE];
    raydium_network_write(NULL,raydium_network_uid,C2S_NEW_SESSION,data);

    //�رս���
    raydium_gui_window_delete_name("menu");
}

void prelearn_gameinfo_prefer_send(void)
{
    char data[RAYDIUM_NETWORK_PACKET_SIZE];
    c2s_4afc_result set;

    set.prefer_type = preference_index;
    set.prefer_result = preference_result;
    set.sid = session_id;

    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &set, sizeof(set));

    raydium_network_write(NULL,raydium_network_uid,C2S_PREFER_RESULT,data);
    raydium_log("����prefer_result");

    raydium_gui_window_delete_name("menu");
    int handle=raydium_gui_window_create("menu",20,45,55,15);
    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblInfo",handle,65,50,gettext("4-AFC Result Sended\n\n     Please Wait..."),0,0,0);
}

//���������Ϣ
void prelearn_gameinfo_userinfo_send(void)
{
    //c2s_user_info info;
/*
    strcpy(userinfo.name, raydium_network_name_local);
    userinfo.sex = raydium_gui_list_id(player_sex,PLAYER_SEX_LIST);
    userinfo.age = player_age;
    userinfo.frequency = raydium_gui_list_id(player_game_frequency,PLAYER_FREQUENCY_LIST);
    userinfo.type = raydium_gui_list_id(player_type,PLAYER_TYPE_LIST);
*/
    char data[RAYDIUM_NETWORK_PACKET_SIZE];
    memset(data, 0, sizeof(data));
    memcpy(data+RAYDIUM_NETWORK_PACKET_OFFSET, &userinfo, sizeof(userinfo));
    raydium_network_write(NULL, raydium_network_uid, C2S_USER_INFO, data);
    raydium_log("�����û���Ϣ");
}

void prelearn_gameinfo_level_result_send(void)
{

}

///��ƫ�ý���洢���������ݿ�
void prelearn_gameinfo_prefer_store(void)
{
    //�õ�ƫ��
    c2s_4afc_result set;
    set.prefer_type = preference_index;
    set.prefer_result = preference_result;
    set.sid = session_id;

    //�������ݿ�
    sqlite.save_session_data(vecAttemptStore, vecLevelStore, vecOriginalStore, set, userinfo);
    //������session
    prelearn_build_gui_New_session();
}

//����level��Ϣ
void prelearn_gameinfo_level_store(void)
{
    levelinfo.time_record = time(0);
    vecLevelStore.push_back(levelinfo);
}

void prelearn_gameinfo_attempt_store(void)
{
    attempinfo.time_record = time(0);
    vecAttemptStore.push_back(attempinfo);
}

//����ԭʼ����
void prelearn_gameinfo_original_store(c2s_driveinfo_attempt_original *pOrig)
{
    vecOriginalStore.push_back(*pOrig);
}

//�ռ��û���Ϣ
void prelearn_gameinfo_userinfo_store()
{
/*    strcpy(userinfo.name, raydium_network_name_local);
    userinfo.sex = raydium_gui_list_id(player_sex,PLAYER_SEX_LIST);
    userinfo.age = player_age;
    userinfo.frequency = raydium_gui_list_id(player_game_frequency,PLAYER_FREQUENCY_LIST);
    userinfo.type = raydium_gui_list_id(player_type,PLAYER_TYPE_LIST);
*/
}

//�������е�����vector
void prelearn_gameinfo_clean_local(void)
{
    vecOriginalStore.clear();
    vecAttemptStore.clear();
    vecLevelStore.clear();
}

void prelearn_gameinfo_level_result_store(void)
{
    /*
    char level_difficult[32];
    char level_skill[32];
    char level_feel[32];
    char level_fun[32];
    char level_fun_degree[32];

    memset(level_difficult, 0, sizeof(level_difficult));
    memset(level_skill, 0, sizeof(level_skill));
    memset(level_feel, 0, sizeof(level_feel));
    memset(level_fun, 0, sizeof(level_fun));
    memset(level_fun_degree, 0, sizeof(level_fun_degree));
    */

    levelinfo.difficult = raydium_gui_read_name("menu", "cboLevelDifficult", 0);
    levelinfo.skill = raydium_gui_read_name("menu", "cboLevelSkill", 0);
    levelinfo.emotion = raydium_gui_read_name("menu", "cboLevelFeel", 0);
    levelinfo.fun = raydium_gui_read_name("menu", "cboLevelFun", 0);
    levelinfo.emotion_degree = raydium_gui_read_name("menu", "cboEmotionDegree", 0);

    /*    raydium_gui_read_name("menu", "cboLevelDifficult", level_difficult);
        raydium_gui_read_name("menu", "cboLevelSkill", level_skill);
        raydium_gui_read_name("menu", "cboLevelFeel", level_feel);
        raydium_gui_read_name("menu", "cboLevelFun", level_fun);
        raydium_gui_read_name("menu", "cboEmotionDegree", level_fun_degree);

        levelinfo.difficult = raydium_gui_list_id(level_difficult,LEVEL_DIFFICULT_LIST);
        levelinfo.skill = raydium_gui_list_id(level_skill, LEVEL_SKILL_LIST);
        levelinfo.emotion = raydium_gui_list_id(level_feel, LEVEL_FEEL_LIST);
        levelinfo.fun = raydium_gui_list_id(level_fun, LEVEL_FUN_LIST);
        levelinfo.emotion_degree = raydium_gui_list_id(level_fun_degree, DEGREE_LIST);
    */
    if(levelinfo.difficult == -1 ||
            levelinfo.skill == -1 ||
            levelinfo.emotion == -1 ||
            levelinfo.fun == -1 ||
            levelinfo.emotion_degree == -1)
    {
        //prelearn_build_gui_level_question();
    }
    else
    {
        //ͳ�ƹؿ���Ϣ
        assert(func_level_collect && "�ؿ����ݴ�����Ϊ��");
        func_level_collect();

        if(level_id >=1 )
        {
            level_id = -1;
            //btnNewGameSession(0);
            prelearn_build_gui_4AFC();
        }
        else
        {
            switch_local_track();
        }
    }
}

//��װ����
void prelearn_set_rays(char *name)
{
    //��������������
    if((ray_f_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, 1, 0, 0)) < 0)
        raydium_log("�����ǰ����ʧ��");
    if((ray_b_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, -1, 0, 0)) < 0)
        raydium_log("����������ʧ��");
    if((ray_l_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, 0, 1, 0)) < 0)
        raydium_log("�����������ʧ��");
    if((ray_r_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, 0, -1, 0)) < 0)
        raydium_log("�����������ʧ��");
    if((ray_fl_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, 1, 1, 0)) < 0)
        raydium_log("�����ǰ����ʧ��");
    if((ray_fr_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, 1, -1, 0)) < 0)
        raydium_log("�����ǰ����ʧ��");
    if((ray_up_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, 0, 0, 1)) < 0)
        raydium_log("�����������ʧ��");
    if((ray_down_id = raydium_ode_element_ray_attach_name(name, RAY_LENGTH, 0, 0, -1)) < 0)
        raydium_log("�����������ʧ��");

    //����������������ƶ�
    dReal dRayPos[3] = { 0, 0, -0.2};
    for(int i=0; i<6; ++i)
    {
        raydium_ode_element_ray_pos_name("corps", i, dRayPos);
    }

}

void prelearn_record_track_type(int type)
{
    originalInfo.track_type = type;
}

bool prelearn_check_track_type()
{
    return (originalInfo.track_type > 0);
}

void prelearn_read_user_info()
{
    char str[RAYDIUM_MAX_NAME_LEN];

    raydium_parser_db_get("player_age", str, "24");
    player_age = atoi(str);
    strcpy(player_age_buf, str);
    userinfo.age = player_age;
/*
    raydium_parser_db_get("player_type", player_type, "Beginner");
    raydium_parser_db_get("player_sex", player_sex, "Male");
    raydium_parser_db_get("player_game_frequency", player_game_frequency, "2-8 hours/week");
*/
    raydium_parser_db_get("player_type", str, "-1");
    userinfo.type = atoi(str);
    raydium_parser_db_get("player_sex", str, "-1");
    userinfo.sex = atoi(str);
    raydium_parser_db_get("player_game_frequency", str, "-1");
    userinfo.frequency = atoi(str);
    //player_game_frequency = atoi(str);
}

//��ȡ���洢�û���Ϣ����
void prelearn_handle_user_info()
{
    char str[RAYDIUM_MAX_NAME_LEN];
    //��ȡ����
    raydium_gui_read_name("menu","edtPlayerName",raydium_network_name_local);
    strcpy(userinfo.name, raydium_network_name_local);
    userinfo.sex = raydium_gui_read_name("menu", "cboPlayerSex", player_sex);
    raydium_gui_read_name("menu", "edtPlayerAge", str);
    player_age = atoi(str);
    userinfo.age = player_age;
    userinfo.frequency = raydium_gui_read_name("menu", "cboPlayerFrequency", player_game_frequency);
    userinfo.type = raydium_gui_read_name("menu", "cboPlayerType", player_type);

    //��������
    raydium_parser_db_set("Generic-PlayerName",raydium_network_name_local);
    itoa(player_age, str, 10);
    raydium_parser_db_set("player_age", str);
    itoa(userinfo.type, str, 10);
    raydium_parser_db_set("player_type", str);
    itoa(userinfo.sex, str, 10);
    raydium_parser_db_set("player_sex", str);
    itoa(userinfo.frequency, str, 10);
    raydium_parser_db_set("player_game_frequency", str);
}

int prelearn_get_session_emotion()
{
    int fun1 = vecLevelStore[0].fun;
    int fun2 = vecLevelStore[1].fun;

    if(fun1 == fun2)
    {
        return PREFERENCE_FUN;
    }

    int emotion1 = vecLevelStore[0].emotion;
    int emotion2 = vecLevelStore[1].emotion;
    int degree1 = vecLevelStore[0].emotion_degree;
    int degree2 = vecLevelStore[1].emotion_degree;

    if(emotion1 == emotion2)
    {
        return emotion1;
    }
    else
    {
        return PREFERENCE_FUN;
    }
}

//void prelearn_session_end(int type, char *buff)
//{
//    raydium_log("session_end");
//    int old;
//
//    old=mode;
//    mni_current[0]=0;
//    message[0]=0;
//    raydium_sound_SourceStop(sound_car);
//    raydium_ode_time_change(0);
//    mode=MODE_NONE;
//
//    raydium_clear_frame();
//    raydium_camera_look_at(0.1,0.1,0,0,1,0);
//    raydium_osd_draw_name("mania_logo2.tga",0,0,100,100);
//    raydium_rendering_finish();
//
//    if(raydium_gui_window_find("menu")>=0)
//    {
//        raydium_gui_window_delete_name("menu");
//        raydium_gui_hide();
//    }
//
//    prelearn_build_gui_4AFC();
//}
