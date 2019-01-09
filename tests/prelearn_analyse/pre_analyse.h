#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

#include "include/COriAnalyserMgr.h"
#include "include/CTrackAnalyser.h"
#include "define.h"

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

class DLL_EXPORT COriAnalyserMgr;
class DLL_EXPORT CTrackAnalyser;
class DLL_EXPORT CTrackResult;

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
