#include "pch.h"
#include "CSprite.h"

#include "CTexture.h"

#include "CAssetMgr.h"

CSprite::CSprite()
	: CAsset(ASSET_TYPE::SPRITE)
{
}

CSprite::~CSprite()
{
}

void CSprite::SetLeftTop(Vec2 _LeftTop)
{
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), (float)m_Atlas->Height());
	m_LeftTopUV = _LeftTop / AtlasResolution;
}

void CSprite::SetSlice(Vec2 _Slice)
{
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), (float)m_Atlas->Height());
	m_SliceUV = _Slice / AtlasResolution;
}

void CSprite::SetBackground(Vec2 _Background)
{
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), (float)m_Atlas->Height());
	m_BackgroundUV = _Background / AtlasResolution;
}

void CSprite::SetOffset(Vec2 _Offset)
{
	// 오프셋의 y방향 부호 반대로 해야 UV좌표계 상에서 방향이 맞음
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), -(float)m_Atlas->Height());
	m_OffsetUV = _Offset / AtlasResolution;
}

void CSprite::Create(Ptr<CTexture> _Atlas, Vec2 _LeftTopPixel, Vec2 _SlicePixel)
{
	m_Atlas = _Atlas;
	assert(m_Atlas);

	UINT width = m_Atlas->Width();
	UINT height = m_Atlas->Height();

	m_LeftTopUV = Vec2(_LeftTopPixel.x / (float)width, _LeftTopPixel.y / (float)height);
	m_SliceUV = Vec2(_SlicePixel.x / (float)width, _SlicePixel.y / (float)height);
}

int CSprite::Save(const wstring& _RelativePath)
{
	// 어디에 저장해뒀는지 알고 있는게 좋음
	SetRelativePath(_RelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentsPath();
	strFilePath += _RelativePath;

	std::filesystem::path dir_path = strFilePath;
	if (!std::filesystem::exists(dir_path.parent_path()))
		std::filesystem::create_directories(dir_path.parent_path());

	FILE* File = nullptr;
	_wfopen_s(&File, strFilePath.c_str(), L"wb");

	if (nullptr == File)
		return E_FAIL;

	// Atlas 참조정보 저장
	SaveAssetRef(m_Atlas, File);

	fwrite(&m_LeftTopUV, sizeof(Vec2), 1, File);
	fwrite(&m_SliceUV, sizeof(Vec2), 1, File);
	fwrite(&m_BackgroundUV, sizeof(Vec2), 1, File);
	fwrite(&m_OffsetUV, sizeof(Vec2), 1, File);

	fclose(File);

	return S_OK;
}

int CSprite::Load(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"rb");

	if (nullptr == File)
	{
		return E_FAIL;
	}

	// Atlas 키, 경로 불러오기
	LoadAssetRef(m_Atlas, File);

	fread(&m_LeftTopUV, sizeof(Vec2), 1, File);
	fread(&m_SliceUV, sizeof(Vec2), 1, File);
	fread(&m_BackgroundUV, sizeof(Vec2), 1, File);
	fread(&m_OffsetUV, sizeof(Vec2), 1, File);

	fclose(File);

	return S_OK;
}