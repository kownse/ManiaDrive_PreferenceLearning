#ifndef ITIMER_H
#define ITIMER_H
//#include "../pre_analyse.h"
#include "ITimerListener.h"

class ITimer
{
    public:
        //ITimer();
        ITimer(ITimerListener* pListener, int flag);
        virtual ~ITimer();

        virtual void DoTime(int bMark, float fTime, float fStep, unsigned char bResetFlag) = 0;
        virtual void TimeDone(bool bTriger) = 0;        ///÷’÷πº∆ ±
        virtual void Reset(void) = 0;
        virtual bool IsTimerOn(void) = 0;

        const int GetType(){ return m_nFlag; }
    protected:
        ITimerListener* m_pListener;
        const int m_nFlag;
    private:
};

#endif // ITIMER_H
