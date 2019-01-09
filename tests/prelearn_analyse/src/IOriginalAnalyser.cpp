#include "../include/IOriginalAnalyser.h"
#include <math.h>

IOriginalAnalyser::IOriginalAnalyser()
    : m_nLastTrackType(0),
    m_bLog(false)
{
    //ctor
}

IOriginalAnalyser::~IOriginalAnalyser()
{
    //dtor
}

/*
///带赛道类型的分析
void IOriginalAnalyser::track_type_statictics(std::vector<stTimeType>& vecValueTime, CAnalyseTypeResult& result)
{
    std::vector<stTimeType>::iterator it;
    for(it = vecValueTime.begin(); it != vecValueTime.end(); ++it)
    {
        float value = it->time;
        int type = it->type - TRACK_TYPE_BEGIN;

        stStaticResult &sr = result.analyse_result[m_nAnalyseType][type];
        ++sr.nCount;
        sr.fSum += value;
        if(value < sr.fMin)
            sr.fMin = value;
        if(value > sr.fMax)
            sr.fMax = value;
    }

    for(int i=0; i<TRACK_TYPE_NUM; ++i)
    {
        stStaticResult &sr = result.analyse_result[m_nAnalyseType][i];
        if(sr.nCount > 0)
            sr.fAvg = sr.fSum / sr.nCount;
    }

    for(it = vecValueTime.begin(); it != vecValueTime.end(); ++it)
    {
        float value = it->time;
        int type = it->type - TRACK_TYPE_BEGIN;

        stStaticResult &sr = result.analyse_result[m_nAnalyseType][type];
        sr.fVariance += pow(value - sr.fAvg, 2);
    }
}
*/
