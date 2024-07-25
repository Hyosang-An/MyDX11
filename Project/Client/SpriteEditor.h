#pragma once
#include "EditorUI.h"

class SE_AtlasView;
class SE_Detail;

class SpriteEditor :
    public EditorUI
{
private:
    SE_AtlasView* m_AtlasView;
    SE_Detail* m_Detail;

public:
    SpriteEditor();
    ~SpriteEditor();

public:
    virtual void Init() override;
    virtual void Update() override;

};

