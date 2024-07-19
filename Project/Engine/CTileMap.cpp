#include "pch.h"
#include "CTileMap.h"

#include "CTransform.h"
#include "CAssetMgr.h"

#include "CStructuredBuffer.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_Row(1)
	, m_Col(1)
	, m_AtlasMaxRow(0)
	, m_AtlasMaxCol(0)
	, m_structuredBuffer(nullptr)
{
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"TileMapMtrl"));

	m_structuredBuffer = new CStructuredBuffer;
}

CTileMap::~CTileMap()
{
	delete m_structuredBuffer;
}

void CTileMap::FinalTick()
{
}

void CTileMap::Render()
{
	for (int i = 0; i < m_vecTileInfo.size(); i++)
	{
		m_vecTileInfo[i].ImgIdx = i;
	}

	// Ÿ���� ������ ����ȭ���۸� ���ؼ� t �������Ϳ� ���ε� ��Ų��.
	m_structuredBuffer->SetData(m_vecTileInfo.data(), sizeof(tTileInfo) * m_Row * m_Col);
	m_structuredBuffer->Binding(15);

	GetMaterial()->SetTexParam(TEX_0, m_TileAtlas);
	GetMaterial()->SetScalarParam(INT_1, m_AtlasMaxRow);
	GetMaterial()->SetScalarParam(INT_2, m_AtlasMaxCol);
	GetMaterial()->SetScalarParam(VEC2_1, Vec2(m_Col, m_Row));
	GetMaterial()->SetScalarParam(VEC2_0, m_AtlasTileSliceUV);
	GetMaterial()->Binding();
	Transform()->Binding();
	GetMesh()->Render();
}

void CTileMap::SetRowCol(UINT _Row, UINT _Col)
{
	m_Row = _Row;
	m_Col = _Col;

	ChangeTileMapSize();

	// Ÿ�� ����
	UINT TileCount = m_Row * m_Col;

	// Ÿ�� ������ �����ϴ� ���Ͱ� Ÿ�ϰ������� ����� ������ ��������
	if (m_vecTileInfo.size() < TileCount)
	{
		m_vecTileInfo.resize(TileCount);
	}

	// Ÿ�������� ���޹޾Ƽ� t �������Ϳ� ���޽�ų ����ȭ���۰� Ÿ�� ��ü ������ ������� ������ ��������
	if (m_structuredBuffer->GetElementSize() < sizeof(tTileInfo) * TileCount)
	{
		m_structuredBuffer->Create(sizeof(tTileInfo), TileCount);
	}

	//for (int i = 0; i < m_vecTileInfo.size(); i++)
	//{
	//	m_vecTileInfo[i].ImgIdx = i;
	//}
}

void CTileMap::SetTileSize(Vec2 _Size)
{
	m_TileSize = _Size;

	ChangeTileMapSize();
}

void CTileMap::ChangeTileMapSize()
{
	Vec2 vSize = m_TileSize * Vec2((float)m_Col, (float)m_Row);
	Transform()->SetRelativeScale(vSize.x, vSize.y, 1.f);
}

void CTileMap::SetAtlasTexture(Ptr<CTexture> _Atlas)
{
	m_TileAtlas = _Atlas;
	m_AtlasResolution = Vec2((float)_Atlas->Width(), (float)_Atlas->Height());

	if (m_AtlasTileResolution != Vec2(0, 0))
		SetAtlasTileResolution(m_AtlasTileResolution);
}

void CTileMap::SetAtlasTileResolution(Vec2 _TileSize)
{
	m_AtlasTileResolution = _TileSize;

	if (nullptr != m_TileAtlas)
	{
		m_AtlasTileSliceUV = m_AtlasTileResolution / m_AtlasResolution;

		m_AtlasMaxCol = int(m_AtlasResolution.x / m_AtlasTileResolution.x);
		m_AtlasMaxRow = int(m_AtlasResolution.y / m_AtlasTileResolution.y);
	}
}