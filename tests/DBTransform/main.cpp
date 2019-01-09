#include <iostream>
#include "pre_db/pre_db.h"
#include "pre_db/include/CDBTransformer.h"
#include "pre_db/include/CSQLiteLoader.h"
#include<string>
#include<io.h>
#include<stdio.h>

using namespace std;

CDBTransformer transformer;
CMySQLSaver mysaver;

bool transform(string &szFilename);

void filesearch(string path,int layer)
{
    struct _finddata_t   filefind;
    string  curr=path+"\\*.data";
    int   done=0,i,handle;
    if((handle=_findfirst(curr.c_str(),&filefind))==-1)
        return;

    if   ((_A_SUBDIR==filefind.attrib))
    {
        cout << ".data不应该是目录" << endl;
        return;
    }
    else
    {
        string szFullPath = path + filefind.name;
        //cout<< szFullPath <<endl;
        if(transform(szFullPath))
        {
            cout << szFullPath + "转换成功" << endl;
        }
        else
        {
            cout << szFullPath + "转换失败" << endl;
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
            cout << ".data不应该是目录" << endl;
            return;
        }
        else
        {
            string szFullPath = path + filefind.name;
            //cout<< szFullPath <<endl;
            if(transform(szFullPath))
            {
                cout << szFullPath + "转换成功" << endl;
            }
            else
            {
                cout << szFullPath + "转换失败" << endl;
            }
        }
    }
    _findclose(handle);
}

int   main()
{
    string   path("./original/");
    filesearch(path,0);
    system("PAUSE");
    return 0;
}


bool transform(string &szFilename)
{
    //CSQLiteLoader sqloader("LENOVO-1A5B3F84.data");
    CSQLiteLoader sqloader(szFilename.c_str());

    transformer.SetLoader(&sqloader);
    transformer.SetSaver(&mysaver);

    return transformer.Transform();
}

