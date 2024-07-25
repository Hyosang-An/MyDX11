#include "pch.h"
#include "SpriteEditor.h"


#include "CEditorMgr.h"
#include "SE_AtlasView.h"
#include "SE_Detail.h"

SpriteEditor::SpriteEditor()
{
}

SpriteEditor::~SpriteEditor()
{
}

void SpriteEditor::Init()
{
	m_AtlasView = (SE_AtlasView*)CEditorMgr::GetInst()->FindEditorUI("SE_AtlasView");
	m_Detail = (SE_Detail*)CEditorMgr::GetInst()->FindEditorUI("SE_Detail");
}

void SpriteEditor::Update()
{
	//// 왼쪽 하위 창
	//ImGui::BeginChild("Left Pane", ImVec2(200, 0), true);
	////ImGui::Text("Left Pane");
	//ImGui::Button("Left Pane");
	//ImGui::EndChild();

	//// 오른쪽 하위 창
	//ImGui::SameLine();
	//ImGui::BeginChild("Right Pane", ImVec2(0, 0), true);
	//if (ImGui::BeginTabBar("LeftTabBar"))
	//{
	//	if (ImGui::BeginTabItem("Left Tab 1")) {
	//		ImGui::Text("This is left tab 1");
	//		ImGui::EndTabItem();
	//	}
	//	ImGui::EndTabBar();
	//}
	//ImGui::EndChild();
}
