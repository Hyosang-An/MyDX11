#pragma once
#include "AssetUI.h"
class PrefabUI :
    public AssetUI
{
public:
    virtual void Update() override;

private:
	LAYER m_spawnLayer = LAYER::DEFAULT;

	virtual void Activate() override;

public:
    PrefabUI();
    ~PrefabUI();
};

