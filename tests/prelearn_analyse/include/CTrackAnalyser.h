#ifndef CTRACKANALYSER_H
#define CTRACKANALYSER_H

#include <vector>
#include "../define.h"
#include <string>

class DLL_EXPORT CTrackAnalyser
{
    public:
        CTrackAnalyser();
        ~CTrackAnalyser();

        bool DoAnalyse(std::string &file);
        void GetStatisticResult(CTrackResult &result);
        void Reset();
    protected:
    private:
        void AnalyseBlock(stTrackBlock &block);
        void Finish();

        std::string m_szName;
        float m_fGoldTime;
        int m_nLastType;
        int m_nCount;
        int m_nTotal;

        std::vector<float> m_vStraight;
        std::vector<float> m_vCrook;
        std::vector<float> m_vBump;
        std::vector<float> m_vUp;
        std::vector<float> m_vDown;
        std::vector<float> m_vSpring;
};

#endif // CTRACKANALYSER_H
