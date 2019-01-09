#include "define.h"
#include <limits>

CAnalyseTypeResult::CAnalyseTypeResult()
{
    init();
}

CAnalyseTypeResult::~CAnalyseTypeResult()
{

}

void CAnalyseTypeResult::init()
{
    /*
    for(int i=0; i<ANALYSE_TYPE_MAX; ++i)
        for(int j=0; j<TRACK_TYPE_NUM; ++j)
        {
            analyse_result[i][j].nCount = 0;
            analyse_result[i][j].fSum = 0.0f;
            analyse_result[i][j].fAvg = 0.0f;
            analyse_result[i][j].fMin = std::numeric_limits<float>::max();
            analyse_result[i][j].fMax = std::numeric_limits<float>::min();
            analyse_result[i][j].fVariance = 0.0f;
        }
    */

    for(int i=0; i<TRACK_TYPE_NUM; ++i)
        for(int j=0; j<ANALYSE_TYPE_MAX; ++j)
        {
            analyse_result[i][j].nCount = 0;
            analyse_result[i][j].fSum = 0.0f;
            analyse_result[i][j].fAvg = 0.0f;
            analyse_result[i][j].fEntropy = 0.0f;
            analyse_result[i][j].fMin = 100.0f;//std::numeric_limits<float>::max();
            analyse_result[i][j].fMax = 0.0f;//std::numeric_limits<float>::min();
            analyse_result[i][j].fVariance = 0.0f;
        }
}

CTrackResult::CTrackResult()
{
    init();
}

CTrackResult::~CTrackResult()
{

}

void CTrackResult::init()
{
    for(int i=0; i<TRACK_TYPE_NUM; ++i)
    {
        track_result[i].nCount = 0;
        track_result[i].fEntropy = 0;
        track_result[i].fAvg = 0.0f;
        track_result[i].fMax = -100;
        track_result[i].fMin = 100;
        track_result[i].fSum = 0;
        track_result[i].fVariance = 0;
    }
}
