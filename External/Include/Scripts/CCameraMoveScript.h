#pragma once
#include "Engine/CScript.h"
class CCameraMoveScript :
    public CScript
{
public:
    CCameraMoveScript();
    ~CCameraMoveScript();
    virtual CCameraMoveScript* Clone() { return new CCameraMoveScript(*this); }

private:
    float   m_CamSpeed;

private:
    void OrthoGraphicMove();
    void PerspectiveMove();


public:
    virtual void Tick() override;
};

