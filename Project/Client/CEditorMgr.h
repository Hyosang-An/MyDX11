#pragma once
#include <Engine\singleton.h>

class CGameObject;

class CEditorMgr :
    public CSingleton<CEditorMgr>
{
private:
    friend class CSingleton<CEditorMgr>;
    CEditorMgr();
    ~CEditorMgr();

private:
    vector<CGameObject*>    m_vecEditorObject;


public:
    void Init();
    void Tick();

private:
    void InitImGui();
    void ImGuiProgress();
    void ImGuiTick();

    void CreateEditorObject();
};

