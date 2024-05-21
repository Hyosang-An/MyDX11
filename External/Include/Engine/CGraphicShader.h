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


public:
    int CreateVertexShader(const wstring& _RelativePath, const string& _FuncName);
    int CreatePixelShader(const wstring& _RelativePath, const string& _FuncName);

    virtual void Binding() override;


public:
    CGraphicShader();
    ~CGraphicShader();
};

