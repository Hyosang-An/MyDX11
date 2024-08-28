#include "pch.h"
#include "Outliner.h"

#include "TreeUI.h"
#include "Inspector.h"
#include "CEditorMgr.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>

Outliner::Outliner()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("OutlinerTree");
	m_Tree->SetOwnerUI(this);
	AddChild(m_Tree);

	// 트리 옵션 세팅
	// 루트 보이지 않기
	m_Tree->ShowRoot(false);

	// Selected Delegate 등록
	m_Tree->AddSelectDelegate((DELEGATE_1)&Outliner::GameObjectSelected);

	// Drag, Drop On
	m_Tree->UseDrag(true);
	m_Tree->UseDrop(true);

	// Self DragDrop Delegate 등록
	m_Tree->AddSelfDragDropDelegate((DELEGATE_2)&Outliner::GameObjectAddChild);

	// 외부 드랍 Delegate 등록
	m_Tree->AddDroppedFromOuterDelegate((DELEGATE_2)&Outliner::DroppedFromOuter);
	m_Tree->SetDropPayLoadName("ContentTree"); // 드롭 받을 외부 TreeUI

	// TreeNode Popup Delegate 등록
	m_Tree->AddPopUpDelegate((DELEGATE_1)&Outliner::PopUpMenu);

	// Asset 상태를 Content 의 TreeUI 에 반영
	RenewLevel();
}

Outliner::~Outliner()
{

}

void Outliner::Update()
{
	// 자식 트리노드만 Update하면 됨. 자기 자신은 따로 해줄 것 없음.

	if (CLevelMgr::GetInst()->IsLevelChanged())
		RenewLevel();

	// 선택된 오브젝트 WorldPos, WorldScale (타일맵은 타일맵 컴포넌트UI에서 처리)
	if (m_SelectedObject && m_SelectedObject->GetLayerIdx() != (int)LAYER::TILEMAP)
	{
		Vec3 objWorldPos = m_SelectedObject->Transform()->GetWorldPos();
		Vec3 objWorldScale = m_SelectedObject->Transform()->GetWorldScale();

		//해당 오브젝트 영역에 반투명한 박스 (Debug Render)
		DrawDebugRect(objWorldPos, objWorldScale * 1.1 ,Vec3(0, 0, 0), Vec4(1.f, 0.f, 1.f, 0.5f), 0.f, false);
	}

	if (m_bRenameObject)
	{
		ImGui::OpenPopup("Rename##GameObject");

		// Always center this window when appearing
		//ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(m_RenamePos, ImGuiCond_Appearing);

		if (ImGui::BeginPopup("Rename##GameObject", ImGuiPopupFlags_None))
		{


			string strObjName = string(m_PopUpSelectedObject->GetName().begin(), m_PopUpSelectedObject->GetName().end());
			char buffer[255];
			sprintf_s(buffer, 255, "%s", strObjName.c_str());

			ImGui::Text("Edit name: ");
			ImGui::InputText("##edit", buffer, 255);
			if (ImGui::IsItemDeactivatedAfterEdit()) 
			{
				string strObjName = string(buffer);
				// 이름 변경
				wstring wstrObjName = wstring(strObjName.begin(), strObjName.end());
				m_PopUpSelectedObject->SetName(wstrObjName);
				m_bRenameObject = false;
				ImGui::CloseCurrentPopup();

				RenewLevel();
			}

			if (ImGui::Button("Close"))
			{
				m_bRenameObject = false;
				ImGui::CloseCurrentPopup();
			}

			// 창 밖을 클릭하면 닫히게, 단 InputText가 활성화되어 있지 않은 경우에만
			if (!ImGui::IsWindowHovered() &&
				!ImGui::IsAnyItemActive() &&  // 활성화된 위젯이 없을 때만
				ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_bRenameObject = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	if (m_bDeleteObject)
	{
		ImGui::OpenPopup("Delete##GameObject");

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);

		if (ImGui::BeginPopup("Delete##GameObject", ImGuiPopupFlags_None))
		{
			string strObjName = string(m_PopUpSelectedObject->GetName().begin(), m_PopUpSelectedObject->GetName().end());
			string text = "Delete Object : " + strObjName;
			ImGui::Text(text.c_str());

			ImGui::NewLine();

			// 버튼들을 중앙에 정렬하기 위해 사용 가능한 너비를 가져옴
			float windowWidth = ImGui::GetWindowSize().x;
			float buttonWidth = ImGui::CalcTextSize("Yes").x + ImGui::GetStyle().FramePadding.x * 2;
			buttonWidth += ImGui::CalcTextSize("No").x + ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().ItemSpacing.x;

			float offsetX = (windowWidth - buttonWidth) * 0.5f;

			ImGui::SetCursorPosX(offsetX); // 버튼을 중앙으로 정렬

			if (ImGui::Button("Yes"))
			{
				// 삭제
				DeleteObject(m_PopUpSelectedObject);
				m_bDeleteObject = false;
				ImGui::CloseCurrentPopup();

				RenewLevel();
			}

			ImGui::SameLine();

			if (ImGui::Button("No"))
			{
				m_bDeleteObject = false;
				ImGui::CloseCurrentPopup();
			}

			// 창 밖을 클릭하면 닫히게, 단 InputText가 활성화되어 있지 않은 경우에만
			if (!ImGui::IsWindowHovered() &&
				!ImGui::IsAnyItemActive() &&  // 활성화된 위젯이 없을 때만
				ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_bDeleteObject = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}

void Outliner::RenewLevel()
{
	// 모든 내용 삭제
	m_Tree->Clear();

	// 루트노드 생성
	TreeNode* pRootNode = m_Tree->AddNode(nullptr, "Root", 0);


	CLevel* pCurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (nullptr == pCurrentLevel)
		return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurrentLevel->GetLayer(i);
		const vector<CGameObject*>& vecParentObjects = pLayer->GetParentObjects();

		for (size_t i = 0; i < vecParentObjects.size(); ++i)
		{
			// 오브젝트가 살아있는 것만
			if (!vecParentObjects[i]->IsDead())
				AddGameObject(pRootNode, vecParentObjects[i]);
		}
	}
}

void Outliner::AddGameObject(TreeNode* _ParentNode, CGameObject* _Object)
{
	string ObjectName = string(_Object->GetName().begin(), _Object->GetName().end());

	TreeNode* pObjectNode = m_Tree->AddNode(_ParentNode, ObjectName.c_str(), (DWORD_PTR)_Object);

	const vector<CGameObject*>& vecChild = _Object->GetChildren();

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(pObjectNode, vecChild[i]);
	}
}

void Outliner::GameObjectSelected(DWORD_PTR _Param)
{
	TreeNode* pSelectedNode = (TreeNode*)_Param;

	m_SelectedObject = (CGameObject*)pSelectedNode->GetData();

	Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
	pInspector->SetTargetObject(m_SelectedObject);
}

void Outliner::GameObjectAddChild(DWORD_PTR _Param1, DWORD_PTR _Param2)
{
	TreeNode* pDragNode = (TreeNode*)_Param1;
	TreeNode* pDropNode = (TreeNode*)_Param2;

	CGameObject* pDragObject = (CGameObject*)pDragNode->GetData();
	CGameObject* pDropObject = nullptr;

	// Drag 오브젝트를 Drop 오브젝트의 자식으로 넣어준다.
	if (pDropNode)
	{
		pDropObject = (CGameObject*)pDropNode->GetData();

		// 자식으로 들어갈 오브젝트가 부모(조상) 중 하나였다면 무시한다.
		if (pDropObject->IsAncestor(pDragObject))
			return;

		pDropObject->AddChild(pDragObject);
	}

	// Drop 목적지가 없기 때문에, Drag 오브젝트를 최상위 부모로 만들어준다.
	else
	{
		// 이미 최상위 부모인 경우
		if (!pDragObject->GetParent())
			return;

		// 부모오브젝트랑 연결을 끊고
		pDragObject->DeregisterThisChildFromParent();

		// 본인 소속 레이어에 최상위 부모로서 재등록 한다.
		CLevelMgr::GetInst()->GetCurrentLevel()->RegisterAsParent(pDragObject->GetLayerIdx(), pDragObject);
	}
}

void Outliner::DroppedFromOuter(DWORD_PTR _OuterData, DWORD_PTR _DropNode)
{
	TreeNode* ContentNode = *((TreeNode**)_OuterData);
	TreeNode* DropNode = (TreeNode*)_DropNode;

	// TODO
}

void Outliner::PopUpMenu(DWORD_PTR _Param)
{
	TreeNode* pTargetNode = (TreeNode*)_Param;
	CGameObject* pObject = (CGameObject*)pTargetNode->GetData();

	if (ImGui::MenuItem("Rename"))
	{
		// 이름 변경 창 띄우기
		m_bRenameObject = true;

		// 이름 변경 창 위치 설정
		m_RenamePos = ImGui::GetItemRectMin();

		m_PopUpSelectedObject = pObject;
	}

	

	if (ImGui::MenuItem("Delete"))
	{
		m_bDeleteObject = true;

		m_PopUpSelectedObject = pObject;
	}
	if (ImGui::MenuItem("Option 3"))
	{
		// Option 3 선택 시 실행할 코드
	}

}

