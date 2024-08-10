#include "pch.h"
#include "SC_Detail.h"

#include "CEditorMgr.h"

#include "ListUI.h"
#include "TreeUI.h"

#include "SC_AtlasView.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CSprite.h>
#include <Engine/func.h>

SE_Detail::SE_Detail()
{
}

SE_Detail::~SE_Detail()
{
}


void SE_Detail::Init()
{
	m_lastSaveDirectory = CPathMgr::GetInst()->GetContentsPath() + L"animation\\";
}

void SE_Detail::Update()
{
	Atlas();

	AtlasInfo();

	SpriteInfo();
}

void SE_Detail::Atlas()
{
	string TexName;

	if (nullptr != m_AtlasTex)
		TexName = string(m_AtlasTex->GetKey().begin(), m_AtlasTex->GetKey().end());

	ImGui::Text("Atlas Texture");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##AtlasTex", (char*)TexName.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
		if (payload)
		{
			TreeNode** ppNode = (TreeNode**)payload->Data;
			TreeNode* pNode = *ppNode;

			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (ASSET_TYPE::TEXTURE == pAsset->GetAssetType())
			{
				SetAtlasTex((CTexture*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##AtlasTexBtn", ImVec2(18.f, 18.f)))
	{
		ListUI* pListUI = (ListUI*)CEditorMgr::GetInst()->FindEditorUI("List");
		pListUI->SetName("Texture");
		vector<string> vecTexNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::TEXTURE, vecTexNames);
		pListUI->AddList(vecTexNames);
		pListUI->AddDelegate(this, (DELEGATE_1)&SE_Detail::SelectTexture);
		pListUI->SetActive(true);
	}
}

void SE_Detail::AtlasInfo()
{
	// 해상도
	UINT width = 0;
	UINT height = 0;

	if (nullptr != m_AtlasTex)
	{
		width = m_AtlasTex->Width();
		height = m_AtlasTex->Height();
	}

	char buff[50] = {};
	sprintf_s(buff, "%d", width);

	ImGui::Text("Width");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureWidth", buff, 50, ImGuiInputTextFlags_ReadOnly);

	sprintf_s(buff, "%d", height);
	ImGui::Text("Height");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureHeight", (char*)std::to_string(height).c_str(), 50, ImGuiInputTextFlags_ReadOnly);
}

void SE_Detail::SpriteInfo()
{
	ImGui::Separator();

	auto spriteBoxPos = GetAtlasView()->GetSpriteBoxPosOnOriginalTex();
	if (spriteBoxPos.first == spriteBoxPos.second)
	{
		spriteBoxPos.first = Vec2();
		spriteBoxPos.second = Vec2();
	}

	m_SpriteLT = spriteBoxPos.first;
	m_SpriteSize = spriteBoxPos.second - spriteBoxPos.first;

	ImGui::PushID("Sprite Info");
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));

	ImGui::Button("Sprite Info");

	ImGui::PopStyleColor(3);
	ImGui::PopID();



	ImGui::Text("Left Top");
	ImGui::SameLine(100);
	ImGui::InputFloat2("##Left Top", m_SpriteLT, "%.0f", ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Right Bottom");
	ImGui::SameLine(100);
	ImGui::InputFloat2("##Right Bottom", spriteBoxPos.second, "%.0f", ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Size");
	ImGui::SameLine(100);
	ImGui::InputFloat2("##Right Bottom", m_SpriteSize, "%.0f", ImGuiInputTextFlags_ReadOnly);

	ImGui::NewLine();

	ImGui::Text("Set Background Size");
	ImGui::InputFloat2("##Set Background Size", m_BackgroundSize, "%.0f");

	// "Save Sprite" 버튼
	ImGui::BeginDisabled(m_SpriteSize.x * m_SpriteSize.y == 0 || m_AtlasTex == nullptr);
	if (ImGui::Button("Save Sprite")) 
		SaveSprite();

	ImGui::EndDisabled(); 


}

void SE_Detail::SetAtlasTex(Ptr<CTexture> _Tex)
{
	m_AtlasTex = _Tex;
	GetAtlasView()->SetAtlasTex(m_AtlasTex);
}

void SE_Detail::SelectTexture(DWORD_PTR _ListUI)
{
	ListUI* pListUI = (ListUI*)_ListUI;
	string strName = pListUI->GetSelectName();

	if (strName == "None")
	{
		m_AtlasTex = nullptr;
		return;
	}

	wstring strAssetName = wstring(strName.begin(), strName.end());

	Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(strAssetName);

	assert(pTex.Get());

	SetAtlasTex(pTex);
}


void SE_Detail::SaveSprite()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileSaveDialog* pFileSave;

		// CLSID_FileSaveDialog 클래스의 인스턴스를 생성하고 IFileSaveDialog 인터페이스 포인터를 가져옵니다.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr)) {
			// 파일 형식 필터를 설정합니다.
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"Sprite Files", L"*.sprite" },
				{ L"All Files", L"*.*" }
			};
			pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec); // 필터를 대화상자에 설정합니다.
			pFileSave->SetFileTypeIndex(1); // 기본 파일 형식을 설정합니다. rgSpec중 어떤 것을 기본값으로 할지. Index는 1부터.
			pFileSave->SetDefaultExtension(L"sprite"); // 기본 확장자를 설정합니다.
			pFileSave->SetTitle(L"Save sprite File"); // 대화상자의 제목을 설정합니다.

			// 여기서 기본 파일 이름을 설정합니다.
			wstring possibleNextFileName;
			if (m_lastFileName != L"")
			{
				possibleNextFileName = incrementTrailingNumber(m_lastFileName);
			}
			pFileSave->SetFileName(possibleNextFileName.c_str()); // 기본 파일 이름 설정

			// 마지막 경로를 초기 폴더로 설정
			IShellItem* psiFolder = nullptr;
			hr = SHCreateItemFromParsingName(m_lastSaveDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFileSave->SetFolder(psiFolder);
				psiFolder->Release();
			}

			// 파일 저장 대화상자를 표시합니다.
			hr = pFileSave->Show(NULL);

			if (SUCCEEDED(hr)) 
			{
				IShellItem* pItem;

				// 사용자가 선택한 파일의 IShellItem을 가져옵니다.
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr)) 
				{
					PWSTR pszFilePath;

					// IShellItem에서 파일 경로를 가져옵니다.
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) 
					{
						// 여기에서 pszFilePath 경로를 사용하여 파일을 저장합니다.

						Ptr<CSprite> pSprite = new CSprite;
						pSprite->Create(m_AtlasTex, m_SpriteLT, m_SpriteSize);
						pSprite->SetBackground(m_BackgroundSize);

						pSprite->Save(pszFilePath);
						

						// 마지막 파일명 및 디렉토리 업데이트
						path filePath = pszFilePath;
						m_lastFileName = filePath.stem().wstring();
						m_lastSaveDirectory = filePath.parent_path().wstring();
						// 파일 경로 사용 후 메모리를 해제합니다.
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release(); // IShellItem 포인터를 해제합니다.
				}
			}
			pFileSave->Release(); // IFileSaveDialog 포인터를 해제합니다.
		}
		CoUninitialize(); // COM 라이브러리를 종료합니다.
	}

}

