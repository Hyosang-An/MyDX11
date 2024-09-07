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
#include <Engine/CPathMgr.h>

Outliner::Outliner()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("OutlinerTree");
	m_Tree->SetOwnerUI(this);
	AddChild(m_Tree);

	// Ʈ�� �ɼ� ����
	// ��Ʈ ������ �ʱ�
	m_Tree->ShowRoot(false);

	// Selected Delegate ���
	m_Tree->AddSelectDelegate((DELEGATE_1)&Outliner::GameObjectSelected);

	// Drag, Drop On
	m_Tree->UseDrag(true);
	m_Tree->UseDrop(true);

	// Self DragDrop Delegate ���
	m_Tree->AddSelfDragDropDelegate((DELEGATE_2)&Outliner::GameObjectAddChild);

	// �ܺ� ��� Delegate ���
	m_Tree->AddDroppedFromOuterDelegate((DELEGATE_2)&Outliner::DroppedFromOuter);
	m_Tree->SetDropPayLoadName("ContentTree"); // ��� ���� �ܺ� TreeUI

	// TreeNode Popup Delegate ���
	m_Tree->AddPopUpDelegate((DELEGATE_1)&Outliner::PopUpMenu);

	// Asset ���¸� Content �� TreeUI �� �ݿ�
	RenewLevel();
}

Outliner::~Outliner()
{

}

void Outliner::Update()
{
	// �ڽ� Ʈ����常 Update�ϸ� ��. �ڱ� �ڽ��� ���� ���� �� ����.

	if (CLevelMgr::GetInst()->IsLevelChanged())
		RenewLevel();

	if (m_SelectedObject && m_SelectedObject->IsDead())
	{
		m_SelectedObject = nullptr;
		Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
		pInspector->SetTargetObject(nullptr);
	}

	// ���õ� ������Ʈ �ѷ��� ����� �׵θ� (Ÿ�ϸ��� Ÿ�ϸ� ������ƮUI���� ó��)
	if (m_SelectedObject && m_SelectedObject->GetLayerIdx() != (int)LAYER::TILEMAP)
	{
		Vec3 objWorldPos = m_SelectedObject->Transform()->GetWorldPos();
		Vec3 objWorldScale = m_SelectedObject->Transform()->GetWorldScale();

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
				// �̸� ����
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

			// â ���� Ŭ���ϸ� ������, �� InputText�� Ȱ��ȭ�Ǿ� ���� ���� ��쿡��
			if (!ImGui::IsWindowHovered() &&
				!ImGui::IsAnyItemActive() &&  // Ȱ��ȭ�� ������ ���� ����
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

			// ��ư���� �߾ӿ� �����ϱ� ���� ��� ������ �ʺ� ������
			float windowWidth = ImGui::GetWindowSize().x;
			float buttonWidth = ImGui::CalcTextSize("Yes").x + ImGui::GetStyle().FramePadding.x * 2;
			buttonWidth += ImGui::CalcTextSize("No").x + ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().ItemSpacing.x;

			float offsetX = (windowWidth - buttonWidth) * 0.5f;

			ImGui::SetCursorPosX(offsetX); // ��ư�� �߾����� ����

			if (ImGui::Button("Yes"))
			{
				// ����
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

			// â ���� Ŭ���ϸ� ������, �� InputText�� Ȱ��ȭ�Ǿ� ���� ���� ��쿡��
			if (!ImGui::IsWindowHovered() &&
				!ImGui::IsAnyItemActive() &&  // Ȱ��ȭ�� ������ ���� ����
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
	// ��� ���� ����
	m_Tree->Clear();

	// ��Ʈ��� ����
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
			// ������Ʈ�� ����ִ� �͸�
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
		// ������Ʈ�� ����ִ� �͸�
		if (!vecChild[i]->IsDead())
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

	// Drag ������Ʈ�� Drop ������Ʈ�� �ڽ����� �־��ش�.
	if (pDropNode)
	{
		pDropObject = (CGameObject*)pDropNode->GetData();

		// �ڽ����� �� ������Ʈ�� �θ�(����) �� �ϳ����ٸ� �����Ѵ�.
		if (pDropObject->IsAncestor(pDragObject))
			return;

		pDropObject->AddChild(pDragObject);
	}

	// Drop �������� ���� ������, Drag ������Ʈ�� �ֻ��� �θ�� ������ش�.
	else
	{
		// �̹� �ֻ��� �θ��� ���
		if (!pDragObject->GetParent())
			return;

		// �θ������Ʈ�� ������ ����
		pDragObject->DeregisterThisChildFromParent();

		// ���� �Ҽ� ���̾ �ֻ��� �θ�μ� ���� �Ѵ�.
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
	m_PopUpSelectedObject = pObject;

	if (ImGui::MenuItem("Rename"))
	{
		// �̸� ���� â ����
		m_bRenameObject = true;

		// �̸� ���� â ��ġ ����
		m_RenamePos = ImGui::GetItemRectMin();
	}

	

	if (ImGui::MenuItem("Delete"))
	{
		m_bDeleteObject = true;
	}

	if (ImGui::MenuItem("Clone"))
	{
		// Ŭ��
		CGameObject* pClone = pObject->Clone();
		
		// �̸� ����
		wstring wstrObjName = pObject->GetName();
		wstrObjName += L" Clone";
		pClone->SetName(wstrObjName);

		// ���� ����
		CLevel* pCurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
		pCurrentLevel->AddObject(pObject->GetLayerIdx(), pClone);
	}

	if (ImGui::MenuItem("Save As Prefab"))
	{
		// Prefab���� ����
		SaveAsPrefabByDialog();
	}

}

void Outliner::SaveAsPrefabByDialog()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileSaveDialog* pFileSave;

		// CLSID_FileSaveDialog Ŭ������ �ν��Ͻ��� �����ϰ� IFileSaveDialog �������̽� �����͸� �����ɴϴ�.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr)) {
			// ���� ���� ���͸� �����մϴ�.
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"Prefab Files", L"*.prefab" },
				{ L"All Files", L"*.*" }
			};
			pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec); // ���͸� ��ȭ���ڿ� �����մϴ�.
			pFileSave->SetFileTypeIndex(1); // �⺻ ���� ������ �����մϴ�. rgSpec�� � ���� �⺻������ ����. Index�� 1����.
			pFileSave->SetDefaultExtension(L"prefab"); // �⺻ Ȯ���ڸ� �����մϴ�.
			pFileSave->SetTitle(L"Save Prefab File"); // ��ȭ������ ������ �����մϴ�.

			// ���⼭ �⺻ ���� �̸��� �����մϴ�. �⺻ �̸��� ������Ʈ �̸����� �����մϴ�.
			wstring DefaultFileName = m_PopUpSelectedObject->GetName();
			pFileSave->SetFileName(DefaultFileName.c_str()); // �⺻ ���� �̸� ����

			// ������ ��θ� �ʱ� ������ ����
			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastPrefabSaveDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"prefab";
			else
				defaultDirectory = m_lastPrefabSaveDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFileSave->SetFolder(psiFolder);
				psiFolder->Release();
			}

			// ���� ���� ��ȭ���ڸ� ǥ���մϴ�.
			hr = pFileSave->Show(NULL);

			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;

				// ����ڰ� ������ ������ IShellItem�� �����ɴϴ�.
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;

					// IShellItem���� ���� ��θ� �����ɴϴ�.
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						// ���⿡�� pszFilePath ��θ� ����Ͽ� ������ �����մϴ�.
						Ptr<CPrefab> pPrefab = new CPrefab;
						CGameObject* PrefabObj = m_PopUpSelectedObject->Clone();
						PrefabObj->SetLayerIdxForPrefabSave(m_PopUpSelectedObject->GetLayerIdx());
						pPrefab->SetProtoObject(PrefabObj);
						pPrefab->Save(pszFilePath);

						// Prefab�� �����鼭 Ÿ�� ����ִ� m_ProtoObject�� �ڵ����� �Ҹ��ϱ� ������ ���� PrefabObj�� ������ �ʿ� ����.


						// ������ ���ϸ� �� ���丮 ������Ʈ
						path filePath = pszFilePath;
						m_lastPrefabSaveDirectory = filePath.parent_path().wstring();
						// ���� ��� ��� �� �޸𸮸� �����մϴ�.
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release(); // IShellItem �����͸� �����մϴ�.
				}
			}
			pFileSave->Release(); // IFileSaveDialog �����͸� �����մϴ�.
		}
		CoUninitialize(); // COM ���̺귯���� �����մϴ�.
	}
}