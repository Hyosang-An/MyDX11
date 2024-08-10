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



	// 현재 선택된 FlipBook
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
			// 데이터를 드랍 받을 트리 이름 : ContentTree
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
			if (payload)
			{
				// payload의 Data는 TreeNode*를 가리키는 포인터
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


		// FlipBook의 Sprite 리스트
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

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) // 아이템이 활성화되어 있으나 호버되지 않은 경우
				{
					// 드래그 방향에 따른 아이템 이동
					int i_next = i + (ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y < 0.f ? -1 : 1);
					if (i_next >= 0 && i_next < spriteVecInFlipBook.size())
					{
						// 현재 인덱스와 다음 인덱스가 다르면 아이템 위치 교환
						if (i != i_next) {
							std::swap(spriteVecInFlipBook[i], spriteVecInFlipBook[i_next]);
							ImGui::ResetMouseDragDelta(); // 드래그 변화량 리셋
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

			// 방향키로 현재 선택된 Sprite 바꾸기
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


			// 선택한 flipBook의 offset 설정
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


	// 스프라이트 폴더 선택 버튼
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

	// 텍스트 크기 계산
	ImVec2 textSize = ImGui::CalcTextSize(strPath.c_str());

	// 패딩 및 여유 공간 추가
	float padding = 20.0f; // 텍스트 좌우의 여유 공간
	float inputTextWidth = textSize.x + padding;

	// 계산된 크기를 사용하여 다음 항목의 너비 설정
	ImGui::SetNextItemWidth(inputTextWidth);
	ImGui::InputText("##Selected Sprite Folder Path", (char*)strPath.c_str(), strPath.length(), ImGuiInputTextFlags_ReadOnly);


	// 폴더 내 sprite들을 리스트로 보여주고 추가 버튼 만들기
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

			// for문에 한번은 들어와야 loop를 종료할 수 있도록
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







	// 리스트 순서 바꾸기

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

	//int selected_index = 2; // 색상을 다르게 설정할 항목의 인덱스

	for (int n = 0; n < item_names.size(); n++)
	{
		const std::string& item = item_names[n];

		static int selected_index = -1;

		if (ImGui::Selectable(item.c_str(), selected_index == n)) // 항목 표시, 선택된 항목은 ImGuiCol_Header 색으로 표현됨.
		{
			selected_index = n;
		}

		if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) // 아이템이 활성화되어 있으나 호버되지 않은 경우
		{
			// 드래그 방향에 따른 아이템 이동
			int n_next = n + (ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y < 0.f ? -1 : 1);
			if (n_next >= 0 && n_next < item_names.size())
			{
				// 현재 인덱스와 다음 인덱스가 다르면 아이템 위치 교환
				if (n != n_next) {
					std::swap(item_names[n], item_names[n_next]);
					ImGui::ResetMouseDragDelta(); // 드래그 변화량 리셋
				}
			}
		}
	}









}


HRESULT FlipBookInspector::SelectSpriteFolderPath()
{
	// COM 라이브러리 초기화
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		// IFileOpenDialog 인터페이스 포인터 선언
		IFileOpenDialog* pFolderOpen = nullptr;

		// IFileOpenDialog 인스턴스 생성
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFolderOpen));
		if (SUCCEEDED(hr)) {
			// 폴더 선택 옵션 설정
			DWORD dwOptions;
			pFolderOpen->GetOptions(&dwOptions);
			pFolderOpen->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM); // 폴더 선택 옵션 추가

			// 대화상자 타이틀 설정
			pFolderOpen->SetTitle(L"Select Folder");

			// 마지막 경로를 초기 폴더로 설정
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

			// 대화상자 표시
			hr = pFolderOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				// 사용자가 선택한 폴더를 나타내는 IShellItem 포인터
				IShellItem* pItem = nullptr;
				hr = pFolderOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					// 선택한 폴더의 경로 얻기
					PWSTR pszFolderPath = nullptr;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
					if (SUCCEEDED(hr))
					{

						m_selectedSpriteDirectory = pszFolderPath;

						// 경로 사용 후 메모리 해제
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

						// .flip 파일을 처리하는 로직 추가
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
	// sprite 벡터 초기화
	m_vecSpriteInFolder.clear();

	WIN32_FIND_DATA tFindData = {};

	// 경로 끝에 백슬래시가 없으면 추가
	if (_folderPath.back() != L'\\' && _folderPath.back() != L'/')
	{
		_folderPath += L"\\";
	}

	// 경로에 맞는 파일을 탐색할 수 있는 커널 오브젝트 생성
	wstring strFindPath = _folderPath + L"*.sprite";
	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
	if (hFinder == INVALID_HANDLE_VALUE)
	{
		// 경로를 찾을 수 없는 경우 오류 메시지 출력 후 반환
		wprintf(L"Error: 경로를 찾을 수 없습니다. 경로: %s\n", _folderPath.c_str());
		return;
	}

	do
	{
		wstring strFindName = tFindData.cFileName;

		// sprite 파일이면 추가
		wstring fullPath = _folderPath + strFindName;
		wstring relativePath = CPathMgr::GetInst()->GetRelativePath(fullPath);
		Ptr<CSprite> pSprite = CAssetMgr::GetInst()->FindAsset<CSprite>(relativePath);

		if (pSprite != nullptr) {
			m_vecSpriteInFolder.push_back(pSprite);
		}
		else {
			// 찾을 수 없는 경우 경고 메시지
			wprintf(L"Warning: CSprite 객체를 찾을 수 없습니다. 경로: %s\n", fullPath.c_str());
		}

	} while (FindNextFile(hFinder, &tFindData) != 0);

	// 핸들 닫기
	FindClose(hFinder);
}

