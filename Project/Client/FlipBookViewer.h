#pragma once
#include "FlipBookEditor.h"
class FlipBookViewer :
    public FlipBookEditor
{
public:
    FlipBookViewer();
    ~FlipBookViewer();

public:
    virtual void Init() override;
    virtual void Update() override;
};

