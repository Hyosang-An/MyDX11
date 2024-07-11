#pragma once
#include "CEntity.h"
class CStructuredBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>                m_SB;
    D3D11_BUFFER_DESC                   m_Desc;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;

    UINT                                m_ElementCount;
    UINT                                m_ElementSize;

public:
    UINT GetElementCount() { return m_ElementCount; }
    UINT GetElementSize() { return m_ElementSize; }
    UINT GetBufferSize() { return m_ElementCount * m_ElementSize; }

    void SetData(void* _pData, UINT _DataSize = 0);
    void Binding(UINT _RegisterNum);

public:
    int Create(UINT _ElementSize, UINT _ElementCount, void* _InitData = nullptr);

public:
    CLONE_DISABLE(CStructuredBuffer)
    CStructuredBuffer();
    ~CStructuredBuffer();
};

