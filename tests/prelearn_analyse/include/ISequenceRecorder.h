#ifndef ISEQUENCERECORDER_H
#define ISEQUENCERECORDER_H

#include <vector>

class ISequenceRecorder
{
    public:
        ISequenceRecorder();
        ISequenceRecorder(std::vector<float> *pVector);
        virtual ~ISequenceRecorder();

        virtual void DoRecord(float value);
        void SetVector(std::vector<float> *pVector)
        { m_pVector = pVector;}

    protected:
        std::vector<float> *m_pVector;
    private:
};

#endif // ISEQUENCERECORDER_H
