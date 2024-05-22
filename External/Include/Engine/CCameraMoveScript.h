#pragma once
#include "CScript.h"
class CCameraMoveScript :
    public CScript
{
public:
    CCameraMoveScript();
    ~CCameraMoveScript();
    virtual CCameraMoveScript* Clone() { return new CCameraMoveScript(*this); }

private:
    float   m_CamSpeed;

public:
    virtual void Tick() override;
};

