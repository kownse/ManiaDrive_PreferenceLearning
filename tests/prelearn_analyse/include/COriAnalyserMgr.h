#ifndef CORIANALYSERMGR_H
#define CORIANALYSERMGR_H

#include <vector>
#include "IOriginalAnalyser.h"
#include "../define.h"
#include "CAnalyserMgr.h"

class DLL_EXPORT COriAnalyserMgr //: public CResultAnalyserMgr<IOriginalAnalyser, c2s_driveinfo_attempt_original, CAnalyseTypeResult>
{
    public:
        COriAnalyserMgr();
        virtual ~COriAnalyserMgr();


        virtual void DoAnalyse(c2s_driveinfo_attempt_original &original);
        virtual void Reset(void);

        virtual void AddAnalyser(IOriginalAnalyser* pAnalyser);

        virtual void GetStaticResult(CAnalyseTypeResult &result);
    protected:
    private:
        std::vector<IOriginalAnalyser*> m_vAnalyser;

};

#endif // CORIANALYSERMGR_H
