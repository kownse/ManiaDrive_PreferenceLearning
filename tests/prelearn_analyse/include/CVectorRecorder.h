#ifndef ISEQUENCERECORDER_H
#define ISEQUENCERECORDER_H

#include <vector>
#include "IRecorder.h"

class CVectorRecorder : public IRecorder
{
    public:
        CVectorRecorder();
        CVectorRecorder(std::vector<float> *pVector);
        virtual ~CVectorRecorder();

        void DoRecord(float value);
        void SetVector(std::vector<float> *pVector)
        { m_pVector = pVector;}

    protected:
        std::vector<float> *m_pVector;
    private:
};

#endif // ISEQUENCERECORDER_H
