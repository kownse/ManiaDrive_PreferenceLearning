#ifndef IDBSAVER_H
#define IDBSAVER_H

#include "../globle.h"
#include "../raydium/PreferenceModeling/globle.h"
#include <vector>

class DLL_EXPORT IDBSaver
{
    public:
        IDBSaver();
        virtual ~IDBSaver();

    DLL_EXPORT virtual int save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                        std::vector<c2s_driveinfo_level> &levelStore,
                        std::vector<c2s_driveinfo_attempt_original> &originalStore,
                        c2s_4afc_result &prefer,
                        c2s_user_info &user) = 0;

    DLL_EXPORT virtual int save_session_data(std::vector<c2s_driveinfo_attempt> &attemptStore,
                        std::vector<c2s_driveinfo_level> &levelStore,
                        std::vector<c2s_driveinfo_attempt_original> &originalStore,
                        c2s_4afc_result &prefer,
                        c2s_user_info &user,
                        session_info &session) = 0;
};

#endif // IDBSAVER_H
