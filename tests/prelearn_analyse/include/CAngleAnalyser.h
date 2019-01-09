#ifndef CANGLEANALYSER_H
#define CANGLEANALYSER_H

#include "ITimerListener.h"
#include "IOriginalAnalyser.h"
#include "ITimer.h"
#include "../define.h"

#include <vector>

class CAngleAnalyser : public IOriginalAnalyser, ITimerListener
{
    public:
        CAngleAnalyser();
        ~CAngleAnalyser();

        void DoAnalyse(c2s_driveinfo_attempt_original &original);
        void TimerDone(float fTime, int flag);
        void GetStatisticResult(CAnalyseTypeResult& result);
        void Reset(void);

    protected:
    private:
        ITimer *m_pBigAglTimer;
        ITimer *m_pOverTurnTimer;

        std::vector<stTimeType> m_vBigAglTime;
        std::vector<stTimeType> m_vOverTurnTime;
};

#endif // CANGLEANALYSER_H
