#include "../include/CCollisionAnalyser.h"
#include "../include/CIntrptTimer.h"
#include "../include/CContinTimer.h"
#include <stdio.h>
#include "../func.h"
#include "../define.h"

const float tColIntervalThread = 1.0f;        //Åö×²¼û¼ä¸ô
const float tColContinueThread = 0.5f;        //Á¬ÐøÅö×²ãÐÖµ
const float tColLeapLiminal = 0.2f;           //Åö×²Ðü¿Õ×î¶ÌÊ±¼ä
const float tLeapLiminal = 0.2f;              //Ðü¿Õ×î¶ÌÊ±¼ä

CCollisionAnalyser::CCollisionAnalyser()
    : m_pColTimer(new CIntrptTimer(this, ANALYSE_TYPE_COLLISION,tColIntervalThread)),
    m_pColLeapTimer(new CContinTimer(this, ANALYSE_TYPE_COLLEAP, tColLeapLiminal)),
    m_pLeapTimer(new CContinTimer(this, ANALYSE_TYPE_LEAP, tLeapLiminal)),
    m_pColControlTimer(new CIntrptTimer(this, ANALYSE_TYPE_USER_COLCONTROL, tLeapLiminal)),
    m_pLeapControlTimer(new CIntrptTimer(this, ANALYSE_TYPE_USER_LEAPCONTROL, tLeapLiminal))
    {

    }

CCollisionAnalyser::~CCollisionAnalyser()
{
    //dtor
    SAFE_DELETE(m_pColTimer);
    SAFE_DELETE(m_pColLeapTimer);
    SAFE_DELETE(m_pLeapTimer);
    SAFE_DELETE(m_pColControlTimer);
    SAFE_DELETE(m_pLeapControlTimer);
}

void CCollisionAnalyser::DoAnalyse(c2s_driveinfo_attempt_original &original)
{
    if(IsTrackType(original.track_type))
    {
        if(m_nLastTrackType != original.track_type)
        {
            m_pColTimer->TimeDone(true);
            m_pColLeapTimer->TimeDone(true);
            m_pLeapTimer->TimeDone(true);
            m_pColControlTimer->TimeDone(true);
            m_pLeapControlTimer->TimeDone(true);
        }
        m_nLastTrackType = original.track_type;
    }

    int bCollision = original.bCollision;
    m_pColTimer->DoTime(bCollision, original.fTime, original.fStep, 1);

    int bColLeap = original.bLeap && m_pColTimer->IsTimerOn();
    int bLeap = original.bLeap && !m_pColTimer->IsTimerOn();

    m_pColLeapTimer->DoTime(bColLeap, original.fTime, original.fStep, 1);
    m_pLeapTimer->DoTime(bLeap, original.fTime, original.fStep, 1);

    int bControl = original.bForward || original.bBack || original.bTurnLeft || original.bTurnRight;
    int bColControl = bCollision && bControl;
    int bLeapControl = bLeap && bControl;

    m_pColControlTimer->DoTime(bColControl, original.fTime, original.fStep, 1);
    m_pLeapControlTimer->DoTime(bLeapControl, original.fTime, original.fStep, 1);
}

void CCollisionAnalyser::TimerDone(float fTime, int flag)
{
    if(fTime < 0)
    {
        printf("´íÎóµÄ¼ÆÊ±(Åö×²·ÖÎö):%f\n", fTime);
        return;
    }

    switch(flag)
    {
    case ANALYSE_TYPE_COLLISION:
        if(m_bLog)
            printf("Åö×²:\t%f\n", fTime);
        m_vColTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_COLLEAP:
        if(m_bLog)
            printf("Åö×²Ðü¿Õ:\t%f\n", fTime);
        m_vColLeapTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_LEAP:
        if(m_bLog)
            printf("Ðü¿Õ:\t%f\n", fTime);
        m_vLeapTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_USER_COLCONTROL:
        if(m_bLog)
            printf("Åö×²¿ØÖÆ:\t%f\n", fTime);
        m_vColControlTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    case ANALYSE_TYPE_USER_LEAPCONTROL:
        if(m_bLog)
            printf("Ðü¿Õ¿ØÖÆ:\t%f\n", fTime);
        m_vLeapControlTime.push_back(stTimeType(fTime, m_nLastTrackType));
        break;
    default:
        printf("error type %d", flag);
    }


}

void CCollisionAnalyser::Reset(void)
{
    m_pColTimer->Reset();
    m_vColTime.clear();

    m_pColLeapTimer->Reset();
    m_vColLeapTime.clear();

    m_pLeapTimer->Reset();
    m_vLeapTime.clear();

    m_pColControlTimer->Reset();
    m_vColControlTime.clear();

    m_pLeapControlTimer->Reset();
    m_vLeapControlTime.clear();
}

void CCollisionAnalyser::GetStatisticResult(CAnalyseTypeResult& result)
{
    /*
    if(m_vConColTime.size() > 0 && m_vSlitColTime.size()>0)
    {
        vecResult.push_back(basic_statistics(m_vConColTime));
        vecResult.push_back(basic_statistics(m_vSlitColTime));
    }
    */
    if(m_vColTime.size()>0)
    {
        track_type_statictics(m_pColTimer->GetType(), m_vColTime, result);
        //vecResult.push_back(basic_statistics(m_vColTime));
    }

    if(m_vColLeapTime.size()>0)
    {
        track_type_statictics(m_pColLeapTimer->GetType(), m_vColLeapTime, result);
    }

    if(m_vLeapTime.size()>0)
    {
        track_type_statictics(m_pLeapTimer->GetType(), m_vLeapTime, result);
    }

    if(m_vLeapControlTime.size()>0)
    {
        track_type_statictics(m_pLeapControlTimer->GetType(), m_vLeapControlTime, result);
    }

    if(m_vColControlTime.size()>0)
    {
        track_type_statictics(m_pColControlTimer->GetType(), m_vColControlTime, result);
    }
}

bool CCollisionAnalyser::IsCollision()
{
    if(m_pColLeapTimer)
        return m_pColLeapTimer->IsTimerOn();
    else
        return false;
}

bool CCollisionAnalyser::IsLeap()
{
    if(m_pLeapTimer && m_pColLeapTimer)
        return m_pLeapTimer->IsTimerOn() || m_pColLeapTimer->IsTimerOn();
    else
        return false;
}
