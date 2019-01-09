#define _WIN32_WINNT 0x0500

#include "globle.h"
#include "utility.h"
#include <cstring>

//
void getPreferStr(int preference_index, char* szEmotion)
{
    switch(preference_index)
    {
    case PREFERENCE_ANXIETY:
        strcpy(szEmotion, "\271 \272");
        break;
    case PREFERENCE_SATISFACTION:
        strcpy(szEmotion, "\273 \274");
        break;
    case PREFERENCE_BORING:
        strcpy(szEmotion, "\275 \276");
        break;
    case PREFERENCE_EXCITMENT:
        strcpy(szEmotion, "\251 \252");
        break;
    case PREFERENCE_FRUSTRATION:
        strcpy(szEmotion, "\253 \254");
        break;
    case PREFERENCE_CHANLENGE:
        strcpy(szEmotion, "\255 \256");
        break;
    case PREFERENCE_FUN:
        strcpy(szEmotion, "\30 \31");
        break;
    }
}

bool CreateDir(char *path)
{
    if(::CreateDirectory(path, NULL) == 0)
    {
        //创建失败,判断失败原因是不是文件夹已经存在
        DWORD nErrorNo = GetLastError();
        if(nErrorNo != ERROR_ALREADY_EXISTS)
        {
            return false;
        }
    }
    return true;
}

void GetPCName(TCHAR *result)
{
    //得到计算机名
    TCHAR buffer[256] = TEXT("");
    int cnf = 0;
    DWORD dwSize = sizeof(buffer);
    //依次取得能取得所有计算机标示符
    for (cnf = 0; cnf < ComputerNameMax; cnf++)
    {
        if (!GetComputerNameExA((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
        {
            continue;
        }
        else
        {
            strcpy(result, buffer);
            break;
        }
    }
}
