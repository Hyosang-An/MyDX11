#pragma once
#include <Engine\CScript.h>

class CEditorCameraScript :
    public CScript
{
private:
    float   m_Speed;
	float   m_ZoomRatio = 1.f;

public:
    virtual void Tick() override;

    virtual void SaveToFile(FILE* _File) override {}
    virtual void LoadFromFile(FILE* _File)override {}

private:
    void OrthoGraphicMove();
    void PerspectiveMove();

public:
    CLONE(CEditorCameraScript);
    CEditorCameraScript();
    ~CEditorCameraScript();
};

