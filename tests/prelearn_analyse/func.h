#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED

#include <vector>
#include "define.h"

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
                );

typedef void (*f_con_col_done)(float tPile);

///�ռ������ж�ֻ�ܳ���������
void col_con_info(int &bMark,
                  float &tPile,
                  float step,
                  const float tThredhold,
                  unsigned char bResetFlag,
                  f_con_col_done fcd);


stStaticResult basic_statistics(std::vector<float>& vecValues);         ///ֱ�ӷ���
void track_type_statictics(const int nAnalyseType, std::vector<stTimeType>& vecValueTime, CAnalyseTypeResult& result);      ///���������͵ķ���
void track_statictics(int nTrackType, std::vector<float>& vecValue, CTrackResult& result);      ///��������

bool IsTrackType(int type);

#endif // FUNC_H_INCLUDED
