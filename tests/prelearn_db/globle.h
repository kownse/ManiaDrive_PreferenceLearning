#ifndef _PRE_DB_GLOBLE_H_INCLUDED
#define _PRE_DB_GLOBLE_H_INCLUDED

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

const int PRE_DB_OK = 0;
const int PRE_DB_ERROR = 1;
const int PRE_DB_ROWEND = 2;
const int PRE_DB_SESSION_END = 3;
const int PRE_DB_USER_END = 4;

#define PREFERENCE_ANXIETY          0
#define PREFERENCE_PLEASURE         1
#define PREFERENCE_BORING           2
#define PREFERENCE_EXCITMENT        3
#define PREFERENCE_FRUSTRATION      4
#define PREFERENCE_CHANLENGE        5
#define PREFERENCE_MAX              5

#endif // GLOBLE_H_INCLUDED
