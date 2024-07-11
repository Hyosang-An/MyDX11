#pragma once
#include <Engine\CScript.h>

class CEditorCameraScript :
    public CScript
{
private:
    float   m_Speed;

public:
    virtual void Tick() override;

private:
    void OrthoGraphicMove();
    void PerspectiveMove();

public:
    CLONE(CEditorCameraScript);
    CEditorCameraScript();
    ~CEditorCameraScript();
};

