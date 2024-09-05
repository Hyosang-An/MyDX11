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

    CGameObject*                m_EditorCamera = nullptr;

    HANDLE                      m_hNotifyHandle = nullptr;

    bool                        m_isAnyUI_Hovered = false;
	bool                        m_isAnyUI_Hovered_Buffer = false;

public:
    EditorUI* FindEditorUI(const string& Name);

public:
    void Init();
    void Progress();

public:
    CGameObject* GetEditorCamera() { return m_EditorCamera; }
    void SetUI_Hovered(bool _b);
	bool IsAnyUI_Hovered() { return m_isAnyUI_Hovered; }

private:
    void ShortCut();

    void CreateEditorObject();
    void EditorObjectProgress();

    void InitImGui();
    void ImGuiAndEditorObjProgress();
    void ImGuiTick();
    void ObserveContent();

    void CreateEditorUI();

	void MouseHoveredCheck();
};

