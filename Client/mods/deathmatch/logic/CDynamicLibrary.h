/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDynamicLibrary.h
 *  PURPOSE:     Header for dynamic library class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

class CDynamicLibrary
{
public:
    CDynamicLibrary(void);
    ~CDynamicLibrary(void);

    bool Load(const char* szFilename);
    void Unload(void);
    bool IsLoaded(void);

    void* GetProcedureAddress(const char* szProcName);

private:
    #ifdef WIN32
    HMODULE m_hModule;
    #else
    void* m_hModule;
    #endif
};
