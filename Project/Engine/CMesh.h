#pragma once
#include "CAsset.h"

class CMesh :
    public CAsset
{
private:
    ComPtr<ID3D11Buffer>	m_VB;
    ComPtr<ID3D11Buffer>	m_IB;

    UINT                    m_VtxCount;
    UINT                    m_IdxCount;

    D3D11_BUFFER_DESC       m_VBDesc;
    D3D11_BUFFER_DESC       m_IBDesc;

    void* m_VtxSysMem;
    void* m_IdxSysMem;

public:
    int Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
    virtual void Binding() override;
    void render();


public:
    CMesh();
    ~CMesh();
};

