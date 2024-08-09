#pragma once
#include "FlipBookEditor.h"
class FlipBookViewer :
    public FlipBookEditor
{
    friend class FlipBookEditor;
public:
    FlipBookViewer();
    ~FlipBookViewer();

private:
    FlipBookEditor* m_owner = nullptr;

public:
    virtual void Init() override;
    virtual void Update() override;
};

