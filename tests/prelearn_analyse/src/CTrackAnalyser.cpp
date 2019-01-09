#include "../include/CTrackAnalyser.h"
#include "../func.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FACT            6
#define MOVE_X          -5
#define MOVE_Y          -3
#define MOVE_Z          -1

char tdata[4096];

void extract_data(char *from, char *to_name, char *to_author, char *to_gold, char *to_author_time, char *message_file, char *ent_file);

CTrackAnalyser::CTrackAnalyser()
    :m_nLastType(0),m_nCount(0),m_nTotal(0)
{
    //ctor
}

CTrackAnalyser::~CTrackAnalyser()
{
    //dtor
}

bool CTrackAnalyser::DoAnalyse(std::string &szFilename)
{
    FILE *in;
    char c;
    char str[255];
    float x,y,z;
    float vx,vy,vz;
    float rvx,rvy,rvz;
    float nx,ny,nz;
    float rnx,rny,rnz;
    float tx,ty,tz;
    int rot,flags,type;
    int obj;
    int i;

    in=fopen(szFilename.c_str(),"rt");
    if (!in)
    {
        printf("mni_gen: unable to open '%s'",szFilename.c_str());
        return false;
    }

    m_szName = szFilename.substr(2);

    while (1)
    {
        c=fgetc(in);
        fgetc(in);

        if (c==EOF)
            break;

        switch (c)
        {
        case 'b':
            {
                fscanf(in,"%f %f %f %f %f %f %i\n", &x,
                   &y,
                   &z,
                   &tx,
                   &ty,
                   &tz,
                   &type);
                x=x*FACT+MOVE_X;
                y=y*FACT+MOVE_Y;
                z=z*FACT+MOVE_Z;
                tx=tx*FACT;
                ty=ty*FACT;
                tz=tz*FACT;
                stTrackBlock block(x,y,z,tx,ty,tz,type);
                AnalyseBlock(block);
                break;
            }
        case 'g':
            fscanf(in,"%f %f %f %i %i %s\n",&x,
                   &y,
                   &z,
                   &rot,
                   &flags,
                   str);
            break;
        case 'd':
            fgets(tdata,4000,in);
            if (tdata[strlen(tdata)-1]=='\n')
                tdata[strlen(tdata)-1]=0;

            char d[6][512];
            extract_data(tdata,d[0],d[1],d[2],d[3],d[4],d[5]);
            m_fGoldTime = atof( d[2] );
        }

    }
    Finish();

    printf("·ÖÎö½áÊø");
    fclose(in);

    return true;
}

void CTrackAnalyser::AnalyseBlock(stTrackBlock &block)
{
    int type = block.type;

    if(m_nLastType != type )
    {
        if(IsTrackType(m_nLastType))
            switch(m_nLastType)
            {
            case TRACK_TYPE_STRAIGHT:
                m_vStraight.push_back(m_nCount);
                break;
            case TRACK_TYPE_CROOK:
                m_vCrook.push_back(m_nCount);
                break;
            case TRACK_TYPE_BUMP:
                m_vBump.push_back(m_nCount);
                break;
            case TRACK_TYPE_UPWARD:
                m_vUp.push_back(m_nCount);
                break;
            case TRACK_TYPE_DOWNWARD:
                m_vDown.push_back(m_nCount);
                break;
            case TRACK_TYPE_SPRING:
                m_vSpring.push_back(m_nCount);
                break;
            }
        m_nTotal += m_nCount;
        m_nCount = 1;

        if(IsTrackType(type))
            m_nLastType = type;
    }
    else
    {
        ++m_nCount;
    }
}

void CTrackAnalyser::Finish()
{
    if(IsTrackType(m_nLastType))
        switch(m_nLastType)
            {
            case TRACK_TYPE_STRAIGHT:
                m_vStraight.push_back(m_nCount);
                break;
            case TRACK_TYPE_CROOK:
                m_vCrook.push_back(m_nCount);
                break;
            case TRACK_TYPE_BUMP:
                m_vBump.push_back(m_nCount);
                break;
            case TRACK_TYPE_UPWARD:
                m_vUp.push_back(m_nCount);
                break;
            case TRACK_TYPE_DOWNWARD:
                m_vDown.push_back(m_nCount);
                break;
            case TRACK_TYPE_SPRING:
                m_vSpring.push_back(m_nCount);
                break;
            }
    m_nTotal += m_nCount;
    m_nCount = 0;
}

void CTrackAnalyser::Reset()
{
    m_nCount = 0;
    m_nTotal = 0;
    m_nLastType = 0;
    m_vStraight.clear();
    m_vCrook.clear();
    m_vBump.clear();
    m_vUp.clear();
    m_vDown.clear();
    m_vSpring.clear();
}

void CTrackAnalyser::GetStatisticResult(CTrackResult &result)
{
    result.fTime = m_fGoldTime;
    result.nTotal = m_nTotal;
    strcpy(result.szName, m_szName.c_str());
    result.nNameLength = strlen(result.szName);

    track_statictics(TRACK_TYPE_STRAIGHT, m_vStraight, result);
    track_statictics(TRACK_TYPE_CROOK, m_vCrook, result);
    track_statictics(TRACK_TYPE_BUMP, m_vBump, result);
    track_statictics(TRACK_TYPE_UPWARD, m_vUp, result);
    track_statictics(TRACK_TYPE_DOWNWARD, m_vDown, result);
    track_statictics(TRACK_TYPE_SPRING, m_vSpring, result);
}

void extract_data(char *from, char *to_name, char *to_author, char *to_gold, char *to_author_time, char *message_file, char *ent_file)
{
    char d[4096];
    int i,start,cpt;

    message_file[0]=0;
    ent_file[0]=0;

    strcpy(d,from);
    cpt=0;
    start=0;
    for (i=0;i<=strlen(from);i++)
    {
        if (d[i]==';' || d[i]==0 || d[i]=='\r')
        {
            d[i]=0;
            //printf("%i %s\n",cpt,d+start);
            if (cpt==0)
                strcpy(to_name,d+start);
            if (cpt==1)
                strcpy(to_author,d+start);
            if (cpt==2)
                strcpy(to_gold,d+start);
            if (cpt==3)
                strcpy(to_author_time,d+start);
            if (cpt==4)
                strcpy(message_file,d+start);
            if (cpt==5)
                strcpy(ent_file,d+start);

            // finalize
            cpt++;
            start=i+1;
        }
    }
}
