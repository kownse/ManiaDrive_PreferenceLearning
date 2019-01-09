#ifndef IORIGINALANALYSER_H
#define IORIGINALANALYSER_H

#include "../../raydium/PreferenceModeling/globle.h"
#include "../func.h"
#include "../define.h"
#include <vector>

class IOriginalAnalyser
{
    public:
        IOriginalAnalyser();
        virtual ~IOriginalAnalyser();

        virtual void DoAnalyse(c2s_driveinfo_attempt_original &original) = 0;
        virtual void GetStatisticResult(CAnalyseTypeResult& result) = 0;
        virtual void Reset(void) = 0;

        void SetLog(bool flag){ m_bLog = flag; }
        //void track_type_statictics(std::vector<stTimeType>& vecValueTime, CAnalyseTypeResult& result);      ///带赛道类型的分析
    protected:
        int m_nLastTrackType;
        bool m_bLog;
    private:
};

#endif // IORIGINALANALYSER_H
