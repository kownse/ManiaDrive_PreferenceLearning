#include "PreferenceModeling/DataCollect/DataAnalyse.h"

//���崦���ռ�����ĺ���ָ��
typedef void (*f_tw_col_done)(float tTotal);

///�ռ��������жϵ�����һ��ʱ�䴰�ڵ�����
void col_tw_info(int &bMark,
                 unsigned char& bFlag,
                 float &tStart,
                 float &tEnd,
                 float &tPile,
                 float fTime,
                 float step,
                 const float tIntervalThread,
                 unsigned char bClsFlag,
                 f_tw_col_done fcd
                )
{
//   col_pile(bMark, bFlag, tStart, tEnd, tPile, fTime, step);
    if(bMark)
    {
        if(bFlag)
        {
            tPile = 0.0f;
        }
        bFlag = 1;

        if(tStart < 0.0001f)
        {
            tStart = fTime;
            tEnd = fTime + step;
        }
        else
        {
            tEnd = fTime;
        }
    }

    if(bClsFlag)
        bMark = 0;

    if(bFlag)
    {
        //����ֵ��Χ���ۼ�
        if(tPile < tIntervalThread)
        {
            tPile += step;
        }
        else
        {
            tPile = 0.0f;
            bFlag = 0;

            float tTotal = tEnd - tStart;
            tStart = tEnd = 0.0f;
            //����ֵ��Χ��ر��ۼ�
            fcd(tTotal);
        }

    }
}

typedef void (*f_con_col_done)(float tPile);

///�ռ������ж�ֻ�ܳ���������
void col_con_info(int &bMark,
                  float &tPile,
                  float step,
                  const float tThredhold,
                  unsigned char bResetFlag,
                  f_con_col_done fcd)
{
    if(bMark)
    {
        tPile += step;
    }
    else
    {
        if(tPile > tThredhold)
        {
            fcd(tPile);
        }
        tPile = 0.0f;
    }
    if(bResetFlag)
    {
        bMark = 1;
    }
}
