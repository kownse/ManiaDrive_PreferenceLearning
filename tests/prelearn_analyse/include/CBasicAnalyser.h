#ifndef CBASICANALYSER_H
#define CBASICANALYSER_H

#include "IOriginalAnalyser.h"
#include "../define.h"

#include <vector>

class CBasicAnalyser  : public IOriginalAnalyser
{
    public:
        CBasicAnalyser();
        ~CBasicAnalyser();

        void DoAnalyse(c2s_driveinfo_attempt_original &original);
        void GetStatisticResult(CAnalyseTypeResult& result);
        void Reset(void);

    protected:
    private:
        std::vector<stTimeType> m_vSpeed;
        std::vector<stTimeType> m_vAglSpeed;
        std::vector<stTimeType> m_vRay;
};

#endif // CBASICANALYSER_H
