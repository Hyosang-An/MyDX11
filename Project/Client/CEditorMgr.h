#pragma once
#include <Engine\singleton.h>

class CGameObject;
class EditorUI;

class CEditorMgr :
    public CSingleton<CEditorMgr>
{
private:
    friend class CSingleton<CEditorMgr>;
    CEditorMgr();
    ~CEditorMgr();

private:
    vector<CGameObject*>        m_vecEditorObject;
    map<string, EditorUI*>      m_mapUI;

    HANDLE                      m_hNotifyHandle = nullptr;

public:
    EditorUI* FindEditorUI(const string& Name);

public:
    void Init();
    void Progress();

private:
private:
    void ShortCut();

    void CreateEditorObject();
    void EditorObjectProgress();

    void InitImGui();
    void ImGuiProgress();
    void ImGuiTick();
    void ObserveContent();

    void CreateEditorUI();
};

