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
	void Activate() { m_Active = true; }
	void Deactivate() { m_Active = false; }

	void SetChildBorder(bool _Set) { m_ChildBorder = _Set; }
	void SetChildSize(ImVec2 _Size) { m_ChildSize = _Size; }


public:
	virtual void Tick();
	virtual void Update() = 0;

public:
	EditorUI();
	~EditorUI();
};

