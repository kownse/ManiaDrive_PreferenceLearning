#ifndef DATAANALYSE_H_INCLUDED
#define DATAANALYSE_H_INCLUDED

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

#endif // DATAANALYSE_H_INCLUDED
