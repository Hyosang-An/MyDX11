#include "pch.h"
#include "CFlipBookComponent.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CTimeMgr.h"
#include "CFlipBook.h"

CFlipBookComponent::CFlipBookComponent()
	: CComponent(COMPONENT_TYPE::FLIPBOOKCOMPONENT)
	, m_CurFlipBook(nullptr)
	, m_CurFrmIdx(0)
{
}

CFlipBookComponent::CFlipBookComponent(CFlipBookComponent& _Origin)
	: CComponent(_Origin)
	, m_vecFlipBook(_Origin.m_vecFlipBook)
	, m_CurFlipBook(_Origin.m_CurFlipBook)
	, m_CurFrmIdx(0)
	, m_FPS(_Origin.m_FPS)
	, m_AccTime(0.f)
	, m_Repeat(_Origin.m_Repeat)
	, m_Finish(false)
{
	if (nullptr != m_CurFlipBook)
	{
		int FlipBookIdx = 0;
		for (; FlipBookIdx < (int)m_vecFlipBook.size(); ++FlipBookIdx)
		{
			if (m_CurFlipBook == m_vecFlipBook[FlipBookIdx])
				break;
		}

		Play(FlipBookIdx, m_FPS, m_Repeat);
	}
}

CFlipBookComponent::~CFlipBookComponent()
{
}

void CFlipBookComponent::FinalTick()
{
	if (m_Finish)
	{
		if (false == m_Repeat)
			return;

		m_CurFrmIdx = 0;
		m_Finish = false;
	}

	if (nullptr != m_CurFlipBook)
	{
		float MaxTime = 1.f / m_FPS;

		m_AccTime += DT;

		if (MaxTime < m_AccTime)
		{
			m_AccTime -= MaxTime;
			++m_CurFrmIdx;

			if (m_CurFlipBook->GetMaxFrameCount() <= m_CurFrmIdx)
			{
				--m_CurFrmIdx;
				m_Finish = true;
			}
		}

		m_CurFrmSprite = m_CurFlipBook->GetSprite(m_CurFrmIdx);
	}
}

void CFlipBookComponent::AddFlipBook(int _Idx, Ptr<CFlipBook> _Flipbook)
{
	if (m_vecFlipBook.size() <= _Idx)
	{
		m_vecFlipBook.resize(_Idx + 1);
	}

	m_vecFlipBook[_Idx] = _Flipbook;
}

Ptr<CFlipBook> CFlipBookComponent::FindFlipBook(const wstring& _Key)
{
	for (size_t i = 0; i < m_vecFlipBook.size(); ++i)
	{
		if (m_vecFlipBook[i]->GetKey() == _Key)
			return m_vecFlipBook[i];
	}

	return nullptr;
}

void CFlipBookComponent::Play(int _FliBookIdx, float _FPS, bool _Repeat)
{
	m_CurFlipBook = m_vecFlipBook[_FliBookIdx];

	if (nullptr == m_CurFlipBook)
	{
		return;
	}

	m_CurFrmIdx = 0;
	m_AccTime = 0.f;
	m_FPS = _FPS;
	m_Repeat = _Repeat;
}

void CFlipBookComponent::Reset()
{
	m_CurFrmIdx = 0;
	m_AccTime = 0.f;
	m_Finish = false;
}

void CFlipBookComponent::Binding()
{
	if (nullptr != m_CurFrmSprite)
	{
		// 현재 표시해야하는 Sprite 의 정보를 Sprite 전용 상수버퍼를 통해서 GPU 에 전달
		tSpriteInfo tInfo = {};

		tInfo.LeftTopInAtlasUV = m_CurFrmSprite->GetLeftTopInAtlasUV();
		tInfo.SliceSizeInAtlasUV = m_CurFrmSprite->GetSliceSizeInAtlasUV();
		tInfo.BackGroundSizeInAtlasUV = m_CurFrmSprite->GetBackgroundSizeInAtlasUV();
		tInfo.OffsetUV = m_CurFrmSprite->GetOffsetUV();
		tInfo.UseFlipbook = 1;

		static CConstBuffer* CB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::SPRITE);

		CB->SetData(&tInfo);
		CB->Binding();

		// FlipBook Sprite 아틀라스 텍스쳐 전용 레지스터번호 t10 에 바인딩
		Ptr<CTexture> pAtlas = m_CurFrmSprite->GetAtlasTexture();
		pAtlas->Binding(10);
	}
	else
	{
		Clear();
	}
}

void CFlipBookComponent::Clear()
{
	tSpriteInfo tInfo = {};
	static CConstBuffer* CB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::SPRITE);
	CB->SetData(&tInfo);
	CB->Binding();
}
