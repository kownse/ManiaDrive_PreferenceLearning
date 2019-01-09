#include "../include/CContinTimer.h"

CContinTimer::CContinTimer(ITimerListener* pListener, int flag, float liminal)
    :ITimer(pListener, flag),
    m_bMark(0),
    m_bFlag(0),
    m_tPile(0.0f),
    m_fLiminal(liminal)
{
    //ctor
}

CContinTimer::~CContinTimer()
{
    //dtor
}

void CContinTimer::DoTime(int bMark, float fTime, float fStep, unsigned char bResetFlag)
{
    m_bMark = bMark;
    if(m_bMark)
    {
        m_bFlag = 1;
        m_tPile += fStep;
    }
    else
    {
        if(m_tPile > m_fLiminal)
        {
            if(m_bFlag && m_pListener)
                m_pListener->TimerDone(m_tPile, m_nFlag);
        }
        m_tPile = 0.0f;
        m_bFlag = 0;
    }
    if(bResetFlag)
    {
        m_bMark = 1;
    }
}

void CContinTimer::Reset(void)
{
    m_bMark = 0;
    m_bFlag = 0;
    m_tPile = 0.0f;
}

void CContinTimer::TimeDone(bool bTriger)
{
    if(m_bMark && bTriger && (m_tPile > m_fLiminal) && m_pListener)
        m_pListener->TimerDone(m_tPile, m_nFlag);
}

bool CContinTimer::IsTimerOn(void)
{
    return (bool)m_bFlag;
}
