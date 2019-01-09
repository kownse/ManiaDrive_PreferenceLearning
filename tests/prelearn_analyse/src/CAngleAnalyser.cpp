#include <stdio.h>
#include <math.h>
#include "../include/CAngleAnalyser.h"
#include "../include/CIntrptTimer.h"
#include "../include/CContinTimer.h"
#include "../func.h"
#include "../define.h"

const float tBigAglLiminal = 0.5f;
const float tOverTurnLiminal = 0.5f;
const float fBigAgl = 2.0f;

CAngleAnalyser::CAngleAnalyser()
    : m_pBigAglTimer(new CIntrptTimer(this, ANALYSE_TYPE_BIGAGL,tBigAglLiminal)),
    m_pOverTurnTimer(new CIntrptTimer(this, ANALYSE_TYPE_OVERTURN, tOverTurnLiminal))
{
    //ctor
}

CAngleAnalyser::~CAngleAnalyser()
{
    //dtor
    SAFE_DELETE(m_pBigAglTimer);
    SAFE_DELETE(m_pOverTurnTimer);
}

void CAngleAnalyser::DoAnalyse(c2s_driveinfo_attempt_original &original)
{
    if(IsTrackType(original.track_type))
    {
        if(m_nLastTrackType != original.track_type)
        {
            m_pBigAglTimer->TimeDone(true);
            m_pOverTurnTimer->TimeDone(true);
        }
        m_nLastTrackType = original.track_type;
    }

    float fAngularSpeed = sqrt(original.pAngSpeed[0]*original.pAngSpeed[0] + original.pAngSpeed[1]*original.pAngSpeed[1] + original.pAngSpeed[2]*original.pAngSpeed[2]);
    int bBigAgl = (fAngularSpeed > fBigAgl) ? 1 : 0;
    m_pBigAglTimer->DoTime(bBigAgl, original.fTime, original.fStep, 1);

    int bOverTurn = (abs(original.pRotate[0]) > 0.3f || abs(original.pRotate[1]) > 0.3f)? 1 : 0;
    m_pOverTurnTimer->DoTime(bOverTurn,  original.fTime, original.fStep, 1);
}

void CAngleAnalyser::TimerDone(float fTime, int flag)
{
    if(fTime < 0)
    {
        printf("错误的计时(角度分析):%f\n", fTime);
        return;
    }

    switch(flag)
    {
    case ANALYSE_TYPE_BIGAGL:
        if(m_bLog)
            printf("极大角速度:\t%f\n", fTime);
        m_vBigAglTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_OVERTURN:
        if(m_bLog)
            printf("翻车:\t%f\n", fTime);
        m_vOverTurnTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    default:
        printf("error type %d", flag);
    }


}

void CAngleAnalyser::Reset(void)
{
    m_pBigAglTimer->Reset();
    m_vBigAglTime.clear();

    m_pOverTurnTimer->Reset();
    m_vOverTurnTime.clear();
}

void CAngleAnalyser::GetStatisticResult(CAnalyseTypeResult& result)
{

    if(m_vBigAglTime.size()>0)
    {
        track_type_statictics(m_pBigAglTimer->GetType(), m_vBigAglTime, result);
        //vecResult.push_back(basic_statistics(m_vColTime));
    }

    if(m_vOverTurnTime.size()>0)
    {
        track_type_statictics(m_pOverTurnTimer->GetType(), m_vOverTurnTime, result);
    }
}
