#include "../include/ITimer.h"

/*
ITimer::ITimer()
    :m_pListener(0)
{
    //dtor
}
*/

ITimer::ITimer(ITimerListener* pListener,int flag)
    :m_pListener(pListener),
    m_nFlag(flag)
{
    //ctor
}

ITimer::~ITimer()
{
    //dtor
}
