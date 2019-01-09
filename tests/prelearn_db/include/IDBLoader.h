#ifndef IDBLOADER_H
#define IDBLOADER_H

#include "../globle.h"
#include "../raydium/PreferenceModeling/globle.h"
#include <vector>
#include <string>

class DLL_EXPORT IDBLoader
{
public:
    IDBLoader();
    virtual ~IDBLoader();

    DLL_EXPORT virtual int load_session_data (std::vector<c2s_driveinfo_attempt> &attemptStore,
            std::vector<c2s_driveinfo_level> &levelStore,
            std::vector<c2s_driveinfo_attempt_original> &originalStore,
            c2s_4afc_result &prefer,
            c2s_user_info &user,
            session_info &session) = 0;
};

#endif // IDBLOADER_H
