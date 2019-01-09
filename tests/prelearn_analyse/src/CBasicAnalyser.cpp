#include "../include/CBasicAnalyser.h"
#include <math.h>

CBasicAnalyser::CBasicAnalyser()
{
    //ctor
}

CBasicAnalyser::~CBasicAnalyser()
{
    //dtor
}

void CBasicAnalyser::DoAnalyse(c2s_driveinfo_attempt_original &original)
{
    if(IsTrackType(original.track_type))
    {
        m_nLastTrackType = original.track_type;
    }

    float fSpeed = sqrt(original.pLinearSpeed[0]*original.pLinearSpeed[0] +
                        original.pLinearSpeed[1]*original.pLinearSpeed[1] +
                        original.pLinearSpeed[2]*original.pLinearSpeed[2]);
    float fAdjustSpeed = fSpeed * 10;       //让线性速度和motor速度在一个数量级上
    m_vSpeed.push_back(stTimeType(fAdjustSpeed, m_nLastTrackType));

    float fAngularSpeed = sqrt(original.pAngSpeed[0]*original.pAngSpeed[0] +
                               original.pAngSpeed[1]*original.pAngSpeed[1] +
                               original.pAngSpeed[2]*original.pAngSpeed[2]);
    m_vAglSpeed.push_back(stTimeType(fAngularSpeed, m_nLastTrackType));

    float fRay = original.ray_l + original.ray_r;
    m_vRay.push_back(stTimeType(fRay, m_nLastTrackType));
}

void CBasicAnalyser::Reset(void)
{
    m_vSpeed.clear();
    m_vAglSpeed.clear();
    m_vRay.clear();
}

void CBasicAnalyser::GetStatisticResult(CAnalyseTypeResult& result)
{
    if(m_vSpeed.size()>0)
    {
        track_type_statictics(ANALYSE_TYPE_SPEED, m_vSpeed, result);
    }

    if(m_vAglSpeed.size()>0)
    {
        track_type_statictics(ANALYSE_TYPE_AGLSPEED, m_vAglSpeed, result);
    }

    if(m_vRay.size()>0)
    {
        track_type_statictics(ANALYSE_TYPE_RAY, m_vRay, result);
    }
}
