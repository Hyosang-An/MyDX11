#pragma once
#include "CEntity.h"

enum SB_TYPE
{
    SRV_ONLY,
    SRV_UAV,
};

class CStructuredBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>                m_SB;           // 메인 버퍼
    ComPtr<ID3D11Buffer>                m_SB_Write;     // CPU 쓰기 버퍼
    ComPtr<ID3D11Buffer>                m_SB_Read;      // CPU 읽기 버퍼

    D3D11_BUFFER_DESC                   m_Desc;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    SB_TYPE                             m_Type;
    bool                                m_SysMemMove = false;

    UINT                                m_ElementCount;
    UINT                                m_ElementSize;

public:
    UINT GetElementCount() { return m_ElementCount; }
    UINT GetElementSize() { return m_ElementSize; }
    UINT GetBufferSize() { return m_ElementCount * m_ElementSize; }

    void SetData(void* _pData, UINT _DataSize = 0);
    void GetData(void* _pData, UINT _DataSize = 0);

    void Binding(UINT _RegisterNum);

public:
    int Create(UINT _ElementSize, UINT _ElementCount, SB_TYPE _Type = SRV_ONLY, bool _SysMemMove = false, void* _InitData = nullptr);

public:
    CLONE_DISABLE(CStructuredBuffer)
    CStructuredBuffer();
    ~CStructuredBuffer();
};

