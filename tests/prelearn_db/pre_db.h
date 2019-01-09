#ifndef PRE_DB_H_INCLUDED
#define PRE_DB_H_INCLUDED

#include <windows.h>
#include "globle.h"
#include "include/MySQLSaver.h"
#include "include/SQLiteSaver.h"

/*  To use this exported function of dll, include this header
 *  in your project.
 */


#ifdef __cplusplus
extern "C"
{
#endif

//void DLL_EXPORT SomeFunction(const LPCSTR sometext);
class DLL_EXPORT CMySQLSaver;
class DLL_EXPORT CSqlite3Saver;

#ifdef __cplusplus
}
#endif

#endif // PRE_DB_H_INCLUDED
