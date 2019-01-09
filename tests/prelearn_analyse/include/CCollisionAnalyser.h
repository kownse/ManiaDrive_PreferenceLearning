#ifndef CCOLLISIONANALYSER_H
#define CCOLLISIONANALYSER_H

#include "ITimerListener.h"
#include "IOriginalAnalyser.h"
#include "ITimer.h"
#include "../define.h"

#include <vector>

class CCollisionAnalyser : public IOriginalAnalyser, ITimerListener
{
    public:
        CCollisionAnalyser();
        ~CCollisionAnalyser();

        void DoAnalyse(c2s_driveinfo_attempt_original &original);
        void TimerDone(float fTime, int flag);
        void GetStatisticResult(CAnalyseTypeResult& result);
        void Reset(void);

        bool IsCollision();
        bool IsLeap();

    protected:
    private:
        ITimer *m_pColTimer;
        ITimer *m_pColLeapTimer;
        ITimer *m_pLeapTimer;
        ITimer *m_pColControlTimer;
        ITimer *m_pLeapControlTimer;

        std::vector<stTimeType> m_vColTime;
        std::vector<stTimeType> m_vColLeapTime;
        std::vector<stTimeType> m_vLeapTime;
        std::vector<stTimeType> m_vColControlTime;
        std::vector<stTimeType> m_vLeapControlTime;
};

#endif // CCOLLISIONANALYSER_H
