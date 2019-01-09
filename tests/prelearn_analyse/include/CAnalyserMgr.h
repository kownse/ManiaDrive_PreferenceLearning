#ifndef CANALYSERMGR_H_INCLUDED
#define CANALYSERMGR_H_INCLUDED

#include <vector>
#include "../define.h"

template <typename T>
class CAnalyserMgr
{
    public:
        CAnalyserMgr(){}
        virtual ~CAnalyserMgr()
        {
            typename std::vector<T*>::iterator it;
            for(it = m_vAnalyser.begin(); it != m_vAnalyser.end(); ++it)
            {
                T* pAnalyser = *it;
                if(pAnalyser)
                    delete pAnalyser;
            }
            m_vAnalyser.clear();
        }

        //void DoAnalyse(c2s_driveinfo_attempt_original &original);
        virtual void Reset(void)
        {
            typename std::vector<T*>::iterator it;
            for(it = m_vAnalyser.begin(); it != m_vAnalyser.end(); ++it)
            {
                T* pAnalyser = *it;
                if(pAnalyser)
                    pAnalyser->Reset();
            }
        }

        virtual void AddAnalyser(T* pAnalyser)
        {
            m_vAnalyser.push_back(pAnalyser);
        }

        //void GetStaticResult(CAnalyseTypeResult &result);
    protected:
        std::vector<T*> m_vAnalyser;
};

template <typename T, typename D>
class CDataAnalyserMgr : public CAnalyserMgr<T>
{
    public:
        CDataAnalyserMgr(){}
        virtual ~CDataAnalyserMgr(){}

        virtual void DoAnalyse(D &data)
        {
            typename std::vector<T*>::iterator it;
            for(it = this->m_vAnalyser.begin(); it != this->m_vAnalyser.end(); ++it)
            {
                T* pAnalyser = *it;
                if(pAnalyser)
                    pAnalyser->DoAnalyse(data);
            }
        }
};

template <typename T, typename D, typename R>
class CResultAnalyserMgr : public CDataAnalyserMgr<T, D>
{
    public:
        CResultAnalyserMgr(){}
        virtual ~CResultAnalyserMgr(){}

        virtual void GetStaticResult(R &result)
        {
            typename std::vector<T*>::iterator it;
            for(it = this->m_vAnalyser.begin(); it != this->m_vAnalyser.end(); ++it)
            {
                T* pAnalyser = *it;
                if(pAnalyser)
                    pAnalyser->GetStatisticResult(result);
            }
        }
};

#endif // CANALYSERMGR_H_INCLUDED
