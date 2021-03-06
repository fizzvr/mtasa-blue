/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveBatcher.cpp
 *  PURPOSE:
 *
 *
 *****************************************************************************/
#include <StdInc.h>
#include "CPrimitiveBatcher.h"
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::CPrimitiveBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveBatcher::CPrimitiveBatcher(bool m_bZTest)
{
    m_bZTest = m_bZTest;
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::~CPrimitiveBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveBatcher::~CPrimitiveBatcher(void)
{
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
    // Cache matrices
    UpdateMatrices(fViewportSizeX, fViewportSizeY);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::OnRenderTargetChange
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveBatcher::OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY)
{
    // Flush dx draws
    Flush();
    // Make new projection transform
    UpdateMatrices(uiNewViewportSizeX, uiNewViewportSizeY);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::UpdateMatrices
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveBatcher::UpdateMatrices(float fViewportSizeX, float fViewportSizeY)
{
    m_fViewportSizeX = fViewportSizeX;
    m_fViewportSizeY = fViewportSizeY;
    D3DXMatrixIdentity(&m_MatView);
    D3DXMatrixIdentity(&m_MatProjection);
    // Make projection 3D friendly, so shaders can alter the z coord for fancy effects
    float fFarPlane = 10000;
    float fNearPlane = 100;
    float Q = fFarPlane / (fFarPlane - fNearPlane);
    float fAdjustZFactor = 1000.f;
    float rcpSizeX = 2.0f / m_fViewportSizeX;
    float rcpSizeY = -2.0f / m_fViewportSizeY;
    rcpSizeX *= fAdjustZFactor;
    rcpSizeY *= fAdjustZFactor;
    m_MatProjection.m[0][0] = rcpSizeX;
    m_MatProjection.m[1][1] = rcpSizeY;
    m_MatProjection.m[2][2] = Q;
    m_MatProjection.m[2][3] = 1;
    m_MatProjection.m[3][0] = (-m_fViewportSizeX / 2.0f - 0.5f) * rcpSizeX;
    m_MatProjection.m[3][1] = (-m_fViewportSizeY / 2.0f - 0.5f) * rcpSizeY;
    m_MatProjection.m[3][2] = -Q * fNearPlane;
    m_MatProjection.m[3][3] = 0;
    m_MatView.m[3][2] = fAdjustZFactor;
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CPrimitiveBatcher::Flush(void)
{
    if (m_primitiveList.empty())
        return;

    // Save render states
    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);
    // Set transformations
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
    m_pDevice->SetTransform(D3DTS_VIEW, &m_MatView);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &m_MatProjection);

    // Set vertex FVF
    m_pDevice->SetFVF(PrimitiveVertice::FNV);

    // Set states
    m_pDevice->SetRenderState(D3DRS_ZENABLE, m_bZTest ? D3DZB_TRUE : D3DZB_FALSE);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // Draw
    m_pDevice->SetTexture(0, nullptr);
    for (int i = 0; i < m_primitiveList.size(); i++)
    {
        sDrawQueuePrimitive primitive = m_primitiveList[i];

        const void* pVertexStreamZeroData = &primitive.vertices[0];
        uint        uiVertexStreamZeroStride = sizeof(PrimitiveVertice);

        DrawPrimitive(primitive.type, primitive.vertices.size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
    }

    // Clean up
    ClearQueue();
    // Restore render states
    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::DrawPrimitive
//
// Draws the primitives on render target
//
////////////////////////////////////////////////////////////////
void CPrimitiveBatcher::DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride)
{
    int iSize = 1;
    switch (eType)
    {
    case D3DPT_POINTLIST:
        iSize = iCollectionSize;
        break;
    case D3DPT_LINELIST:
        iSize = iCollectionSize / 2;
        break;
    case D3DPT_LINESTRIP:
        iSize = iCollectionSize - 1;
        break;
    case D3DPT_TRIANGLEFAN:
    case D3DPT_TRIANGLESTRIP:
        iSize = iCollectionSize - 2;
        break;
    case D3DPT_TRIANGLELIST:
        iSize = iCollectionSize / 3;
        break;
    }
    m_pDevice->DrawPrimitiveUP(eType, iSize, pDataAddr, uiVertexStride);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::ClearQueue
//
// Clears all primitives in current queue
//
////////////////////////////////////////////////////////////////
void CPrimitiveBatcher::ClearQueue()
{
    // Clean up
    size_t prevSize = m_primitiveList.size();
    m_primitiveList.clear();
    m_primitiveList.reserve(prevSize);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveBatcher::AddTriangle
//
// Add a new primitive to the list
//
////////////////////////////////////////////////////////////////
void CPrimitiveBatcher::AddPrimitive(sDrawQueuePrimitive primitive)
{
    m_primitiveList.push_back(primitive);
}
