#pragma once

#include "ImGui/imgui.h"
class EditorUI
{
private:
	static UINT			m_GlobalID;

	string				m_Name;			// â�� ǥ�õ� �̸�
	const UINT			m_ID;			// UI ���� ���̵�
	string				m_FullName;		// UI ���� ���̵� �̿��� Ű name
	bool				m_Active;

	EditorUI*			m_Parent;
	vector<EditorUI*>	m_vecChildUI;

	ImVec2				m_ChildSize;	// �ڽ� â�� ũ��

	bool				m_Modal = false;
	bool				m_ChildBorder;	// �ڽ� ���м� ǥ�� ����


public:
	void AddChild(EditorUI* _UI);

	EditorUI* GetParent() { return m_Parent; }
	const vector<EditorUI*>& GetChildren() { return m_vecChildUI; }

	void SetName(const string& _Name);

	const string& GetName() { return m_Name; }

	UINT GetID() { return m_ID; }
	const string& GetFullName() { return m_FullName; }

	bool IsActive() { return m_Active; }

	void SetChildBorder(bool _Set) { m_ChildBorder = _Set; }
	void SetChildSize(ImVec2 _Size) { m_ChildSize = _Size; }

	void SetActive(bool _Active);

	void SetModal(bool _Modal) { m_Modal = _Modal; }
	bool IsModal() { return m_Modal; }

private:
	virtual void Activate() {}
	virtual void Deactivate() {}

public:
	virtual void Tick();
	virtual void Update() = 0;

public:
	EditorUI();
	virtual ~EditorUI();
};

typedef void(EditorUI::* DELEGATE_0)(void);
typedef void(EditorUI::* DELEGATE_1)(DWORD_PTR);
typedef void(EditorUI::* DELEGATE_2)(DWORD_PTR, DWORD_PTR);
typedef void(EditorUI::* DELEGATE_3)(DWORD_PTR, DWORD_PTR, DWORD_PTR);
