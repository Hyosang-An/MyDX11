#include "pch.h"
#include "FlipBookInspector.h"
#include <Engine/CAssetMgr.h>

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



	// 


	// Ư�� ���� ���� ��������Ʈ ��� �����ֱ�
	ImGui::Separator();
	if (ImGui::Button("Select Sprite Folder"))
	{
		if (SUCCEEDED(GetSpriteFolderPath()))
		{
			FindSprites(m_selectedDirectory);	
		}

		m_vecSprite;
	}

	ImGui::Text("Selected Folder"); 
	ImGui::SameLine();
	
	wstring relativePath = CPathMgr::GetInst()->GetRelativePath(m_selectedDirectory);
	auto strPath = string(relativePath.begin(), relativePath.end());
	
	// �ؽ�Ʈ ũ�� ���
	ImVec2 textSize = ImGui::CalcTextSize(strPath.c_str());

	// �е� �� ���� ���� �߰�
	float padding = 20.0f; // �ؽ�Ʈ �¿��� ���� ����
	float inputTextWidth = textSize.x + padding;

	// ���� ũ�⸦ ����Ͽ� ���� �׸��� �ʺ� ����
	ImGui::SetNextItemWidth(inputTextWidth);

	ImGui::InputText("##Selected Sprite Folder Path", (char*)strPath.c_str(), strPath.length(), ImGuiInputTextFlags_ReadOnly);


	// sprite���� ����Ʈ�� �����ְ� �߰� ��ư �����

	// �ڽ� ���� ����
	ImVec2 boxSize = ImVec2(0, 200);  // ���ϴ� �ڽ� ũ��
	ImGui::BeginChild("Sprite Box", boxSize, ImGuiChildFlags_Border, ImGuiWindowFlags_None);


	static float maxWidth = 0;
	static bool firstLoop = true;

	for (int i = 0; i < m_vecSprite.size(); i++)
	{
		bool b = false;

		Ptr<CSprite>& pSprite = m_vecSprite[i];
		wstring wstrKey = pSprite->GetKey();
		string strKey = string(wstrKey.begin(), wstrKey.end());

		string spriteName = path(strKey).stem().string();

		float selectableWidth = 0;
		if (firstLoop == false)
			selectableWidth = maxWidth;

		//ImGui::SetNextItemAllowOverlap();
		ImGui::Selectable(spriteName.c_str(), &b, ImGuiSelectableFlags_None, { selectableWidth, 0 });
		ImGui::SameLine();

		if (firstLoop)
		{
			auto pos = ImGui::GetCursorPos();

			maxWidth = max(pos.x, maxWidth);
		}

		if (ImGui::SmallButton("+"))
		{

		}

		if (i == m_vecSprite.size() - 1)
			firstLoop = false;
	}

	ImGui::EndChild();



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

	int highlight_index = 2; // ������ �ٸ��� ������ �׸��� �ε���

	for (int n = 0; n < item_names.size(); n++)
	{
		const std::string& item = item_names[n];

		bool isSelected = false;
		if (n == highlight_index)
			isSelected = true;

		ImGui::Selectable(item.c_str(), &isSelected); // �׸� ǥ��, ���õ� �׸��� ImGuiCol_Header ������ ǥ����.

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


HRESULT FlipBookInspector::GetSpriteFolderPath()
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
			if (m_selectedDirectory.empty())
				m_selectedDirectory = CPathMgr::GetInst()->GetContentsPath() + L"animation";
			hr = SHCreateItemFromParsingName(m_selectedDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
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

						m_selectedDirectory = pszFolderPath;

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


void FlipBookInspector::FindSprites(wstring _folderPath)
{
	// sprite ���� �ʱ�ȭ
	m_vecSprite.clear();

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
			m_vecSprite.push_back(pSprite);
		}
		else {
			// ã�� �� ���� ��� ��� �޽���
			wprintf(L"Warning: CSprite ��ü�� ã�� �� �����ϴ�. ���: %s\n", fullPath.c_str());
		}

	} while (FindNextFile(hFinder, &tFindData) != 0);

	// �ڵ� �ݱ�
	FindClose(hFinder);
}

