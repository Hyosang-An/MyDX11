#pragma once
#include "AssetUI.h"
class MaterialUI :
    public AssetUI
{
public:
    virtual void Update() override;

private:
    void ShaderParameter();
    void SelectShader(DWORD_PTR _ListUI);

public:
    MaterialUI();
    ~MaterialUI();
};

