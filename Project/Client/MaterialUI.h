#pragma once
#include "AssetUI.h"
class MaterialUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    MaterialUI();
    ~MaterialUI();
};

