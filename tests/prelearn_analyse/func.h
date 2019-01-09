#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED

#include <vector>
#include "define.h"

//具体处理收集结果的函数指针
typedef void (*f_tw_col_done)(float tTotal);

///收集允许有中断但满足一定时间窗口的数据
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

///收集不能中断只能持续的数据
void col_con_info(int &bMark,
                  float &tPile,
                  float step,
                  const float tThredhold,
                  unsigned char bResetFlag,
                  f_con_col_done fcd);


stStaticResult basic_statistics(std::vector<float>& vecValues);         ///直接分析
void track_type_statictics(const int nAnalyseType, std::vector<stTimeType>& vecValueTime, CAnalyseTypeResult& result);      ///带赛道类型的分析
void track_statictics(int nTrackType, std::vector<float>& vecValue, CTrackResult& result);      ///赛道分析

bool IsTrackType(int type);

#endif // FUNC_H_INCLUDED
