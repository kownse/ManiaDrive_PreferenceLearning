#include "../include/ISequenceRecorder.h"
ISequenceRecorder::ISequenceRecorder()
{
    //ctor
}

ISequenceRecorder::ISequenceRecorder(std::vector<float> *pVector)
    :m_pVector(pVector)
{

}

ISequenceRecorder::~ISequenceRecorder()
{
    //dtor
}

void ISequenceRecorder::DoRecord(float value)
{
    if(m_pVector)
        m_pVector->push_back(value);
}
