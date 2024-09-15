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
	// FlipBook �߰�
	CFlipBookComponent* pFlipBookComponent = FlipBookComponent();
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Spawn\\Spawn.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Idle\\Idle.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Run\\Run.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Jump\\Jump.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Dash\\Dash.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\DreamDash\\DreamDash.flip"));
	pFlipBookComponent->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\badeline\\Climb\\Climb.flip"));

	pFlipBookComponent->Play(L"Spawn", false); // ���� �ִϸ��̼��� �� 1.25��

	// ���� ������ �÷��̾� ��ġ�� ����
	assert(m_Player != nullptr);
	m_TrackingStartPos = m_Player->Transform()->GetWorldPos();

	// Badeline�� ��ġ�� m_TrackingStartPos�� �̵���ų �ӵ� ���
	Vec3 vDir = m_TrackingStartPos - Transform()->GetWorldPos();
	m_Vel_MoveToTrackingStartPos = vDir / 1.75f; // 1.75�ʿ� ���� �̵�
}

void CBadelineScript::Tick()
{
	// �� �������� �÷��̾��� ��ġ �� ��Ÿ �����͸� queue�� ����
	tPlayerTrail playerTrail;
	playerTrail.Pos = m_Player->Transform()->GetWorldPos();
	playerTrail.State = m_Player->GetScript<CPlayerScript>()->GetCurState();
	playerTrail.FacingRight = m_Player->GetScript<CPlayerScript>()->IsFacingRight();
	playerTrail.SpriteIdx = m_Player->FlipBookComponent()->GetCurFrmIdx();
	playerTrail.Time = CTimeMgr::GetInst()->GetTime();

	m_PlayerTrailQueue.push(playerTrail);

	// ���� Spawn �ִϸ��̼��� ������̸� ����
	if (FlipBookComponent()->GetCurFlipBookName() == L"Spawn" && !FlipBookComponent()->IsFinished())
		return;

	// ���� Tracking ���� �ƴ϶�� �� ������Ʈ�� m_TrackingStartPos�� �̵�
	if (!m_bTracking)
	{
		Vec3 pos = Transform()->GetWorldPos();
		Transform()->SetRelativePos(pos + m_Vel_MoveToTrackingStartPos * DT);

		// Tracking ���� ��ġ�� �����ϸ� Tracking ����
		if ((m_TrackingStartPos - Transform()->GetWorldPos()).Length() < 10.f)
		{
			m_bTracking = true;

			FlipBookComponent()->Pause();
		}
	}

	// Tracking ���̶�� �÷��̾��� ��ġ�� �̵�
	else
	{
		// �÷��̾��� ��ġ�� queue���� ������ �̵�
		if (!m_PlayerTrailQueue.empty())
		{
			tPlayerTrail curTrail = m_PlayerTrailQueue.front();
			m_PlayerTrailQueue.pop();

			// �����
			auto trailTime = curTrail.Time;
			auto curTime = CTimeMgr::GetInst()->GetTime();

			auto diff = curTime - trailTime;

			// �÷��̾��� ��ġ�� �̵�
			Vec3 pos = curTrail.Pos;
			Transform()->SetWorldPos(pos);

			// �÷��̾��� ���¿� ���� �ִϸ��̼� ��������Ʈ ����
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

			// FacingRight ����
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
			// queue�� ��������� 
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
