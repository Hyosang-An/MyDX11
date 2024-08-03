#pragma once
#include "FlipBookEditor.h"
class FlipBookInspector :
    public FlipBookEditor
{
public:
    FlipBookInspector();
    ~FlipBookInspector();

public:
    virtual void Init() override;
    virtual void Update() override;
};

