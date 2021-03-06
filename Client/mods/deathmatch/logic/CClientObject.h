/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientObject.h
 *  PURPOSE:     Physical object entity class
 *
 *****************************************************************************/

class CClientObject;

#pragma once

#include "CClientStreamElement.h"

struct SLastSyncedObjectData
{
    CVector vecPosition;
    CVector vecRotation;
    float   fHealth;
};

class CClientObject : public CClientStreamElement
{
    DECLARE_CLASS(CClientObject, CClientStreamElement)
    friend class CClientObjectManager;
    friend class CClientPed;

public:
    CClientObject(class CClientManager* pManager, ElementID ID, unsigned short usModel, bool bLowLod);
    ~CClientObject(void);

    void Unlink(void);

    eClientEntityType GetType(void) const { return CCLIENTOBJECT; };

    CObject*       GetGameObject(void) { return m_pObject; }
    CEntity*       GetGameEntity(void) { return m_pObject; }
    const CEntity* GetGameEntity(void) const { return m_pObject; }

    void            GetPosition(CVector& vecPosition) const;
    void            SetPosition(const CVector& vecPosition);
    virtual CSphere GetWorldBoundingSphere(void);

    void         GetRotationDegrees(CVector& vecRotation) const;
    void         GetRotationRadians(CVector& vecRotation) const;
    void         SetRotationDegrees(const CVector& vecRotation);
    virtual void SetRotationRadians(const CVector& vecRotation);

    void GetMoveSpeed(CVector& vecMoveSpeed) const;
    void SetMoveSpeed(const CVector& vecMoveSpeed);

    void GetTurnSpeed(CVector& vecTurnSpeed) const;
    void SetTurnSpeed(const CVector& vecTurnSpeed);

    void         GetOrientation(CVector& vecPosition, CVector& vecRotationRadians);
    virtual void SetOrientation(const CVector& vecPosition, const CVector& vecRotationRadians);

    void ModelRequestCallback(CModelInfo* pModelInfo);

    float GetDistanceFromCentreOfMassToBaseOfModel(void);

    bool IsVisible(void) { return m_bIsVisible; };
    void SetVisible(bool bVisible);

    unsigned short GetModel(void) const { return m_usModel; };
    void           SetModel(unsigned short usModel);

    bool           IsLowLod(void);
    bool           SetLowLodObject(CClientObject* pLowLodObject);
    CClientObject* GetLowLodObject(void);

    void Render(void);

    bool IsFrozen(void) { return m_bIsFrozen; }
    void SetFrozen(bool bFrozen);

    unsigned char GetAlpha(void) { return m_ucAlpha; }
    void          SetAlpha(unsigned char ucAlpha);
    void          GetScale(CVector& vecScale) const;
    void          SetScale(const CVector& vecScale);

    bool IsCollisionEnabled(void) { return m_bUsesCollision; };
    void SetCollisionEnabled(bool bCollisionEnabled);

    float GetHealth(void);
    void  SetHealth(float fHealth);
    float GetTurnMass(void);
    void  SetTurnMass(float fTurnMass);
    float GetAirResistance(void);
    void  SetAirResistance(float fAirResistance);
    float GetElasticity(void);
    void  SetElasticity(float fElasticity);
    float GetBuoyancyConstant(void);
    void  SetBuoyancyConstant(float fBuoyancyConstant);
    void  GetCenterOfMass(CVector& vecCenterOfMass) const;
    void  SetCenterOfMass(const CVector& vecCenterOfMass);

    bool IsBreakable(bool bCheckModelList = true);
    bool SetBreakable(bool bBreakable);
    bool Break(void);
    bool IsRespawnEnabled(void) { return m_bRespawnEnabled; };
    void SetRespawnEnabled(bool bRespawnEnabled) { m_bRespawnEnabled = bRespawnEnabled; };

    float GetMass(void);
    void  SetMass(float fMass);

    bool IsVisibleInAllDimensions(void) { return m_bVisibleInAllDimensions; };
    void SetVisibleInAllDimensions(bool bVisible, unsigned short usNewDimension = 0);

    void ReCreate(void);
    void UpdateVisibility(void);

    bool IsBeingRespawned(void) { return m_bBeingRespawned; };
    void SetBeingRespawned(bool bBeingRespawned) { m_bBeingRespawned = bBeingRespawned; };

protected:
    void StreamIn(bool bInstantly);
    void StreamOut(void);

    void Create(void);
    void Destroy(void);

    void NotifyCreate(void);
    void NotifyDestroy(void);

    void StreamedInPulse(void);

    class CClientObjectManager*       m_pObjectManager;
    class CClientModelRequestManager* m_pModelRequester;

    unsigned short m_usModel;

    CVector       m_vecPosition;
    CVector       m_vecRotation;
    bool          m_bIsVisible;
    bool          m_bIsFrozen;
    bool          m_bUsesCollision;
    unsigned char m_ucAlpha;
    CVector       m_vecScale;
    float         m_fHealth;
    bool          m_bBreakingDisabled;
    bool          m_bBeingRespawned;
    bool          m_bRespawnEnabled;
    float         m_fMass;
    float         m_fTurnMass;
    float         m_fAirResistance;
    float         m_fElasticity;
    float         m_fBuoyancyConstant;
    CVector       m_vecCenterOfMass;
    bool          m_bVisibleInAllDimensions = false;

    CVector m_vecMoveSpeed;
    CVector m_vecTurnSpeed;

    const bool                  m_bIsLowLod;                    // true if this object is low LOD
    CClientObject*              m_pLowLodObject;                // Pointer to low LOD version of this object
    std::vector<CClientObject*> m_HighLodObjectList;            // List of objects that use this object as a low LOD version
    bool                        m_IsHiddenLowLod;               // true if this object is low LOD and should not be drawn

public:
    CObject*              m_pObject;
    SLastSyncedObjectData m_LastSyncedData;
};
