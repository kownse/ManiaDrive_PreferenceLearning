#include "../include/CUserInputAnalyser.h"
#include "../define.h"
#include "../include/CIntrptTimer.h"
#include <stdio.h>

const float tInputIntervalThread = 0.5f;
const float tInputThread = 0.5f;

CUserInputAnalyser::CUserInputAnalyser()
    : m_pForwardTimer(new CIntrptTimer(this,ANALYSE_TYPE_USER_FORWARD, tInputIntervalThread)),
    m_pBackTimer(new CIntrptTimer(this,ANALYSE_TYPE_USER_BACK, tInputIntervalThread)),
    m_pLeftTimer(new CIntrptTimer(this,ANALYSE_TYPE_USER_LEFT, tInputIntervalThread)),
    m_pRightTimer(new CIntrptTimer(this,ANALYSE_TYPE_USER_RIGHT, tInputIntervalThread)),
    m_pForwardLeftTimer(new CIntrptTimer(this,ANALYSE_TYPE_USER_FORWARD_LEFT, tInputIntervalThread)),
    m_pForwardRightTimer(new CIntrptTimer(this,ANALYSE_TYPE_USER_FORWARD_RIGHT, tInputIntervalThread))
{
    //ctor
}

CUserInputAnalyser::~CUserInputAnalyser()
{
    //dtor
    SAFE_DELETE(m_pForwardTimer);
    SAFE_DELETE(m_pBackTimer);
    SAFE_DELETE(m_pLeftTimer);
    SAFE_DELETE(m_pRightTimer);
    SAFE_DELETE(m_pForwardLeftTimer);
    SAFE_DELETE(m_pForwardRightTimer);
}

void CUserInputAnalyser::DoAnalyse(c2s_driveinfo_attempt_original &original)
{
    if(IsTrackType(original.track_type))
    {
        if(m_nLastTrackType != original.track_type)
        {
            m_pForwardTimer->TimeDone(true);
            m_pBackTimer->TimeDone(true);
            m_pLeftTimer->TimeDone(true);
            m_pRightTimer->TimeDone(true);
            m_pForwardLeftTimer->TimeDone(true);
            m_pForwardRightTimer->TimeDone(true);
        }
        m_nLastTrackType = original.track_type;
    }

    int bForward = original.bForward && !original.bTurnLeft && !original.bTurnRight;
    int bLeft = original.bTurnLeft && !original.bForward && !original.bBack;
    int bRight = original.bTurnRight && !original.bForward && !original.bBack;
    int bForwardLeft = original.bForward && original.bTurnLeft;
    int bForwardRight = original.bForward && original.bTurnRight;
    int bBack = original.bBack;
    int bConflit = (original.bForward && original.bBack) || (original.bTurnLeft && original.bTurnRight);

    m_pForwardTimer->DoTime(bForward, original.fTime, original.fStep, 1);
    m_pBackTimer->DoTime(bBack, original.fTime, original.fStep, 1);
    m_pLeftTimer->DoTime(bLeft, original.fTime, original.fStep, 1);
    m_pRightTimer->DoTime(bRight, original.fTime, original.fStep, 1);
    m_pForwardLeftTimer->DoTime(bForwardLeft, original.fTime, original.fStep, 1);
    m_pForwardRightTimer->DoTime(bForwardRight, original.fTime, original.fStep, 1);

}
void CUserInputAnalyser::TimerDone(float fTime, int flag)
{
    if(fTime < 0)
    {
        printf("错误的计时(输入分析):%f\n", fTime);
        return;
    }

    switch(flag)
    {
    case ANALYSE_TYPE_USER_FORWARD:
        if(m_bLog)
            printf("前进:\t%f\n", fTime);
        m_vForwardTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_USER_BACK:
        if(m_bLog)
            printf("后退:\t%f\n", fTime);
        m_vBackTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_USER_LEFT:
        if(m_bLog)
            printf("左转:\t%f\n", fTime);
        m_vLeftTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_USER_RIGHT:
        if(m_bLog)
            printf("右转:\t%f\n", fTime);
        m_vRightTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_USER_FORWARD_LEFT:
        if(m_bLog)
            printf("前进左转:\t%f\n", fTime);
        m_vForwardLeftTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_USER_FORWARD_RIGHT:
        if(m_bLog)
            printf("前进右转:\t%f\n", fTime);
        m_vForwardRightTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    }
}

void CUserInputAnalyser::Reset(void)
{
    m_pForwardTimer->Reset();
    m_pBackTimer->Reset();
    m_pLeftTimer->Reset();
    m_pRightTimer->Reset();
    m_pForwardLeftTimer->Reset();
    m_pForwardRightTimer->Reset();

    m_vForwardTime.clear();
    m_vBackTime.clear();
    m_vLeftTime.clear();
    m_vRightTime.clear();
    m_vForwardLeftTime.clear();
    m_vForwardRightTime.clear();
}

void CUserInputAnalyser::GetStatisticResult(CAnalyseTypeResult& result)
{
    if(m_vForwardTime.size()>0)
    {
        track_type_statictics(m_pForwardTimer->GetType(), m_vForwardTime, result);
    }
    if(m_vBackTime.size()>0)
    {
        track_type_statictics(m_pBackTimer->GetType(), m_vBackTime, result);
    }

    if(m_vLeftTime.size()>0)
    {
        track_type_statictics(m_pLeftTimer->GetType(), m_vLeftTime, result);
    }

    if(m_vRightTime.size()>0)
    {
        track_type_statictics(m_pRightTimer->GetType(), m_vRightTime, result);
    }

    if(m_vForwardLeftTime.size()>0)
        track_type_statictics(m_pForwardLeftTimer->GetType(), m_vForwardLeftTime, result);

    if(m_vForwardRightTime.size()>0)
        track_type_statictics(m_pForwardRightTimer->GetType(), m_vForwardRightTime, result);
}
