#include "../include/CIntrptTimer.h"

CIntrptTimer::CIntrptTimer(ITimerListener* pListener, int flag, float liminal)
    :ITimer(pListener, flag),
    m_bMark(0),
    m_bFlag(0),
    m_tStart(0.0f),
    m_tEnd(0.0f),
    m_tPile(0.0f),
    m_fLiminal(liminal)
{
    //ctor
}

CIntrptTimer::~CIntrptTimer()
{
    //dtor
}

void CIntrptTimer::DoTime(int bMark, float fTime, float fStep, unsigned char bResetFlag)
{
    m_bMark = bMark;
    if(m_bMark)
    {
        if(m_bFlag)
        {
            m_tPile = 0.0f;
        }
        m_bFlag = 1;

        if(m_tStart < 0.0001f)
        {
            m_tStart = fTime;
            m_tEnd = fTime + fStep;
        }
        else
        {
            m_tEnd = fTime;
        }
    }

    if(bResetFlag)
        m_bMark = 0;

    if(m_bFlag)
    {
        //����ֵ��Χ���ۼ�
        if(m_tPile < m_fLiminal)
        {
            m_tPile += fStep;
        }
        else
        {
            m_tPile = 0.0f;
            m_bFlag = 0;

            float tTotal = m_tEnd - m_tStart;
            m_tStart = m_tEnd = 0.0f;
            //����ֵ��Χ��ر��ۼ�
            if(m_pListener)
                m_pListener->TimerDone(tTotal, m_nFlag);
        }

    }
}

void CIntrptTimer::TimeDone(bool bTriger)
{
    if(m_bMark)
    {
        float tTotal = m_tEnd - m_tStart;
        m_tStart = m_tEnd = 0.0f;
        //����ֵ��Χ��ر��ۼ�
        if(bTriger && m_pListener)
            m_pListener->TimerDone(tTotal, m_nFlag);
    }
}

void CIntrptTimer::Reset(void)
{
    m_bMark = 0;
    m_bFlag = 0;
    m_tPile = 0;
    m_tStart = 0;
    m_tEnd = 0;
}

bool CIntrptTimer::IsTimerOn(void)
{
    return m_bFlag > 0;
}
