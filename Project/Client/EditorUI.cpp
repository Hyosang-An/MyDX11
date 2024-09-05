#include "pch.h"
#include "EditorUI.h"

#include "CEditorMgr.h"

#include "ImGui/imgui.h"

UINT EditorUI::m_GlobalID = 0;

EditorUI::EditorUI()
	: m_ID(m_GlobalID++)
{

}

EditorUI::~EditorUI()
{
	Delete_Vec(m_vecChildUI);
}

void EditorUI::Tick()
{
	if (!m_Active)
		return;

	bool bActive = m_Active;

	int flags = 0;
	if (m_HorizontalScrollbar)
		flags |= ImGuiWindowFlags_HorizontalScrollbar;
	if (m_UseMenuBar)
		flags |= ImGuiWindowFlags_MenuBar;
	if (!m_Move)
		flags |= ImGuiWindowFlags_NoMove;


	// 창의 최소 크기 및 최대 크기를 설정
	if (m_isSizeConstrained)
		ImGui::SetNextWindowSizeConstraints(m_minSize, m_maxSize);
	

	// 최상위 부모 UI 인 경우
	if (nullptr == m_Parent)
	{
		// Modaless
		if (false == m_Modal)
		{			
			ImGui::Begin(m_FullName.c_str(), &bActive, flags);

			if (m_Active != bActive)
			{
				SetActive(bActive);
			}

			Update();

			for (size_t i = 0; i < m_vecChildUI.size(); ++i)
			{
				m_vecChildUI[i]->Tick();

				if (m_vecChildUI[i]->m_ChildBorder && i == m_vecChildUI.size() - 1)
					ImGui::Separator();
			}

			MouseHoverCheck();

			ImGui::End();
		}


		// Modal
		else
		{
			
			ImGui::OpenPopup(m_FullName.c_str());

			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal(m_FullName.c_str(), &bActive))
			{
				Update();

				for (size_t i = 0; i < m_vecChildUI.size(); ++i)
				{
					m_vecChildUI[i]->Tick();

					if (m_vecChildUI[i]->m_ChildBorder && i == m_vecChildUI.size() - 1)
						ImGui::Separator();
				}

				ImGui::EndPopup();
			}
			else
			{
				if (m_Active != bActive)
				{
					SetActive(bActive);
				}
			}
		}
	}

	// 자식 타입 UI 인 경우
	else
	{
		if (m_ChildBorder)
		{
			ImGui::Separator();
		}

		ImGui::BeginChild(m_FullName.c_str(), m_ChildSize);


		Update();

		for (size_t i = 0; i < m_vecChildUI.size(); ++i)
		{
			//if (m_vecChildUI[i]->m_ChildBorder)
			//	ImGui::Separator();

			m_vecChildUI[i]->Tick();

			// 마지막 자식 UI 구분선 닫아주기
			if (m_vecChildUI[i]->m_ChildBorder && i == m_vecChildUI.size() - 1)
				ImGui::Separator();
		}

		MouseHoverCheck();

		ImGui::EndChild();
	}
}

void EditorUI::AddChild(EditorUI* _UI)
{
	_UI->m_Parent = this;
	m_vecChildUI.push_back(_UI);
}

void EditorUI::SetName(const string& _Name)
{
	m_Name = _Name;

	//// ID 문자열 생성
	//m_FullName = m_Name;
	//char szNum[50] = {};

	//// 수정 예정
	//_itoa_s(m_ID, szNum, 10);

	// ## 뒤로는 UI창에 표시 안됨
	m_FullName = m_Name + "##" + std::to_string(m_ID);
}

void EditorUI::SetActive(bool _Active)
{
	if (m_Active == _Active)
		return;

	m_Active = _Active;

	if (m_Active)
		Activate();
	else
		Deactivate();
}

void EditorUI::SetFocus()
{
	ImGui::SetWindowFocus(m_FullName.c_str());
}

void EditorUI::MouseHoverCheck()
{
	ImVec2 windowPos = ImGui::GetWindowPos();  // 스크린 좌표계에서의 좌상단 좌표
	ImVec2 windowSize = ImGui::GetWindowSize(); // 윈도우 크기
	ImVec2 windowRBPos = ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y); // 스크린 좌표계에서의 우하단 좌표
	
	ImVec2 mousePos = ImGui::GetMousePos(); // 마우스 좌표

	if (mousePos.x >= windowPos.x && mousePos.x <= windowRBPos.x &&
		mousePos.y >= windowPos.y && mousePos.y <= windowRBPos.y)
	{
		CEditorMgr::GetInst()->SetUI_Hovered(true);
	}
	else
	{
		CEditorMgr::GetInst()->SetUI_Hovered(false);
	}

}
