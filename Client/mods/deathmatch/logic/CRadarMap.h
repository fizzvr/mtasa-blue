/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRadarMap.h
 *  PURPOSE:     Header for radar map class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientCommon.h"
#include <CClientManager.h>
#include <CClientTextDisplay.h>
#include <gui/CGUI.h>

class CRadarMap
{
public:
    CRadarMap(class CClientManager* pManager);
    virtual ~CRadarMap(void);

    void DoPulse(void);
    void DoRender(void);

    bool IsRadarShowing(void);

    bool GetRadarEnabled(void) const { return m_bIsRadarEnabled; };
    void SetRadarEnabled(bool bIsRadarEnabled);

    bool GetForcedState(void) const { return m_bForcedState; }
    void SetForcedState(bool bState);

    bool GetBoundingBox(CVector& vecMin, CVector& vecMax);

    void ToggleHelpText(void);

protected:
    void InternalSetRadarEnabled(bool bEnabled);

    void          CreateMarkerTextures(void);
    CTextureItem* GetMarkerTexture(CClientRadarMarker* pMarker, float fLocalZ, float* pfScale, SColor* pColor);

public:
    bool IsAttachedToLocalPlayer(void) const { return m_bIsAttachedToLocal; };
    void SetAttachedToLocalPlayer(bool bIsAttachedToLocal);

    bool IsMovingNorth(void) const { return m_bIsMovingNorth; };
    void SetMovingNorth(bool bIsMovingNorth) { m_bIsMovingNorth = bIsMovingNorth; };

    bool IsMovingSouth(void) const { return m_bIsMovingSouth; };
    void SetMovingSouth(bool bIsMovingSouth) { m_bIsMovingSouth = bIsMovingSouth; };

    bool IsMovingEast(void) const { return m_bIsMovingEast; };
    void SetMovingEast(bool bIsMovingEast) { m_bIsMovingEast = bIsMovingEast; };

    bool IsMovingWest(void) const { return m_bIsMovingWest; };
    void SetMovingWest(bool bIsMovingWest) { m_bIsMovingWest = bIsMovingWest; };

    void ZoomIn(void);
    void ZoomOut(void);

    SString GetBoundKeyName(const SString& strCommand);

private:
    bool CalculateEntityOnScreenPosition(class CClientEntity* pEntity, CVector2D& vecLocalPos);
    bool CalculateEntityOnScreenPosition(CVector vecPosition, CVector2D& vecLocalPos);
    void SetupMapVariables(void);

    void MoveNorth(void);
    void MoveSouth(void);
    void MoveEast(void);
    void MoveWest(void);

private:
    class CClientManager*            m_pManager;
    class CClientRadarMarkerManager* m_pRadarMarkerManager;
    class CClientRadarAreaManager*   m_pRadarAreaManager;

    CTextureItem*              m_pRadarImage;
    CTextureItem*              m_pLocalPlayerBlip;
    std::vector<CTextureItem*> m_MarkerTextureList;

    unsigned int m_uiHeight;
    unsigned int m_uiWidth;

    float m_fMapSize;
    int   m_iMapMinX;
    int   m_iMapMaxX;
    int   m_iMapMinY;
    int   m_iMapMaxY;

    int m_iHorizontalMovement;
    int m_iVerticalMovement;

    float m_fZoom;

    bool m_bIsRadarEnabled;
    bool m_bForcedState;
    bool m_bIsAttachedToLocal;

    bool m_bIsMovingNorth;
    bool m_bIsMovingSouth;
    bool m_bIsMovingEast;
    bool m_bIsMovingWest;

    unsigned long m_ulUpdateTime;

    std::vector<CClientTextDisplay*> m_HelpTextList;
    bool                             m_bHideHelpText;

    bool m_bHudVisible;
    bool m_bChatVisible;
    bool m_bRadarVisible;
    bool m_bDebugVisible;
    bool m_bTextVisible;
};
