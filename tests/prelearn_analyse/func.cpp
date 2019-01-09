#include "func.h"
#include "define.h"
#include <math.h>
#include <limits>

//具体处理收集结果的函数指针
typedef void (*f_tw_col_done)(float tTotal);

///收集允许有中断但满足一定时间窗口的数据
void col_tw_info(int &bMark,
                 unsigned char& bFlag,
                 float &tStart,
                 float &tEnd,
                 float &tPile,
                 float fTime,
                 float step,
                 const float tIntervalThread,
                 unsigned char bClsFlag,
                 f_tw_col_done fcd
                )
{
//   col_pile(bMark, bFlag, tStart, tEnd, tPile, fTime, step);
    if(bMark)
    {
        if(bFlag)
        {
            tPile = 0.0f;
        }
        bFlag = 1;

        if(tStart < 0.0001f)
        {
            tStart = fTime;
            tEnd = fTime + step;
        }
        else
        {
            tEnd = fTime;
        }
    }

    if(bClsFlag)
        bMark = 0;

    if(bFlag)
    {
        //在阈值范围内累计
        if(tPile < tIntervalThread)
        {
            tPile += step;
        }
        else
        {
            tPile = 0.0f;
            bFlag = 0;

            float tTotal = tEnd - tStart;
            tStart = tEnd = 0.0f;
            //在阈值范围外关闭累计
            fcd(tTotal);
        }

    }
}

typedef void (*f_con_col_done)(float tPile);

///收集不能中断只能持续的数据
void col_con_info(int &bMark,
                  float &tPile,
                  float step,
                  const float tThredhold,
                  unsigned char bResetFlag,
                  f_con_col_done fcd)
{
    if(bMark)
    {
        tPile += step;
    }
    else
    {
        if(tPile > tThredhold)
        {
            fcd(tPile);
        }
        tPile = 0.0f;
    }
    if(bResetFlag)
    {
        bMark = 1;
    }
}

///计算平均值，最大值，标准差
stStaticResult basic_statistics(std::vector<float>& vecValues)
{
    int nCount = vecValues.size();
    float fSum = 0.0f;
    float fMin = std::numeric_limits<float>::max();
    float fMax = std::numeric_limits<float>::min();

    std::vector<float>::iterator it;
    for(it = vecValues.begin(); it != vecValues.end(); ++it)
    {
        float value = *it;
        fSum += value;
        if(value < fMin)
            fMin = value;
        if(value > fMax)
            fMax = value;
    }

    float fAvg = fSum / nCount;

    float fDiv = 0.0f;
    for(it = vecValues.begin(); it != vecValues.end(); ++it)
    {
        float value = *it;
        fDiv += pow(value - fAvg, 2);
    }
    float fVariance = sqrt(fDiv / nCount);
    return stStaticResult(nCount, fSum, fAvg, fMin, fMax, fVariance);
}

void track_type_statictics(const int nAnalyseType, std::vector<stTimeType>& vecValueTime, CAnalyseTypeResult& result)
{
    std::vector<stTimeType>::iterator it;
    for(it = vecValueTime.begin(); it != vecValueTime.end(); ++it)
    {
        if(!IsTrackType(it->type))
            continue;

        float value = it->time;
        int type = it->type - TRACK_TYPE_BEGIN;



        stStaticResult &sr = result.analyse_result[type][nAnalyseType];
        ++sr.nCount;
        sr.fSum += value;
        if(value < sr.fMin)
            sr.fMin = value;
        if(value > sr.fMax)
            sr.fMax = value;
    }

    for(int i=0; i<TRACK_TYPE_NUM; ++i)
    {
        stStaticResult &sr = result.analyse_result[i][nAnalyseType];
        if(sr.nCount > 0)
            sr.fAvg = sr.fSum / sr.nCount;
    }

    for(it = vecValueTime.begin(); it != vecValueTime.end(); ++it)
    {
        if(!IsTrackType(it->type))
            continue;

        float value = it->time;
        int type = it->type - TRACK_TYPE_BEGIN;

        stStaticResult &sr = result.analyse_result[type][nAnalyseType];

        if(sr.nCount <= 0)
            continue;

        sr.fVariance += pow(value - sr.fAvg, 2);

        float propaty = value/sr.fSum;
        sr.fEntropy += propaty * log(propaty);
    }

    for(int i=0; i<TRACK_TYPE_NUM; ++i)
    {
        stStaticResult &sr = result.analyse_result[i][nAnalyseType];

        if(sr.nCount > 0)
        {
            if(sr.fEntropy != 0 && sr.fSum != 1)
                sr.fEntropy *= -1/log(sr.fSum);

            if(sr.fEntropy < 0.0001f)
                sr.fEntropy = 0;
        }
    }
}

bool IsTrackType(int type)
{
    return (type >= TRACK_TYPE_BEGIN && type <= TRACK_TYPE_END);
}

///赛道分析
void track_statictics(int nTrackType, std::vector<float>& vecValue, CTrackResult& result)
{
    if(!IsTrackType(nTrackType))
        return;

    std::vector<float>::iterator it;
    stStaticResult &sr = result.track_result[nTrackType - TRACK_TYPE_STRAIGHT];
    for(it = vecValue.begin(); it != vecValue.end(); ++it)
    {
        float value = *it;

        ++sr.nCount;
        sr.fSum += value;
        if(value < sr.fMin)
            sr.fMin = value;
        if(value > sr.fMax)
            sr.fMax = value;
    }

    if(sr.nCount > 0)
        sr.fAvg = sr.fSum / sr.nCount;

    for(it = vecValue.begin(); it != vecValue.end(); ++it)
    {
        if(sr.nCount <= 0)
            continue;

        float value = *it;
        sr.fVariance += pow(value - sr.fAvg, 2);

        float propaty = value/sr.fSum;
        sr.fEntropy += propaty * log(propaty);
    }
    if(sr.fEntropy != 0 && sr.fSum != 1)
        sr.fEntropy *= -1/log(sr.fSum);
}

