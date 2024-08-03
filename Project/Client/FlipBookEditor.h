#pragma once
#include "EditorUI.h"
class FlipBookEditor :
    public EditorUI
{
public:
    FlipBookEditor();
    ~FlipBookEditor();


private:
    class FlipBookViewer* m_Viewer = nullptr;
    class FlipBookInspector* m_Inspector = nullptr;

    float m_prevViewerWidth;
    

private:
    ImVec2 Splitter(float thickness, float* leftSize, float* rightSize, float min_leftSize, float min_rightSize);

public:
    virtual void Init() override;
    virtual void Update() override;

    virtual void Activate() override;
    virtual void Deactivate() override;
};

