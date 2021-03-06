/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSingularFileDownload.h
 *  PURPOSE:     Header for singular file download
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifndef _WINDOWS_

#define WIN32_LEAN_AND_MEAN     // Exclude all uncommon functions from windows.h to reduce build time by 100ms

#include <windows.h>
#endif

#include <bochs_internal/crc32.h>
#include "CChecksum.h"

class CSingularFileDownload
{
public:
    CSingularFileDownload(CResource* pResource, const char* szName, const char* szNameShort, SString strHTTPURL, CChecksum serverChecksum);
    ~CSingularFileDownload(void);

    static void DownloadFinishedCallBack(const SHttpDownloadResult& result);

    bool DoesClientAndServerChecksumMatch(void);

    const char* GetName(void) { return m_strName; };
    const char* GetShortName(void) { return m_strNameShort; };

    CResource* GetResource(void) { return m_pResource; };

    void SetComplete(void) { m_bComplete = true; };
    bool GetComplete(void) { return m_bComplete; };

    void CallFinished(bool bSuccess);
    void Cancel(void);

    CChecksum GenerateClientChecksum(void);

protected:
    SString m_strName;
    SString m_strNameShort;

    CResource* m_pResource;

    bool m_bComplete;
    bool m_bBeingDeleted;

    CChecksum m_LastClientChecksum;
    CChecksum m_ServerChecksum;
};
