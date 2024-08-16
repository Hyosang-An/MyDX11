#include "pch.h"
#include "TileMapUI.h"

#include "TreeUI.h"

#include "Engine/CTileMap.h"
#include "Engine/CPathMgr.h"
#include "Engine/CAssetMgr.h"

TileMapUI::TileMapUI()
	: ComponentUI(COMPONENT_TYPE::TILEMAP)
{
}

TileMapUI::~TileMapUI()
{
}

void TileMapUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();



	Title();

	m_selectedTileMap = GetTargetObject()->TileMap();

	if (m_selectedTileMap == nullptr)
		return;

	// 타일맵 아틀라스 설정
	Ptr<CTexture> pAtlasTex = m_selectedTileMap->GetAtlasTexture();
	if (pAtlasTex != nullptr)
	{
		// 이미지	
		ImGui::Text("Atlas");
		ImGui::SameLine(100);
		ImVec2 uv_min = ImVec2(0.0f, 0.0f);
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);

		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

		float aspectRatio = (float)pAtlasTex->Width() / (float)pAtlasTex->Height();

		ImGui::Image(pAtlasTex->GetSRV().Get(), ImVec2(150, 150 / aspectRatio), uv_min, uv_max, tint_col, border_col);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
			if (payload)
			{
				TreeNode** ppNode = (TreeNode**)payload->Data;
				TreeNode* pNode = *ppNode;

				Ptr<CAsset> pAsset = reinterpret_cast<CAsset*>(pNode->GetData());
				if (ASSET_TYPE::TEXTURE == pAsset->GetAssetType())
				{
					pAtlasTex = ((CTexture*)pAsset.Get());
					m_selectedTileMap->SetAtlasTexture(pAtlasTex);
				}
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::Button("...##Tile AtlasTexture"))
		{ 
			SelectTileMapAtlasByDialog();
		}
	}

	// TileResolution 설정
	Vec2 atlasTileResolution = m_selectedTileMap->GetAtlasTileResolution();
	ImGui::Text("Tile Resolution");
	ImGui::SameLine(120);
	int tileResolution[2] = { (int)atlasTileResolution.x, (int)atlasTileResolution.y };
	ImGui::InputInt2("##TileResolution", tileResolution);
	tileResolution[0] = max(1, tileResolution[0]);
	tileResolution[1] = max(1, tileResolution[1]);
	m_selectedTileMap->SetAtlasTileResolution(Vec2((float)tileResolution[0], (float)tileResolution[1]));


	// 타일맵 Row, Col 설정
	Vec2 vRowCol = m_selectedTileMap->GetRowCol();
	int rowcol[2] = { (int)vRowCol.x, (int)vRowCol.y };
	ImGui::Text("Row Col");
	ImGui::SameLine(120);
	ImGui::InputInt2("##RowCol", rowcol);
	rowcol[0] = max(1, rowcol[0]);
	rowcol[1] = max(1, rowcol[1]);
	m_selectedTileMap->SetRowCol(rowcol[0], rowcol[1]);

	// 타일맵 타일 크기 설정 (World Scale)
	Vec2 vTileSize = m_selectedTileMap->GetTileSize();
	ImGui::Text("Tile Size");
	ImGui::SameLine(120);
	ImGui::DragFloat2("##Tile Size", vTileSize, 1.f, 0.0f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	vTileSize.x = max(0.f, vTileSize.x);
	vTileSize.y = max(0.f, vTileSize.y);
	m_selectedTileMap->SetTileSize(vTileSize);


	

	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}

void TileMapUI::SelectTileMapAtlasByDialog()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			// 파일 필터를 이미지 파일에 맞게 변경
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"Image Files", L"*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.dds" },
				{ L"All Files", L"*.*" }
			};
			pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
			pFileOpen->SetFileTypeIndex(1);
			pFileOpen->SetTitle(L"Load Image File");

			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastTextureDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"texture";
			else
				defaultDirectory = m_lastTextureDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFileOpen->SetFolder(psiFolder);
				psiFolder->Release();
			}

			hr = pFileOpen->Show(NULL);

			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) {

						// 이미지 파일을 처리하는 로직 추가
						wstring relativePath = CPathMgr::GetInst()->GetRelativePath(pszFilePath);

						// 지원하는 이미지 확장자 검사 (대소문자 구분 없이)
						path filePath = pszFilePath;
						wstring extension = filePath.extension().wstring();
						std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower); // 확장자를 소문자로 변환

						if (extension != L".png" && extension != L".jpg" && extension != L".jpeg" &&
							extension != L".bmp" && extension != L".tga" && extension != L".dds")
						{
							MessageBox(NULL, L"선택한 파일이 지원되는 이미지 파일이 아닙니다.", L"Error", MB_OK);
							return;
						}

						// 이미지 파일을 처리하는 로직을 여기에 추가
						// 예: m_owner->SetTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));
						m_selectedTileMap->SetAtlasTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));

						m_lastTextureDirectory = filePath.parent_path().wstring();

						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
}
