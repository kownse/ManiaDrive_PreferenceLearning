#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include "windows.h"
#include "WinNT.h"

///��ƫ��index�õ�ƫ���ַ���
void getPreferStr(int preference_index, char* szEmotion);

///����Ŀ¼
bool CreateDir(char *path);

///��ȡ�������
void GetPCName(TCHAR *result);

#endif // UTILITY_H_INCLUDED
