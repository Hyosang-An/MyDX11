#pragma once

template<typename T, int _Size>
void Delete_Array(T(&_arr)[_Size])
{
	for (int i = 0; i < _Size; ++i)
	{
		if (nullptr != _arr[i])
			delete _arr[i];
	}
}



template<typename T>
void Delete_Vec(vector<T*>& _vec)
{
	for (size_t i = 0; i < _vec.size(); ++i)
	{
		if (nullptr != _vec[i])
			delete _vec[i];
	}
	_vec.clear();
}


template<typename T1, typename T2>
void Delete_Map(map<T1, T2>& _map)
{
	for (const auto& pair : _map)
	{
		if (nullptr != pair.second)
			delete pair.second;
	}

	_map.clear();
}

#include "assets.h"
template<typename T>
ASSET_TYPE GetAssetType()
{
	if constexpr (std::is_same_v<T, CMesh>)
		return ASSET_TYPE::MESH;
	//if constexpr (IsSame<T, CMeshData>)
	//	return ASSET_TYPE::MESH_DATA;
	//if constexpr (IsSame<T, CMaterial>)
	//	return ASSET_TYPE::MATERIAL;
	//if constexpr (IsSame<T, CPrefab>)
	//	return ASSET_TYPE::PREFAB;
	//if constexpr (IsSame<T, CTexture>)
	//	return ASSET_TYPE::TEXTURE;
	//if constexpr (IsSame<T, CSound>)
	//	return ASSET_TYPE::SOUND;
	if constexpr (std::is_same_v<T, CGraphicShader>)
		return ASSET_TYPE::GRAPHIC_SHADER;
	if constexpr (std::is_same_v<T, CComputeShader>)
		return ASSET_TYPE::COMPUTE_SHADER;
}