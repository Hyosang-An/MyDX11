#pragma once

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

	Ptr<CAsset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);
	//void AddAsset(const wstring& _Key, CAsset* _Asset);

	template<typename T>
	Ptr<T> FindAsset(const wstring& _Key);

	template<typename T>
	void AddAsset(const wstring& _Key, Ptr<T> _Asset);
};


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