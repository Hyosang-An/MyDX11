#include "pch.h"
#include "CBadelineScript.h"

#include "Engine/CGameObject.h"
#include "Engine/CFlipBookComponent.h"
#include "Engine/CFlipBook.h"

CBadelineScript::CBadelineScript() :
	CScript((UINT)SCRIPT_TYPE::BADELINESCRIPT)
{
}

CBadelineScript::CBadelineScript(const CBadelineScript& _Other) :
	CScript((UINT)SCRIPT_TYPE::BADELINESCRIPT)
{
}

CBadelineScript::~CBadelineScript()
{
}

void CBadelineScript::Begin()
{
	// FlipBook 추가
	CFlipBookComponent* pFlipBookComponent = FlipBookComponent();
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Spawn\\Spawn.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Idle\\Idle.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Run\\Run.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Jump\\Jump.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Dash\\Dash.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\DreamDash\\DreamDash.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Climb\\Climb.flip"));

	pFlipBookComponent->Play(L"Spawn", false); // 스폰 애니메이션은 약 1.25초

	// 현재 시점의 플레이어 위치를 저장
	assert(m_Player != nullptr);
	m_TrackingStartPos = m_Player->Transform()->GetWorldPos();

	// Badeline의 위치를 m_TrackingStartPos로 이동시킬 속도 계산
	Vec3 vDir = m_TrackingStartPos - Transform()->GetWorldPos();
	m_Vel_MoveToTrackingStartPos = vDir / 1.75f; // 1.75초에 걸쳐 이동
}

void CBadelineScript::Tick()
{
	// 매 프레임의 플레이어의 위치 및 기타 데이터를 queue에 저장
	tPlayerTrail playerTrail;
	playerTrail.Pos = m_Player->Transform()->GetWorldPos();
	playerTrail.State = m_Player->GetScript<CPlayerScript>()->GetCurState();
	playerTrail.FacingRight = m_Player->GetScript<CPlayerScript>()->IsFacingRight();
	playerTrail.SpriteIdx = m_Player->FlipBookComponent()->GetCurFrmIdx();
	playerTrail.Time = CTimeMgr::GetInst()->GetTime();

	m_PlayerTrailQueue.push(playerTrail);

	// 아직 Spawn 애니메이션이 재생중이면 리턴
	if (FlipBookComponent()->GetCurFlipBookName() == L"Spawn" && !FlipBookComponent()->IsFinished())
		return;

	// 아직 Tracking 중이 아니라면 이 오브젝트를 m_TrackingStartPos로 이동
	if (!m_bTracking)
	{
		Vec3 pos = Transform()->GetWorldPos();
		Transform()->SetRelativePos(pos + m_Vel_MoveToTrackingStartPos * DT);

		// Tracking 시작 위치에 도달하면 Tracking 시작
		if ((m_TrackingStartPos - Transform()->GetWorldPos()).Length() < 10.f)
		{
			m_bTracking = true;

			FlipBookComponent()->Pause();
		}
	}

	// Tracking 중이라면 플레이어의 위치로 이동
	else
	{
		// 플레이어의 위치를 queue에서 꺼내서 이동
		if (!m_PlayerTrailQueue.empty())
		{
			tPlayerTrail curTrail = m_PlayerTrailQueue.front();
			m_PlayerTrailQueue.pop();

			// 디버깅
			auto trailTime = curTrail.Time;
			auto curTime = CTimeMgr::GetInst()->GetTime();

			auto diff = curTime - trailTime;

			// 플레이어의 위치로 이동
			Vec3 pos = curTrail.Pos;
			Transform()->SetWorldPos(pos);

			// 플레이어의 상태에 따라 애니메이션 스프라이트 변경
			CFlipBookComponent* pFlipBookComponent = FlipBookComponent();
			switch (curTrail.State)
			{
				case PLAYER_STATE::IDLE:
					pFlipBookComponent->SetCurFlipBook(L"Idle");
					pFlipBookComponent->SetCurSpriteByIdx(curTrail.SpriteIdx);
					break;

				case PLAYER_STATE::RUN:
					pFlipBookComponent->SetCurFlipBook(L"Run");
					pFlipBookComponent->SetCurSpriteByIdx(curTrail.SpriteIdx);
					break;

				case PLAYER_STATE::JUMP:
					pFlipBookComponent->SetCurFlipBook(L"Jump");
					pFlipBookComponent->SetCurSpriteByIdx(curTrail.SpriteIdx);
					break;

				case PLAYER_STATE::DASH:
					pFlipBookComponent->SetCurFlipBook(L"Dash");
					pFlipBookComponent->SetCurSpriteByIdx(curTrail.SpriteIdx);
					break;

				case PLAYER_STATE::DREAM_DASH:
					pFlipBookComponent->SetCurFlipBook(L"DreamDash");
					pFlipBookComponent->SetCurSpriteByIdx(curTrail.SpriteIdx);
					break;

				case PLAYER_STATE::CLIMB:
					pFlipBookComponent->SetCurFlipBook(L"Climb");
					pFlipBookComponent->SetCurSpriteByIdx(curTrail.SpriteIdx);
					break;

				//case PLAYER_STATE::IDLE:
				//	pFlipBookComponent->Play(L"Idle");
				//	break;

				//case PLAYER_STATE::RUN:
				//	pFlipBookComponent->Play(L"Run");
				//	break;

				//case PLAYER_STATE::JUMP:
				//	pFlipBookComponent->Play(L"Jump");
				//	break;

				//case PLAYER_STATE::DASH:
				//	pFlipBookComponent->Play(L"Dash");
				//	break;

				//case PLAYER_STATE::DREAM_DASH:
				//	pFlipBookComponent->Play(L"DreamDash");
				//	break;

				//case PLAYER_STATE::CLIMB:
				//	pFlipBookComponent->Play(L"Climb");
				//	break;
			}

			// FacingRight 적용
			if (!curTrail.FacingRight)
			{
				Transform()->SetRelativeRotation(Vec3(0, XM_PI, 0));
			}
			else
			{
				Transform()->SetRelativeRotation(Vec3(0, 0, 0));
			}

		}
		else
		{
			// queue가 비어있으면 
			assert(false);
		}
	}


}

void CBadelineScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CBadelineScript::SaveToFile(FILE* _File)
{
}

void CBadelineScript::LoadFromFile(FILE* _File)
{
}
