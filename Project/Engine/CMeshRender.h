#pragma once

#include "CRenderComponent.h"

class CMeshRender :
    public CRenderComponent
{

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual CMeshRender* Clone() { return new CMeshRender(*this); }

public:
    CMeshRender();
    ~CMeshRender();
};