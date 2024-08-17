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

	// Ÿ�ϸ� ��Ʋ�� ����
	Ptr<CTexture> pAtlasTex = m_selectedTileMap->GetAtlasTexture();
	if (pAtlasTex != nullptr)
	{
		// �̹���	
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
					m_selectedTileImgIndex = -1;
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

	// TileResolution ����
	Vec2 atlasTileResolution = m_selectedTileMap->GetAtlasTileResolution();
	Vec2 prevAtlasTileResolution = atlasTileResolution;
	ImGui::Text("Tile Resolution");
	ImGui::SameLine(120);
	int tileResolution[2] = { (int)atlasTileResolution.x, (int)atlasTileResolution.y };
	ImGui::InputInt2("##TileResolution", tileResolution);
	tileResolution[0] = max(1, tileResolution[0]);
	tileResolution[1] = max(1, tileResolution[1]);
	if (prevAtlasTileResolution != Vec2((float)tileResolution[0], (float)tileResolution[1]))
		m_selectedTileMap->SetAtlasTileResolution(Vec2((float)tileResolution[0], (float)tileResolution[1]));


	// Ÿ�ϸ� Row, Col ����
	Vec2 vRowCol = m_selectedTileMap->GetRowCol();
	Vec2 prevRowCol = vRowCol;
	int rowcol[2] = { (int)vRowCol.x, (int)vRowCol.y };
	ImGui::Text("Row Col");
	ImGui::SameLine(120);
	ImGui::InputInt2("##RowCol", rowcol);
	rowcol[0] = max(1, rowcol[0]);
	rowcol[1] = max(1, rowcol[1]);
	if (prevRowCol != Vec2((float)rowcol[0], (float)rowcol[1]))
		m_selectedTileMap->SetRowCol(rowcol[0], rowcol[1]);

	// Ÿ�ϸ� Ÿ�� ũ�� ���� (World Scale)
	Vec2 vTileSize = m_selectedTileMap->GetTileSize();
	Vec2 prevTileSize = vTileSize;
	ImGui::Text("Tile Size");
	ImGui::SameLine(120);
	ImGui::DragFloat2("##Tile Size", vTileSize, 1.f, 0.0f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	vTileSize.x = max(0.f, vTileSize.x);
	vTileSize.y = max(0.f, vTileSize.y);
	if (prevTileSize != vTileSize)
		m_selectedTileMap->SetTileSize(vTileSize);


	// Ÿ�ϸ� Ÿ�� ����
	ImGui::Text("Tile Select");
	ImGui::SameLine(120);
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	int atlasMaxRow = m_selectedTileMap->GetAtlasMaxRowCol().x;
	int atlasMaxCol = m_selectedTileMap->GetAtlasMaxRowCol().y;
	int atlasMaxTileSize = atlasMaxRow * atlasMaxCol;
	if (m_selectedTileImgIndex == -1 || m_selectedTileImgIndex >= atlasMaxTileSize)
	{
		tint_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		int row = m_selectedTileImgIndex / atlasMaxCol;
		int col = m_selectedTileImgIndex % atlasMaxCol;
		uv_min = ImVec2((float)col / atlasMaxCol, (float)row / atlasMaxRow);
		uv_max = ImVec2((float)(col + 1) / atlasMaxCol, (float)(row + 1) / atlasMaxRow);
	}
	float tileAspectRatio = atlasTileResolution.x / atlasTileResolution.y;
	ImGui::Image(pAtlasTex->GetSRV().Get(), ImVec2(50, 50 / tileAspectRatio), uv_min, uv_max, tint_col, border_col);
	// Ÿ�� �̹��� ��Ŭ���� �˾� â ���� ��Ʋ���̹����� �����ֱ�
	if (ImGui::IsItemClicked(0))
		ImGui::OpenPopup("TileSelect");
	if (ImGui::BeginPopup("TileSelect"))
	{
		ImGui::Text("Tile Select");
		ImGui::Separator();
		ImVec2 selectedTileLeftTopPos = ImGui::GetCursorScreenPos(); // �ʱ�ȭ
		for (int i = 0; i < atlasMaxTileSize; ++i)
		{
			int row = i / atlasMaxCol;
			int col = i % atlasMaxCol;
			uv_min = ImVec2((float)col / atlasMaxCol, (float)row / atlasMaxRow);
			uv_max = ImVec2((float)(col + 1) / atlasMaxCol, (float)(row + 1) / atlasMaxRow);
			tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
			if (m_selectedTileImgIndex == i)
			{
				// ������ Ÿ�� �̹��� selectedTileLeftTopPos ����
				selectedTileLeftTopPos = ImGui::GetCursorScreenPos();
			}
			ImGui::Image(pAtlasTex->GetSRV().Get(), ImVec2(50, 50 / tileAspectRatio), uv_min, uv_max, tint_col, border_col);
			if (ImGui::IsItemClicked())
			{
				m_selectedTileImgIndex = i;
				ImGui::CloseCurrentPopup();
			}
			if ((i + 1) % atlasMaxCol != 0)
				ImGui::SameLine();
		}

		// ������ Ÿ�� �ֺ��� �ʷϻ� �׵θ��� �׸���
		if (m_selectedTileImgIndex != -1)
		{
			// ��� �β� ����
			float borderThickness = 4.f;

			// �簢�� ũ�� ����
			ImVec2 borderBoxSize = ImVec2(50, 50 / tileAspectRatio);
			border_col = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // �׵θ� ���� ����
			// �簢�� ��� �׸���
			ImGui::GetWindowDrawList()->AddRect(selectedTileLeftTopPos, ImVec2(selectedTileLeftTopPos.x + borderBoxSize.x, selectedTileLeftTopPos.y + borderBoxSize.y), ImGui::GetColorU32(border_col), 0.0f, 0, borderThickness);
		}
		ImGui::EndPopup();
	}


	//


	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	//SetChildSize(content_size);
}

void TileMapUI::SelectTileMapAtlasByDialog()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			// ���� ���͸� �̹��� ���Ͽ� �°� ����
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

						// �̹��� ������ ó���ϴ� ���� �߰�
						wstring relativePath = CPathMgr::GetInst()->GetRelativePath(pszFilePath);

						// �����ϴ� �̹��� Ȯ���� �˻� (��ҹ��� ���� ����)
						path filePath = pszFilePath;
						wstring extension = filePath.extension().wstring();
						std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower); // Ȯ���ڸ� �ҹ��ڷ� ��ȯ

						if (extension != L".png" && extension != L".jpg" && extension != L".jpeg" &&
							extension != L".bmp" && extension != L".tga" && extension != L".dds")
						{
							MessageBox(NULL, L"������ ������ �����Ǵ� �̹��� ������ �ƴմϴ�.", L"Error", MB_OK);
							return;
						}

						// �̹��� ������ ó���ϴ� ������ ���⿡ �߰�
						// ��: m_owner->SetTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));
						m_selectedTileMap->SetAtlasTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));
						m_selectedTileImgIndex = -1;

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
