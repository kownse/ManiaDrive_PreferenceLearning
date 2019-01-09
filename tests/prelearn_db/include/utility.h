#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include "windows.h"
#include "WinNT.h"

///由偏好index得到偏好字符串
void getPreferStr(int preference_index, char* szEmotion);

///创建目录
bool CreateDir(char *path);

///获取计算机名
void GetPCName(TCHAR *result);

#endif // UTILITY_H_INCLUDED
