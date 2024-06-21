#pragma once
#include "EditorUI.h"
class Outliner :
    public EditorUI
{
public:
    virtual void Update() override;

public:
    Outliner();
    ~Outliner();
};

