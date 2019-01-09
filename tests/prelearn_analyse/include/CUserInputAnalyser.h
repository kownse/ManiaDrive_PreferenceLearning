#ifndef CUSERINPUTANALYSER_H
#define CUSERINPUTANALYSER_H

#include "ITimerListener.h"
#include "IOriginalAnalyser.h"
#include "ITimer.h"
#include "../define.h"

#include <vector>

class CUserInputAnalyser : public IOriginalAnalyser, ITimerListener
{
    public:
        CUserInputAnalyser();
        ~CUserInputAnalyser();

        void DoAnalyse(c2s_driveinfo_attempt_original &original);
        void TimerDone(float fTime, int flag);
        void GetStatisticResult(CAnalyseTypeResult& result);
        void Reset(void);

    protected:
    private:
        ITimer *m_pForwardTimer;
        ITimer *m_pBackTimer;
        ITimer *m_pLeftTimer;
        ITimer *m_pRightTimer;
        ITimer *m_pForwardLeftTimer;
        ITimer *m_pForwardRightTimer;

        std::vector<stTimeType> m_vForwardTime;
        std::vector<stTimeType> m_vBackTime;
        std::vector<stTimeType> m_vLeftTime;
        std::vector<stTimeType> m_vRightTime;
        std::vector<stTimeType> m_vForwardLeftTime;
        std::vector<stTimeType> m_vForwardRightTime;


};

#endif // CUSERINPUTANALYSER_H
