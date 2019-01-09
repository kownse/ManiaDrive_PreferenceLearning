#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#define SAFE_DELETE(p) if(p) delete p;

#define ANALYSE_TYPE_COLLISION          0
#define ANALYSE_TYPE_COLLEAP            1
#define ANALYSE_TYPE_LEAP               2
#define ANALYSE_TYPE_USER_LEAPCONTROL   3
#define ANALYSE_TYPE_USER_COLCONTROL    4

#define ANALYSE_TYPE_USER_FORWARD       5
#define ANALYSE_TYPE_USER_BACK          6
#define ANALYSE_TYPE_USER_LEFT          7
#define ANALYSE_TYPE_USER_RIGHT         8
#define ANALYSE_TYPE_USER_FORWARD_LEFT  9
#define ANALYSE_TYPE_USER_FORWARD_RIGHT 10

#define ANALYSE_TYPE_BIGAGL             11
#define ANALYSE_TYPE_OVERTURN           12

#define ANALYSE_TYPE_SPEED              13
#define ANALYSE_TYPE_AGLSPEED           14
#define ANALYSE_TYPE_RAY                15

#define ANALYSE_TYPE_MAX                16

#define TRACK_TYPE_BEGIN        200
#define TRACK_TYPE_STRAIGHT     200
#define TRACK_TYPE_CROOK        201
#define TRACK_TYPE_BUMP         202
#define TRACK_TYPE_UPWARD       203
#define TRACK_TYPE_DOWNWARD     204
#define TRACK_TYPE_SPRING       205
#define TRACK_TYPE_END          205

#define TRACK_TYPE_NUM          (TRACK_TYPE_END - TRACK_TYPE_STRAIGHT + 1)

#define STATISTIC_TYPE          5
#define TRACK_STATISTIC_TYPE    4

#include <string>

static std::string szTrackType[] = { "st", "cr", "bp", "up", "dn", "sp"};
static std::string szAnalyseType[] = { "col","cl","l","u_lc","u_cc",
                                        "u_f", "u_b", "u_l", "u_r", "u_fl", "u_fr",
                                        "ba", "ot", "sp", "ap", "ray"};
static std::string szStatistic[] = { "cnt", "sum", "avg", "ent", "var", "min", "max"};

typedef struct _stTimeType
{
    float time;
    int   type;

    _stTimeType(float t, int tp) : time(t), type(tp)
    {
    }
}stTimeType;

///计算平均值，最大值，标准差
typedef struct _stStaticResult
{
    int nCount;
    float fSum;
    float fAvg;
    float fMin;
    float fMax;
    float fVariance;
    float fEntropy;

    _stStaticResult(int cnt, float sum, float avg, float min, float max, float var)
        :nCount(cnt),fSum(sum),fAvg(avg),fMin(min),fMax(max),fVariance(var)
    {
    }

    _stStaticResult()
        :nCount(-1),fSum(-1),fAvg(-1),fMin(-1),fMax(-1),fVariance(-1)
    {
    }
}stStaticResult;

typedef struct _stTrackBlock
{
    float x;
    float y;
    float z;
    float tx;
    float ty;
    float tz;
    int type;

    _stTrackBlock(float xt, float yt, float zt, float txt, float tyt, float tzt, float tt)
        : x(xt), y(yt), z(zt), tx(txt), ty(tyt), tz(tzt), type(tt)
        {
        }
    _stTrackBlock()
        : x(0), y(0), z(0), tx(0), ty(0), tz(0), type(0)
    {
    }
}stTrackBlock;

struct DLL_EXPORT CAnalyseTypeResult
{
    CAnalyseTypeResult();
    ~CAnalyseTypeResult();

    void init();

    int id;
    /*
    float partytime;            //总尝试的时间
    float finishProportionaTotal;  //完成比例
    int num_attempt;                 //尝试次数
    int num_finished;                //通关次数
    int difficult;                  //玩家评判的困难程度
    int skill;                      //玩家对自己在该赛道中的技术评价
    int emotion;                       //感受
    int fun;                        //情感
    int emotion_degree;                  //情感程度
    */
    stStaticResult analyse_result[TRACK_TYPE_NUM][ANALYSE_TYPE_MAX];
};

struct DLL_EXPORT CTrackResult
{
    CTrackResult();
    ~CTrackResult();

    void init();

    char szName[255];
    float fTime;
    unsigned long nNameLength;
    int nTotal;

    stStaticResult track_result[TRACK_TYPE_NUM];
};

#endif // DEFINE_H_INCLUDED
