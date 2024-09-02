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
    FlipBookEditor*     m_owner = nullptr;
    FlipBookInspector*  m_inspector = nullptr;

	float               m_ZoomRatio = 1;        // 실제 해상도 대비 출력 이미지의 비율
private:
	void WheelCheck();

public:
    virtual void Init() override;
    virtual void Update() override;
};

