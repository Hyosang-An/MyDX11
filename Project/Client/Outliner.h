#pragma once
#include "EditorUI.h"
class Outliner :
    public EditorUI
{
private:
    class TreeUI* m_Tree;

public:
    virtual void Update() override;

public:
    Outliner();
    ~Outliner();
};

