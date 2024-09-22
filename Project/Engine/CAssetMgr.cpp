#include "pch.h"
#include "CAssetMgr.h"

#include "assets.h"


CAssetMgr::CAssetMgr()
{

}

CAssetMgr::~CAssetMgr()
{

}

void CAssetMgr::Tick()
{
	if (m_Changed)
		m_Changed = false;
}


Ptr<CAsset> CAssetMgr::FindAsset(ASSET_TYPE _Type, const wstring& _Key)
{
	map<wstring, Ptr<CAsset>>::iterator iter = m_arrAssetMap[(UINT)_Type].find(_Key);

	if (iter == m_arrAssetMap[(UINT)_Type].end())
	{
		return nullptr;
	}

	return iter->second;
}

Ptr<CTexture> CAssetMgr::CreateTexture(wstring _strKey, UINT _Width, UINT _Height
	, DXGI_FORMAT _Format, UINT _Flags, D3D11_USAGE _Usage)
{
	// �ߺ�Ű �˻�
	Ptr<CTexture> pTexture = FindAsset<CTexture>(_strKey);
	assert(!pTexture.Get());

	pTexture = new CTexture;
	if (FAILED(pTexture->Create(_Width, _Height, _Format, _Flags, _Usage)))
	{
		MessageBox(nullptr, L"�ؽ��� ���� ����", L"�ؽ��� ���� ����", MB_OK);
		return nullptr;
	}

	pTexture->m_Key = _strKey;
	m_arrAssetMap[(UINT)ASSET_TYPE::TEXTURE].insert(make_pair(_strKey, pTexture.Get()));

	return pTexture;
}

Ptr<CTexture> CAssetMgr::CreateTexture(wstring _strKey, ComPtr<ID3D11Texture2D> _Tex2D)
{
	// �ߺ�Ű �˻�
	Ptr<CTexture> pTexture = FindAsset<CTexture>(_strKey);
	assert(!pTexture.Get());

	pTexture = new CTexture;
	if (FAILED(pTexture->Create(_Tex2D)))
	{
		MessageBox(nullptr, L"�ؽ��� ���� ����", L"�ؽ��� ���� ����", MB_OK);
		return nullptr;
	}

	pTexture->m_Key = _strKey;
	m_arrAssetMap[(UINT)ASSET_TYPE::TEXTURE].insert(make_pair(_strKey, pTexture.Get()));

	return pTexture;
}


void CAssetMgr::GetAssetNames(ASSET_TYPE _Type, vector<string>& _vecOut)
{
	for (const auto& pair : m_arrAssetMap[(UINT)_Type])
	{
		_vecOut.push_back(string(pair.first.begin(), pair.first.end()));
	}
}

void CAssetMgr::DeleteAsset(ASSET_TYPE _Type, const wstring& _Key)
{
	map<wstring, Ptr<CAsset>>::iterator iter = m_arrAssetMap[(UINT)_Type].find(_Key);
	assert(iter != m_arrAssetMap[(UINT)_Type].end());
	m_arrAssetMap[(UINT)_Type].erase(iter);

	// Asset ���� �˸�
	CTaskMgr::GetInst()->AddTask(tTask{ ASSET_CHANGED });
}

void CAssetMgr::FindAssetName(const wstring& _FolderPath, const wstring& _Filter)
{
	WIN32_FIND_DATA tFindData = {};

	// ��ο� �´� ���� �� ������ Ž���� �� �ִ� Ŀ�� ������Ʈ ����
	wstring strFindPath = _FolderPath + _Filter;
	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
	if (hFinder == INVALID_HANDLE_VALUE)
	{
		assert(!"�ش� ��θ� ã�� �� �����ϴ�");
		return;
	}

	do
	{
		wstring strFindName = tFindData.cFileName;

		if (tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// ������ ��� ȣ��
			if (strFindName != L"." && strFindName != L"..")
			{
				FindAssetName(_FolderPath + strFindName + L"\\", _Filter);
			}
		}

		// �����̸� ��� �߰�
		else
		{
			wstring RelativePath = CPathMgr::GetInst()->GetRelativePath(_FolderPath + strFindName);
			m_vecAssetRelativePath.push_back(RelativePath);
		}
	} while (FindNextFile(hFinder, &tFindData));

	FindClose(hFinder);
}

void CAssetMgr::LoadAsset(const path& _Path)
{
	// ����θ� Ű ������ AssetMgr�� ���

	path ext = _Path.extension();

	if (ext == L".mesh")
		CAssetMgr::GetInst()->Load<CMesh>(_Path, _Path);
	//else if (ext == L".mdat")
		//CAssetMgr::GetInst()->Load<CMeshData>(_Path, _Path);
	else if (ext == L".mtrl")
		CAssetMgr::GetInst()->Load<CMaterial>(_Path, _Path);
	else if (ext == L".prefab")
		CAssetMgr::GetInst()->Load<CPrefab>(_Path, _Path);
	else if (ext == L".png" || ext == L".jpg" || ext == L".jpeg" || ext == L".bmp" || ext == L".dds" || ext == L".tga"
		|| ext == L".PNG" || ext == L".JPG" || ext == L".JPEG" || ext == L".BMP" || ext == L".DDS" || ext == L".TGA")
		CAssetMgr::GetInst()->Load<CTexture>(_Path, _Path);
	else if (ext == L".mp3" || ext == L".mp4" || ext == L".ogg" || ext == L".wav"
		|| ext == L".MP3" || ext == L".MP4" || ext == L".OGG" || ext == L".WAV")
		CAssetMgr::GetInst()->Load<CSound>(_Path, _Path);
	else if (ext == L".sprite")
		CAssetMgr::GetInst()->Load<CSprite>(_Path, _Path);
	else if (ext == L".flip")
		CAssetMgr::GetInst()->Load<CFlipBook>(_Path, _Path);
}
