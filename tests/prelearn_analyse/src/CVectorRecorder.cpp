#include "../include/CVectorRecorder.h"
CVectorRecorder::CVectorRecorder()
{
    //ctor
}

CVectorRecorder::CVectorRecorder(std::vector<float> *pVector)
    :m_pVector(pVector)
{

}

CVectorRecorder::~CVectorRecorder()
{
    //dtor
}

void CVectorRecorder::DoRecord(float value)
{
    if(m_pVector)
        m_pVector->push_back(value);
}
