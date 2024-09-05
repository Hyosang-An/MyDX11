#include "pch.h"
#include "CTileMap.h"

#include "CTransform.h"
#include "CAssetMgr.h"

#include "CStructuredBuffer.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_AtlasMaxRow(1)
	, m_AtlasMaxCol(1)
	, m_structuredBuffer(nullptr)
{
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"TileMapMtrl"));

	m_structuredBuffer = new CStructuredBuffer;
	m_vecTileInfo.resize(m_Row * m_Col);
}

CTileMap::CTileMap(const CTileMap& _Origin) :
	CRenderComponent(_Origin),
	m_Row(_Origin.m_Row),
	m_Col(_Origin.m_Col),
	m_TileSize(_Origin.m_TileSize),
	m_TileAtlas(_Origin.m_TileAtlas),
	m_AtlasResolution(_Origin.m_AtlasResolution),
	m_AtlasTileResolution(_Origin.m_AtlasTileResolution),
	m_AtlasTileSliceUV(_Origin.m_AtlasTileSliceUV),
	m_AtlasMaxRow(_Origin.m_AtlasMaxRow),
	m_AtlasMaxCol(_Origin.m_AtlasMaxCol),
	m_isShowTileGrid(_Origin.m_isShowTileGrid),
	m_vecTileInfo(_Origin.m_vecTileInfo),
	m_structuredBuffer(nullptr)
{
	m_structuredBuffer = new CStructuredBuffer;
}
CTileMap::~CTileMap()
{
	delete m_structuredBuffer;
}

void CTileMap::Init()
{
	// 행, 렬 설정해서 구조화버퍼 크기 조정
	//SetRowCol(m_Row, m_Col);
	//SetTileSize(m_TileSize);


	// m_vecTileInfo와 구조화 버퍼 초기화 (ChangeTileMapSize를 여기서 해버리면 자식 타일맵 오브젝트를 로드할 때 문제가 생김)

	// 타일 개수
	UINT TileCount = m_Row * m_Col;

	m_vecTileInfo.resize(TileCount);

	// 타일정보를 전달받아서 t 레지스터에 전달시킬 구조화버퍼가 타일 전체 데이터 사이즈보다 작으면 리사이즈
	auto elementsize = m_structuredBuffer->GetElementSize();
	auto tileinfosize = sizeof(tTileInfo) * TileCount;
	if (m_structuredBuffer->GetBufferSize() < sizeof(tTileInfo) * TileCount)
	{
		m_structuredBuffer->Create(sizeof(tTileInfo), TileCount);
	}
}

void CTileMap::FinalTick()
{
}

void CTileMap::Render()
{
	// 타일의 정보를 구조화버퍼를 통해서 t 레지스터에 바인딩 시킨다.
	m_structuredBuffer->SetData(m_vecTileInfo.data(), sizeof(tTileInfo) * m_Row * m_Col);
	m_structuredBuffer->Binding(15);

	GetMaterial()->SetTexParam(TEX_0, m_TileAtlas);
	GetMaterial()->SetScalarParam(INT_1, m_AtlasMaxRow);
	GetMaterial()->SetScalarParam(INT_2, m_AtlasMaxCol);
	GetMaterial()->SetScalarParam(INT_3, m_isShowTileGrid);
	GetMaterial()->SetScalarParam(VEC2_0, m_AtlasTileSliceUV);
	GetMaterial()->SetScalarParam(VEC2_1, Vec2(m_Col, m_Row));
	GetMaterial()->Binding();
	Transform()->Binding();
	GetMesh()->Render();
}

void CTileMap::SetRowCol(UINT _Row, UINT _Col)
{
	UINT prevRow = m_Row;
	UINT prevCol = m_Col;
	m_Row = _Row;
	m_Col = _Col;
	
	// 자식 오브젝트가 있다면 원래의 Row, Col위치를 유지하도록
	auto vecChildren = GetOwner()->GetChildren();
	for (size_t i = 0; i < vecChildren.size(); i++)
	{
		Vec3 vRelativePos = vecChildren[i]->Transform()->GetRelativePos();
		Vec3 vRelativeScale = vecChildren[i]->Transform()->GetRelativeScale();

		int prevChildRowPos = -vRelativePos.y * prevRow;
		int prevChildColPos = vRelativePos.x * prevCol;

		
		Vec3 vNewRelativePos = Vec3((float)prevChildColPos / m_Col, -(float)prevChildRowPos / m_Row, vRelativePos.z);
		vecChildren[i]->Transform()->SetRelativePos(vNewRelativePos);

		Vec3 vNewRelativeScale = Vec3(vRelativeScale.x * (prevCol / (float)m_Col), vRelativeScale.y * (prevRow / (float)m_Row), vRelativeScale.z);
		vecChildren[i]->Transform()->SetRelativeScale(vNewRelativeScale);
	}

	ChangeTileMapSize();

	// 타일 개수
	UINT TileCount = m_Row * m_Col;

	// 타일 정보를 저장하는 벡터의 데이터 개수가 타일개수랑 다르면 리사이즈
	if (m_vecTileInfo.size() != TileCount)
	{
		vector<tTileInfo> prevVecTileInfo = m_vecTileInfo;

		m_vecTileInfo.clear();
		m_vecTileInfo.resize(TileCount);

		// 이전 데이터를 이전 행렬의 구조에 맞게 새로운 벡터에 복사
		for (size_t i = 0; i < prevRow; i++)
		{
			for (size_t j = 0; j < prevCol; j++)
			{
				if (i >= m_Row || j >= m_Col)
					continue;

				m_vecTileInfo[i * m_Col + j] = prevVecTileInfo[i * prevCol + j];
			}
		}
	}

	// 타일정보를 전달받아서 t 레지스터에 전달시킬 구조화버퍼가 타일 전체 데이터 사이즈보다 작으면 리사이즈
	auto elementsize = m_structuredBuffer->GetElementSize();
	auto tileinfosize = sizeof(tTileInfo) * TileCount;
	if (m_structuredBuffer->GetBufferSize() < sizeof(tTileInfo) * TileCount)
	{
		m_structuredBuffer->Create(sizeof(tTileInfo), TileCount);
	}

	//// 타일 종류별로 한개씩 순서대로 넣기
	//for (int i = 0; i < m_vecTileInfo.size(); i++)
	//{
	//	m_vecTileInfo[i].ImgIdx = i;
	//}
}

void CTileMap::SetTileSize(Vec2 _Size)
{
	m_TileSize = _Size;

	// 자식 타일맵이 있다면 그것도 같이 변경
	auto vecChildren = GetOwner()->GetChildren();
	for (size_t i = 0; i < vecChildren.size(); i++)
	{
		if (vecChildren[i]->TileMap() == nullptr)
			continue;

		vecChildren[i]->TileMap()->m_TileSize = _Size;
	}

	ChangeTileMapSize();
}

void CTileMap::ChangeTileMapSize()
{
	Vec2 vSize = m_TileSize * Vec2((float)m_Col, (float)m_Row);
	//Transform()->SetRelativeScale(vSize.x, vSize.y, 1.f);
	Transform()->SetWorldScale(Vec3(vSize.x, vSize.y, 1.f));
}

void CTileMap::SetAtlasTexture(Ptr<CTexture> _Atlas, Vec2 _tileResolution)
{
	m_TileAtlas = _Atlas;

	if (nullptr == m_TileAtlas)
		m_AtlasResolution = Vec2(0.f, 0.f);
	else
		m_AtlasResolution = Vec2((float)_Atlas->Width(), (float)_Atlas->Height());

	if (_tileResolution != Vec2(0, 0))
		SetAtlasTileResolution(_tileResolution);
	else
		SetAtlasTileResolution(m_AtlasTileResolution);
}

void CTileMap::SetAtlasTileResolution(Vec2 _TileResolution)
{
	m_AtlasTileResolution = _TileResolution;

	if (nullptr != m_TileAtlas)
	{
		m_AtlasTileSliceUV = m_AtlasTileResolution / m_AtlasResolution;

		m_AtlasMaxCol = int(m_AtlasResolution.x / m_AtlasTileResolution.x);
		m_AtlasMaxRow = int(m_AtlasResolution.y / m_AtlasTileResolution.y);
	}
}

void CTileMap::SaveToFile(FILE* _File)
{
	//// 부모클래스인 RenderComponent 의 SaveToFile 호출
	//SaveDataToFile(_File);

	//fwrite(&m_Col, sizeof(int), 1, _File);
	//fwrite(&m_Row, sizeof(int), 1, _File);

	//fwrite(&m_TileSize, sizeof(Vec2), 1, _File);
	//fwrite(&m_AtlasTileResolution, sizeof(Vec2), 1, _File);

	//for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	//{
	//	fwrite(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	//}

	//// 아틀라스 텍스쳐
	//SaveAssetRef(m_TileAtlas, _File);


	// =========================================

	SaveDataToFile(_File);

	fwrite(&m_Col, sizeof(int), 1, _File);
	fwrite(&m_Row, sizeof(int), 1, _File);

	fwrite(&m_TileSize, sizeof(Vec2), 1, _File);
	SaveAssetRef(m_TileAtlas, _File);
	fwrite(&m_AtlasTileResolution, sizeof(Vec2), 1, _File);
	fwrite(&m_AtlasTileSliceUV, sizeof(Vec2), 1, _File);

	fwrite(&m_AtlasMaxRow, sizeof(int), 1, _File);
	fwrite(&m_AtlasMaxCol, sizeof(int), 1, _File);

	// m_vecTileInfo 개수 저장
	size_t count = m_vecTileInfo.size();
	fwrite(&count, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		fwrite(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	}
}

void CTileMap::LoadFromFile(FILE* _File)
{
	//// 부모클래스인 RenderComponent 의 LoadFromFile 호출
	//LoadDataFromFile(_File);
	//
	//int col = 0;
	//int row = 0;

	//fread(&col, sizeof(int), 1, _File);
	//fread(&row, sizeof(int), 1, _File);

	//SetRowCol(row, col);

	//fread(&m_TileSize, sizeof(Vec2), 1, _File);
	//fread(&m_AtlasTileResolution, sizeof(Vec2), 1, _File);

	//SetTileSize(m_TileSize);

	//for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	//{
	//	fread(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	//}

	//// 아틀라스 텍스쳐
	//LoadAssetRef(m_TileAtlas, _File);
	//if (nullptr != m_TileAtlas)
	//{
	//	SetAtlasTexture(m_TileAtlas, m_AtlasTileResolution);
	//}


	// =========================================

	LoadDataFromFile(_File);

	fread(&m_Col, sizeof(int), 1, _File);
	fread(&m_Row, sizeof(int), 1, _File);

	fread(&m_TileSize, sizeof(Vec2), 1, _File);
	LoadAssetRef(m_TileAtlas, _File);
	fread(&m_AtlasTileResolution, sizeof(Vec2), 1, _File);
	fread(&m_AtlasTileSliceUV, sizeof(Vec2), 1, _File);

	fread(&m_AtlasMaxRow, sizeof(int), 1, _File);
	fread(&m_AtlasMaxCol, sizeof(int), 1, _File);

	// m_vecTileInfo 개수 로드
	size_t count = 0;
	fread(&count, sizeof(size_t), 1, _File);

	// m_vecTileInfo 리사이즈
	m_vecTileInfo.resize(count);

	// m_vecTileInfo 로드
	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		fread(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	}
}