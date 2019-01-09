#ifndef CCONTINTIMER_H
#define CCONTINTIMER_H

#include "../pre_analyse.h"
#include "ITimer.h"


class CContinTimer : public ITimer
{
    public:
        CContinTimer(ITimerListener* pListener,int flag, float liminal);
        virtual ~CContinTimer();

        void DoTime(int bMark, float fTime, float fStep, unsigned char bResetFlag);
        void TimeDone(bool bTriger);
        void Reset(void);
        bool IsTimerOn(void);

    protected:
    private:
        int m_bMark;
        int m_bFlag;
        float m_tPile;
        const float m_fLiminal;
};

#endif // CCONTINTIMER_H
