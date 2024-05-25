#pragma once

#include "CPathMgr.h"

class CAsset;

class CAssetMgr
	: public CSingleton<CAssetMgr>
{
private:
	friend class CSingleton<CAssetMgr>;
	CAssetMgr();
	~CAssetMgr();

private:
	map<wstring, Ptr<CAsset>> m_mapAsset[(UINT)ASSET_TYPE::END];

public:
	void Init();

	template<typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

	Ptr<CAsset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);
	//void AddAsset(const wstring& _Key, CAsset* _Asset);

	template<typename T>
	Ptr<T> FindAsset(const wstring& _Key);

	template<typename T>
	void AddAsset(const wstring& _Key, Ptr<T> _Asset);

	// _Flags : D3D11_BIND_FLAG
	Ptr<CTexture> CreateTexture(wstring _strKey, UINT _Width, UINT _Height
		, DXGI_FORMAT _Format, UINT _Flags
		, D3D11_USAGE _Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT);

	Ptr<CTexture> CreateTexture(wstring _strKey, ComPtr<ID3D11Texture2D> _Tex2D);
};

template<typename T>
Ptr<T> CAssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{
	// 동일 키값 에셋이 있는지 확인
	Ptr<T> Asset = FindAsset<T>(_Key);

	if (nullptr != Asset)
	{
		return Asset;
	}

	// 동일 키값의 에셋이 없었으면
	wstring strFilePath = CPathMgr::GetInst()->GetContentsPath();
	strFilePath += _RelativePath;

	Asset = new T;

	// 로딩 실패 시 예외처리
	if (FAILED(Asset->Load(strFilePath)))
	{
		MessageBox(nullptr, L"알 수 없는 텍스쳐 포맷", L"텍스쳐 로딩 실패", MB_OK);
		return nullptr;
	}

	// Asset 이 자신의 키값과 경로를 알게 함
	Asset->m_Key = _Key;
	Asset->m_RelativePath = _RelativePath;

	// 맵에 등록
	ASSET_TYPE type = GetAssetType<T>();
	m_mapAsset[(UINT)type].insert(make_pair(_Key, Asset.Get()));

	// 로딩된 에셋 주소 반환
	return Asset;
}

template<typename T>
Ptr<T> CAssetMgr::FindAsset(const wstring& _Key)
{
	ASSET_TYPE Type = GetAssetType<T>();

	map<wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);

	if (iter == m_mapAsset[(UINT)Type].end())
		return nullptr;

	//Ptr<CAsset> pAsset;
	//Ptr<CMesh> pMesh = (CMesh*)pAsset.Get()

	return (T*)iter->second.Get();
}

template<typename T>
void CAssetMgr::AddAsset(const wstring& _Key, Ptr<T> _Asset)
{
	ASSET_TYPE Type = GetAssetType<T>();

	assert(!FindAsset(Type, _Key).Get());

	m_mapAsset[(UINT)Type].insert(make_pair(_Key, _Asset.Get()));
}