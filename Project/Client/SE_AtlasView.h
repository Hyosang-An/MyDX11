#pragma once
#include "SE_Sub.h"
class SE_AtlasView :
    public SE_Sub
{
public:
    SE_AtlasView();
    ~SE_AtlasView();

public:
    virtual void Init() override;
    virtual void Update() override;

};

