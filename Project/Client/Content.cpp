#include "pch.h"
#include "Content.h"

#include "CEditorMgr.h"
#include "Inspector.h"
#include "TreeUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>
#include <Engine/CTaskMgr.h>

Content::Content()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("ContentTree");
	m_Tree->SetOwnerUI(this);
	AddChild(m_Tree);

	// Ʈ�� �ɼ� ����
	m_Tree->ShowRoot(false);	// ��Ʈ ������ �ʱ�
	m_Tree->UseDrag(true);		// Tree Drag ��� ON
	m_Tree->ShowNameOnly(true); // ����� �̸����� ��� �� Ȯ���ڴ� �����ϰ� �����̸��� ���̱�

	m_Tree->AddSelectDelegate((DELEGATE_1)&Content::AssetSelected);

	// Asset ���¸� Content �� TreeUI �� �ݿ�
	RenewContent();
}

Content::~Content()
{
}

void Content::Init()
{
	Reload();
}

void Content::Update()
{
	// AssetMgr �� �������� �����, Ʈ�� ������ �籸���Ѵ�.
	if (CAssetMgr::GetInst()->IsChanged())
	{
		RenewContent();
	}
}

void Content::RenewContent()
{
	// Ʈ���� ������ ���� ����
	m_Tree->Clear();

	// �θ��带 �������� ���� == ��Ʈ��� �Է�
	TreeNode* pRoot = m_Tree->AddNode(nullptr, "Root");

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		TreeNode* pNode = m_Tree->AddNode(pRoot, ToString((ASSET_TYPE)i));
		pNode->SetFrame(true);

		const map<wstring, Ptr<CAsset>>& assetMap = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);

		for (const auto& pair : assetMap)
		{
			// Asset�� �ּҰ��� Data�� �־��ش�. �̶�, pair�� const & �̹Ƿ�, pair.second.Get() �� const �Լ����� �Ѵ�.
			m_Tree->AddNode(pNode, string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second.Get());
		}
	}
}

void Content::AssetSelected(DWORD_PTR _Param)
{
	// _Param�� SelectedNode�̴�.
	TreeNode* pNode = (TreeNode*)_Param;
	if (pNode->IsFrame())
		return;

	Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();

	Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
	pInspector->SetTargetAsset(pAsset);

	// GUI������ ��Ŀ�� ����
	ImGui::SetWindowFocus(nullptr);
}

void Content::Reload()
{
	// Content ������ �ִ� �������ϵ��� ��θ� ���� �˾Ƴ���.
	wstring ContentPath = CPathMgr::GetInst()->GetContentsPath();
	FindAssetName(ContentPath, L"*.*");

	// �˾Ƴ� ���� ���ϵ��� ��θ� ���ؼ� Asset ���� ����θ� Key������ AssetMgr �� �ε��Ѵ�.
	for (size_t i = 0; i < m_vecAssetPath.size(); ++i)
	{
		LoadAsset(m_vecAssetPath[i]);
	}


	// ���� �Ŵ������� �ε��Ǿ�������, content �������� ���� ������ AssetMgr ���� �����ϱ�
	// �ε��� ���¿� �ش��ϴ� ���� ������ Content ������ �ִ��� Exist üũ
	wstring strContentPath = CPathMgr::GetInst()->GetContentsPath();

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);
		for (const auto &pair : mapAsset)
		{
			// �������� ������ ������ ���� ���������� ���⶧���� �Ѿ��.
			if (pair.second->IsEngineAsset())
				continue;


			wstring strRelativePath = pair.second->GetRelativePath();

			if (strRelativePath == L"" || false == std::filesystem::exists(strContentPath + strRelativePath))
			{
				if (pair.second->GetRefCount() <= 1)
				{
					// // AssetMgr���� ���� ��û
					tTask deleteAssetTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get() };
					CTaskMgr::GetInst()->AddTask(deleteAssetTask);
				}
				else
				{
					wstring msg = pair.second->GetName() + L"�ٸ� ������ �����ǰ� ���� �� �ֽ��ϴ�.";
					int ret = MessageBox(nullptr, msg.c_str(), L"���� ���� ���", MB_YESNO);
					if (ret == IDYES)
					{
						
						CTaskMgr::GetInst()->AddTask(tTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get(), });
					}
				}
			}
		}
	}
}

void Content::FindAssetName(const wstring& _FolderPath, const wstring& _Filter)
{
	WIN32_FIND_DATA tFindData = {};

	// ��ο� �´� ���� �� ������ Ž���� �� �ִ� Ŀ�ο�����Ʈ ����
	wstring strFindPath = _FolderPath + _Filter;
	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
	assert(hFinder != INVALID_HANDLE_VALUE);

	// Ž�� Ŀ�ο�����Ʈ�� �̿��ؼ� ���� ������ �ݺ��ؼ� ã�Ƴ���
	while (FindNextFile(hFinder, &tFindData))
	{
		wstring strFindName = tFindData.cFileName;

		if (tFindData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strFindName == L"..")
				continue;

			FindAssetName(_FolderPath + strFindName + L"\\", _Filter);
		}
		else
		{
			wstring RelativePath = CPathMgr::GetInst()->GetRelativePath(_FolderPath + strFindName);
			m_vecAssetPath.push_back(RelativePath);
		}
	}

	FindClose(hFinder);
}

void Content::LoadAsset(const path& _Path)
{
	// ����θ� Ű ������ AssetMgr�� ���

	path ext = _Path.extension();

	if (ext == L".mesh")
		CAssetMgr::GetInst()->Load<CMesh>(_Path, _Path);
	//else if (ext == L".mdat")
		//CAssetMgr::GetInst()->Load<CMeshData>(_Path, _Path);
	else if (ext == L".mtrl")
		CAssetMgr::GetInst()->Load<CMaterial>(_Path, _Path);
	else if (ext == L".pref")
		CAssetMgr::GetInst()->Load<CPrefab>(_Path, _Path);
	else if (ext == L".png" || ext == L".jpg" || ext == L".jpeg" || ext == L".bmp" || ext == L".dds" || ext == L".tga"
		|| ext == L".PNG" || ext == L".JPG" || ext == L".JPEG" || ext == L".BMP" || ext == L".DDS" || ext == L".TGA")
		CAssetMgr::GetInst()->Load<CTexture>(_Path, _Path);
	//else if (ext == L".mp3" || ext == L".mp4" || ext == L".ogg" || ext == L".wav" 
	//	|| ext == L".MP3" || ext == L".MP4" || ext == L".OGG" || ext == L".WAV")
	//	CAssetMgr::GetInst()->Load<CSound>(_Path, _Path);
	else if (ext == L".sprite")
		CAssetMgr::GetInst()->Load<CSprite>(_Path, _Path);
	else if (ext == L".flip")
		CAssetMgr::GetInst()->Load<CFlipBook>(_Path, _Path);
}