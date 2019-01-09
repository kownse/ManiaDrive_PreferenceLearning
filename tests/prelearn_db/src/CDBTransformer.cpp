#include "../raydium/PreferenceModeling/globle.h"
#include "CDBTransformer.h"
#include <assert.h>
#include <vector>
#include <stdio.h>

CDBTransformer::CDBTransformer()
{
    //ctor
}

CDBTransformer::~CDBTransformer()
{
    //dtor
}

bool CDBTransformer::Transform()
{
    assert(m_pLoader && "Loader不能为空");
    assert(m_pSaver && "Saver不能为空");

    std::vector<c2s_driveinfo_attempt> attemptTmp;
    std::vector<c2s_driveinfo_level> levelTmp;
    std::vector<c2s_driveinfo_attempt_original> originalTmp;
    c2s_4afc_result preferTmp;
    c2s_user_info userTmp;
    session_info sessionTmp;

    int rc;
    do
    {
        rc = m_pLoader->load_session_data(attemptTmp, levelTmp, originalTmp, preferTmp, userTmp, sessionTmp);

        if(rc == PRE_DB_SESSION_END)
        {
            rc = m_pLoader->load_session_data(attemptTmp, levelTmp, originalTmp, preferTmp, userTmp, sessionTmp);
            continue;
        }
        else if(rc == PRE_DB_ERROR)
        {
            fprintf(stderr, "读取数据错误\n");
            return false;
        }

        m_pSaver->save_session_data(attemptTmp, levelTmp, originalTmp, preferTmp, userTmp, sessionTmp);

        attemptTmp.clear();
        levelTmp.clear();
        originalTmp.clear();

    }while(rc == PRE_DB_OK || rc == PRE_DB_SESSION_END);

    return true;
}
