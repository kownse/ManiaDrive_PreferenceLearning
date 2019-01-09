#ifndef CINTRPTTIMER_H
#define CINTRPTTIMER_H

#include "../pre_analyse.h"
#include "ITimer.h"


class CIntrptTimer : public ITimer
{
    public:
        CIntrptTimer(ITimerListener* pListener, int flag, float liminal);
        virtual ~CIntrptTimer();

        void DoTime(int bMark, float fTime, float fStep, unsigned char bResetFlag);
        void TimeDone(bool bTriger);
        void Reset(void);
        bool IsTimerOn(void);

    protected:
    private:
        int m_bMark;
        unsigned char m_bFlag;
        float m_tStart;
        float m_tEnd;
        float m_tPile;
        const float m_fLiminal;
};

#endif // CINTRPTTIMER,_H
