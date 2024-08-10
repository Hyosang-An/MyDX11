#include "pch.h"
#include "FlipBookInspector.h"
#include <Engine/CAssetMgr.h>

#include "TreeUI.h"

FlipBookInspector::FlipBookInspector()
{
}

FlipBookInspector::~FlipBookInspector()
{
}


void FlipBookInspector::Init()
{

}

void FlipBookInspector::Update()
{
	ImGui::Text("This is the FlipBookInspector.");



	// ���� ���õ� FlipBook
	ImGui::BeginChild("Selected Sprite", ImVec2(0, 300), ImGuiChildFlags_None, ImGuiWindowFlags_None);
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));

		ImGui::Button("Current FlipBook");

		ImGui::PopStyleColor(3);

		Ptr<CFlipBook> flipBook = m_owner->GetFlipBook();
		string flipBookName;

		if (flipBook.Get())
		{
			flipBookName = string(flipBook->GetKey().begin(), flipBook->GetKey().end());
			flipBookName = path(flipBookName).stem().string();
		}

		ImGui::Text("FlipBook");
		ImGui::SameLine(100);
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputText("##FlipBookKey", (char*)flipBookName.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
		if (ImGui::BeginDragDropTarget())
		{
			// �����͸� ��� ���� Ʈ�� �̸� : ContentTree
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
			if (payload)
			{
				// payload�� Data�� TreeNode*�� ����Ű�� ������
				TreeNode** ppNode = (TreeNode**)payload->Data;
				TreeNode* pNode = *ppNode;

				Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
				if (ASSET_TYPE::FLIPBOOK == pAsset->GetAssetType())
				{
					m_owner->SetFlipBook((CFlipBook*)pAsset.Get());
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();

		if (ImGui::Button("..."))
		{
			SelectFlipBook();
		}


		// FlipBook�� Sprite ����Ʈ
		if (flipBook.Get())
		{
			ImGui::BeginChild("Sprite List", ImVec2(0, 200), ImGuiChildFlags_Border, ImGuiWindowFlags_None);

			vector<Ptr<CSprite>>& spriteVecInFlipBook = flipBook->GetSpriteVec();
			static float maxWidth = 0;
			static bool firstLoop = true;

			for (int i = 0; i < spriteVecInFlipBook.size(); i++)
			{
				Ptr<CSprite>& pSprite = spriteVecInFlipBook[i];
				wstring wstrKey = pSprite->GetKey();
				string strKey = string(wstrKey.begin(), wstrKey.end());

				string spriteName = path(strKey).stem().string();

				float selectableWidth = 0;
				if (firstLoop == false)
					selectableWidth = maxWidth;

				if (ImGui::Selectable(spriteName.c_str(), (m_selectedSpriteIndex == i), ImGuiSelectableFlags_None, { selectableWidth, 0 }))
				{
					m_selectedSpriteIndex = i;
				}

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) // �������� Ȱ��ȭ�Ǿ� ������ ȣ������ ���� ���
				{
					// �巡�� ���⿡ ���� ������ �̵�
					int i_next = i + (ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y < 0.f ? -1 : 1);
					if (i_next >= 0 && i_next < spriteVecInFlipBook.size())
					{
						// ���� �ε����� ���� �ε����� �ٸ��� ������ ��ġ ��ȯ
						if (i != i_next) {
							std::swap(spriteVecInFlipBook[i], spriteVecInFlipBook[i_next]);
							ImGui::ResetMouseDragDelta(); // �巡�� ��ȭ�� ����
						}
					}
				}

				ImGui::SameLine();

				if (firstLoop)
				{
					auto pos = ImGui::GetCursorPos();

					maxWidth = max(pos.x, maxWidth);
				}

				if (ImGui::SmallButton("-"))
				{

				}

				if (i == spriteVecInFlipBook.size() - 1)
					firstLoop = false;
			}

			// ����Ű�� ���� ���õ� Sprite �ٲٱ�
			if (ImGui::IsWindowFocused())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
				{
					if (m_selectedSpriteIndex > 0)
					{
						m_selectedSpriteIndex--;
					}
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
				{
					if (m_selectedSpriteIndex < spriteVecInFlipBook.size() - 1)
					{
						m_selectedSpriteIndex++;
					}
				}
			}
			ImGui::EndChild();


			// ������ flipBook�� offset ����
			if (m_selectedSpriteIndex != -1)
			{
				ImGui::Text("Offset");
				ImGui::SameLine();
				Vec2 offsetPixel = flipBook->GetSprite((int)m_selectedSpriteIndex)->GetOffsetPixel();
				ImGui::DragFloat2("##Sprite Offset FlipBookInspector", offsetPixel, 1, 0, 0, "%.0f");
				flipBook->GetSprite(m_selectedSpriteIndex)->SetOffsetPixel(offsetPixel);
			}
		}


	}
	ImGui::EndChild();


	//================================================================================================================================================================================
	//================================================================================================================================================================================


	// ��������Ʈ ���� ���� ��ư
	ImGui::Separator();
	if (ImGui::Button("Select Sprite Folder"))
	{
		if (SUCCEEDED(SelectSpriteFolderPath()))
		{
			FindSprites(m_selectedSpriteDirectory);
		}

		m_vecSpriteInFolder;
	}

	ImGui::Text("Selected Folder");
	ImGui::SameLine();

	wstring relativePath = CPathMgr::GetInst()->GetRelativePath(m_selectedSpriteDirectory);
	auto strPath = string(relativePath.begin(), relativePath.end());

	// �ؽ�Ʈ ũ�� ���
	ImVec2 textSize = ImGui::CalcTextSize(strPath.c_str());

	// �е� �� ���� ���� �߰�
	float padding = 20.0f; // �ؽ�Ʈ �¿��� ���� ����
	float inputTextWidth = textSize.x + padding;

	// ���� ũ�⸦ ����Ͽ� ���� �׸��� �ʺ� ����
	ImGui::SetNextItemWidth(inputTextWidth);
	ImGui::InputText("##Selected Sprite Folder Path", (char*)strPath.c_str(), strPath.length(), ImGuiInputTextFlags_ReadOnly);


	// ���� �� sprite���� ����Ʈ�� �����ְ� �߰� ��ư �����
	ImGui::BeginChild("Sprite List2", ImVec2(0, 200), ImGuiChildFlags_Border, ImGuiWindowFlags_None);
	{
		static float maxWidth = 0;
		static bool firstLoop = true;

		for (int i = 0; i < m_vecSpriteInFolder.size(); i++)
		{
			bool bSelected = false;

			Ptr<CSprite>& pSprite = m_vecSpriteInFolder[i];
			wstring wstrKey = pSprite->GetKey();
			string strKey = string(wstrKey.begin(), wstrKey.end());

			string spriteName = path(strKey).stem().string();

			float selectableWidth = 0;
			if (firstLoop == false)
				selectableWidth = maxWidth;

			//ImGui::SetNextItemAllowOverlap();
			ImGui::Selectable(spriteName.c_str(), &bSelected, ImGuiSelectableFlags_None, { selectableWidth, 0 });
			ImGui::SameLine();

			if (firstLoop)
			{
				auto pos = ImGui::GetCursorPos();

				maxWidth = max(pos.x, maxWidth);
			}

			if (ImGui::SmallButton("+"))
			{

			}

			// for���� �ѹ��� ���;� loop�� ������ �� �ֵ���
			if (i == m_vecSpriteInFolder.size() - 1)
				firstLoop = false;
		}
	}
	ImGui::EndChild();


	//static bool firstLoop = true;
	//static float maxWidth = 0;
	//vector<string> m_vecString = { "a", "ab", "abc" };
	//for (string& str : m_vecString)
	//{
	//	bool b = false;
	//	ImGui::Selectable(str.c_str(), &b, ImGuiSelectableFlags_None, { maxWidth, 0 });
	//	ImGui::SameLine();
	//	if (firstLoop)
	//	{
	//		auto pos = ImGui::GetCursorPos();

	//		maxWidth = fmax(pos.x, maxWidth);
	//	}

	//	if (ImGui::SmallButton("+"))
	//	{

	//	}
	//}

	//firstLoop = false;







	// ����Ʈ ���� �ٲٱ�

	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("We don't use the drag and drop api at all here!\n"
			"Instead we query when the item is held but not hovered, and order items accordingly.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}


	static std::vector<std::string> item_names = {
		"Item One", "Item Two", "Item Three", "Item Four", "Item Five"
	};

	//int selected_index = 2; // ������ �ٸ��� ������ �׸��� �ε���

	for (int n = 0; n < item_names.size(); n++)
	{
		const std::string& item = item_names[n];

		static int selected_index = -1;

		if (ImGui::Selectable(item.c_str(), selected_index == n)) // �׸� ǥ��, ���õ� �׸��� ImGuiCol_Header ������ ǥ����.
		{
			selected_index = n;
		}

		if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) // �������� Ȱ��ȭ�Ǿ� ������ ȣ������ ���� ���
		{
			// �巡�� ���⿡ ���� ������ �̵�
			int n_next = n + (ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y < 0.f ? -1 : 1);
			if (n_next >= 0 && n_next < item_names.size())
			{
				// ���� �ε����� ���� �ε����� �ٸ��� ������ ��ġ ��ȯ
				if (n != n_next) {
					std::swap(item_names[n], item_names[n_next]);
					ImGui::ResetMouseDragDelta(); // �巡�� ��ȭ�� ����
				}
			}
		}
	}









}


HRESULT FlipBookInspector::SelectSpriteFolderPath()
{
	// COM ���̺귯�� �ʱ�ȭ
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		// IFileOpenDialog �������̽� ������ ����
		IFileOpenDialog* pFolderOpen = nullptr;

		// IFileOpenDialog �ν��Ͻ� ����
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFolderOpen));
		if (SUCCEEDED(hr)) {
			// ���� ���� �ɼ� ����
			DWORD dwOptions;
			pFolderOpen->GetOptions(&dwOptions);
			pFolderOpen->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM); // ���� ���� �ɼ� �߰�

			// ��ȭ���� Ÿ��Ʋ ����
			pFolderOpen->SetTitle(L"Select Folder");

			// ������ ��θ� �ʱ� ������ ����
			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_selectedSpriteDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentsPath() + L"animation";
			else
				defaultDirectory = m_selectedSpriteDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFolderOpen->SetFolder(psiFolder);
				psiFolder->Release();
			}

			// ��ȭ���� ǥ��
			hr = pFolderOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				// ����ڰ� ������ ������ ��Ÿ���� IShellItem ������
				IShellItem* pItem = nullptr;
				hr = pFolderOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					// ������ ������ ��� ���
					PWSTR pszFolderPath = nullptr;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
					if (SUCCEEDED(hr))
					{

						m_selectedSpriteDirectory = pszFolderPath;

						// ��� ��� �� �޸� ����
						CoTaskMemFree(pszFolderPath);
					}
					pItem->Release();
				}
			}

			pFolderOpen->Release();
		}
		CoUninitialize();
	}

	return hr;
}

void FlipBookInspector::SelectFlipBook()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"Flip Files", L"*.flip" },
				{ L"All Files", L"*.*" }
			};
			pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
			pFileOpen->SetFileTypeIndex(1);
			pFileOpen->SetTitle(L"Load Flip File");

			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastFlipBookDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentsPath() + L"animation";
			else
				defaultDirectory = m_lastFlipBookDirectory;
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

						// .flip ������ ó���ϴ� ���� �߰�
						m_owner->SetFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(CPathMgr::GetInst()->GetRelativePath(pszFilePath)));

						path filePath = pszFilePath;
						m_lastFlipBookDirectory = filePath.parent_path().wstring();

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


void FlipBookInspector::FindSprites(wstring _folderPath)
{
	// sprite ���� �ʱ�ȭ
	m_vecSpriteInFolder.clear();

	WIN32_FIND_DATA tFindData = {};

	// ��� ���� �齽���ð� ������ �߰�
	if (_folderPath.back() != L'\\' && _folderPath.back() != L'/')
	{
		_folderPath += L"\\";
	}

	// ��ο� �´� ������ Ž���� �� �ִ� Ŀ�� ������Ʈ ����
	wstring strFindPath = _folderPath + L"*.sprite";
	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
	if (hFinder == INVALID_HANDLE_VALUE)
	{
		// ��θ� ã�� �� ���� ��� ���� �޽��� ��� �� ��ȯ
		wprintf(L"Error: ��θ� ã�� �� �����ϴ�. ���: %s\n", _folderPath.c_str());
		return;
	}

	do
	{
		wstring strFindName = tFindData.cFileName;

		// sprite �����̸� �߰�
		wstring fullPath = _folderPath + strFindName;
		wstring relativePath = CPathMgr::GetInst()->GetRelativePath(fullPath);
		Ptr<CSprite> pSprite = CAssetMgr::GetInst()->FindAsset<CSprite>(relativePath);

		if (pSprite != nullptr) {
			m_vecSpriteInFolder.push_back(pSprite);
		}
		else {
			// ã�� �� ���� ��� ��� �޽���
			wprintf(L"Warning: CSprite ��ü�� ã�� �� �����ϴ�. ���: %s\n", fullPath.c_str());
		}

	} while (FindNextFile(hFinder, &tFindData) != 0);

	// �ڵ� �ݱ�
	FindClose(hFinder);
}

