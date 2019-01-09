#include <iostream>
#include "pre_analyse/define.h"
#include "pre_analyse/include/CTrackAnalyser.h"
#include "pre_db/pre_db.h"
#include <string>
#include <io.h>
#include <stdio.h>
#include <string.h>

#define FACT            6
#define MOVE_X          -5
#define MOVE_Y          -3
#define MOVE_Z          -1

using namespace std;

CTrackAnalyser analyser;
CMySQLSaver mysaver;

void filesearch(string path,int layer)
{
    struct _finddata_t   filefind;
    string  curr=path+"\\*.mni";
    int   done=0,i,handle;
    if((handle=_findfirst(curr.c_str(),&filefind))==-1)
        return;

    if   ((_A_SUBDIR==filefind.attrib))
    {
        cout << ".mni不应该是目录" << endl;
        return;
    }
    else
    {
        string szFullPath = path + filefind.name;
        //cout<< szFullPath <<endl;
        if(analyser.DoAnalyse(szFullPath))
        {
            cout << szFullPath + "分析成功" << endl;
            CTrackResult result;
            analyser.GetStatisticResult(result);
            mysaver.db_save_trackresult_stmt(result);
            analyser.Reset();
        }
        else
        {
            cout << szFullPath + "分析失败" << endl;
        }
    }

    while(!(done=_findnext(handle,&filefind)))
    {
        //printf("测试的--%s\n",filefind.name);
        if(!strcmp(filefind.name,".."))
        {
            continue;
        }

        for(i=0; i<layer; i++)
            cout<<"     ";

        if   ((_A_SUBDIR==filefind.attrib))
        {
            /*
            printf("----------%s\n",filefind.name);
            cout<<filefind.name<<"(dir)"<<endl;
            curr=path+"\\"+filefind.name;
            filesearch(curr,layer+1);
            */
            cout << ".mni不应该是目录" << endl;
            return;
        }
        else
        {
            string szFullPath = path + filefind.name;
            //cout<< szFullPath <<endl;
            if(analyser.DoAnalyse(szFullPath))
            {
                cout << szFullPath + "分析成功" << endl;
                CTrackResult result;
                memset(&result, 0, sizeof(CTrackResult));
                analyser.GetStatisticResult(result);
                mysaver.db_save_trackresult_stmt(result);
                analyser.Reset();
            }
            else
            {
                cout << szFullPath + "分析失败" << endl;
            }
        }
    }
    _findclose(handle);
}

int   main()
{
    string   path("./");
    filesearch(path,0);
    return 0;
}

