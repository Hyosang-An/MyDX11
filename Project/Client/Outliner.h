#pragma once
#include "EditorUI.h"
class Outliner :
    public EditorUI
{
private:
    class TreeUI* m_Tree;

	class CGameObject* m_SelectedObject;

    bool m_bRenameObject = false;
	ImVec2 m_RenamePos;

	bool m_bDeleteObject = false;

    class CGameObject* m_PopUpSelectedObject;

	wstring m_lastPrefabSaveDirectory;

public:
	void SetSelectedObject(class CGameObject* _Object) { m_SelectedObject = _Object; }

public:
    virtual void Update() override;

private:
    void RenewLevel();
    void GameObjectSelected(DWORD_PTR _Param);
    void AddGameObject(class TreeNode* pNode, class CGameObject* _Object);
	void SaveAsPrefabByDialog();

    void GameObjectAddChild(DWORD_PTR _Param1, DWORD_PTR _Param2);
    void DroppedFromOuter(DWORD_PTR _OuterData, DWORD_PTR _DropNode);
    void PopUpMenu(DWORD_PTR _Param);
public:
    Outliner();
    ~Outliner();
};

