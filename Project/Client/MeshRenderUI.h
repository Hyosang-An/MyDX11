#pragma once
#include "ComponentUI.h"
class MeshRenderUI :
    public ComponentUI
{
private:
    virtual void Update() override;

public:
    MeshRenderUI();
    ~MeshRenderUI();
};

