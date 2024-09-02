#pragma once
#include "ComponentUI.h"
class FlipBookComUI :
    public ComponentUI
{
private:
    wstring m_lastFlipBookDirectory;

    float   m_FPS = 12;
	bool    m_Repeat = true;

private:
    virtual void Update() override;

    void AddFlipBookByDialog();

public:
    FlipBookComUI();
    ~FlipBookComUI();
};

