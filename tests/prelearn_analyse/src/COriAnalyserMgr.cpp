#include "../include/COriAnalyserMgr.h"
#include "../include/CCollisionAnalyser.h"
#include "../include/CUserInputAnalyser.h"
#include "../include/CAngleAnalyser.h"
#include "../include/CBasicAnalyser.h"
#include "../func.h"

COriAnalyserMgr::COriAnalyserMgr()
{
    //ctor
    IOriginalAnalyser *pCollision = new CCollisionAnalyser();
    pCollision->SetLog(true);
    this->AddAnalyser(pCollision);

    IOriginalAnalyser *pUserInput = new CUserInputAnalyser();
    pUserInput->SetLog(true);
    this->AddAnalyser(pUserInput);

    this->AddAnalyser(new CAngleAnalyser());
    this->AddAnalyser(new CBasicAnalyser());
}


COriAnalyserMgr::~COriAnalyserMgr()
{
    //dtor
    std::vector<IOriginalAnalyser*>::iterator it;
    for(it = m_vAnalyser.begin(); it != m_vAnalyser.end(); ++it)
    {
        IOriginalAnalyser* pAnalyser = *it;
        if(pAnalyser)
            delete pAnalyser;
    }
    m_vAnalyser.clear();
}


void COriAnalyserMgr::DoAnalyse(c2s_driveinfo_attempt_original &original)
{
    //CDataAnalyserMgr<IOriginalAnalyser, c2s_driveinfo_attempt_original>::DoAnalyse(original);

    std::vector<IOriginalAnalyser*>::iterator it;
    for(it = m_vAnalyser.begin(); it != m_vAnalyser.end(); ++it)
    {
        IOriginalAnalyser* pAnalyser = *it;
        if(pAnalyser)
            pAnalyser->DoAnalyse(original);
    }
}
void COriAnalyserMgr::Reset(void)
{
    //CAnalyserMgr<IOriginalAnalyser>::Reset();

    std::vector<IOriginalAnalyser*>::iterator it;
    for(it = m_vAnalyser.begin(); it != m_vAnalyser.end(); ++it)
    {
        IOriginalAnalyser* pAnalyser = *it;
        if(pAnalyser)
            pAnalyser->Reset();
    }
}

void COriAnalyserMgr::AddAnalyser(IOriginalAnalyser* pAnalyser)
{
    this->m_vAnalyser.push_back(pAnalyser);
}

void COriAnalyserMgr::GetStaticResult(CAnalyseTypeResult &result)
{
    //CResultAnalyserMgr<IOriginalAnalyser, c2s_driveinfo_attempt_original, CAnalyseTypeResult>::GetStaticResult(result);

    std::vector<IOriginalAnalyser*>::iterator it;
    for(it = m_vAnalyser.begin(); it != m_vAnalyser.end(); ++it)
    {
        IOriginalAnalyser* pAnalyser = *it;
        if(pAnalyser)
            pAnalyser->GetStatisticResult(result);
    }
}

