#pragma once
#include "CShader.h"

class CGraphicShader :
    public CShader
{
private:
    ComPtr<ID3DBlob>			m_VSBlob;
    ComPtr<ID3DBlob>			m_PSBlob;

    ComPtr<ID3D11VertexShader>  m_VS;
    ComPtr<ID3D11PixelShader>	m_PS;

    ComPtr<ID3D11InputLayout>   m_Layout;

    D3D11_PRIMITIVE_TOPOLOGY    m_Topology;

    RS_TYPE                     m_RSType;
    DS_TYPE                     m_DSType;
    BS_TYPE                     m_BSType;

    SHADER_DOMAIN               m_Domain;   // ½¦ÀÌ´õÀÇ ·»´õ¸µ ¹æ½Ä

public:
    void SetDomain(SHADER_DOMAIN _Domain) { m_Domain = _Domain; }
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topology = _Topology; }
    void SetRSType(RS_TYPE _Type) { m_RSType = _Type; }
    void SetDSType(DS_TYPE _Type) { m_DSType = _Type; }
    void SetBSType(BS_TYPE _Type) { m_BSType = _Type; }

    SHADER_DOMAIN GetDomain() { return m_Domain; }
    RS_TYPE GetRSType() { return m_RSType; }
    DS_TYPE GetDSType() { return m_DSType; }
    BS_TYPE GetBSType() { return m_BSType; }


public:
    int CreateVertexShader(const wstring& _RelativePath, const string& _FuncName);
    int CreatePixelShader(const wstring& _RelativePath, const string& _FuncName);

    void Binding();


public:
    CGraphicShader();
    ~CGraphicShader();
};

