#pragma once
#include "CAsset.h"

class CShader :
    public CAsset
{
protected:
    ComPtr<ID3DBlob> m_ErrBlob;


public:
    CShader(ASSET_TYPE _Type);
    ~CShader();
};