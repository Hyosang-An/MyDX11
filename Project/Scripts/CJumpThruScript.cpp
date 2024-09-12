#include "pch.h"
#include "CJumpThruScript.h"
#include "CRigidBody.h"
#include "CPlayerScript.h"

#include "Engine/CGameObject.h"
#include "Engine/CCollisionMgr.h"

CJumpThruScript::CJumpThruScript() :
	CScript((UINT)SCRIPT_TYPE::JUMPTHRUSCRIPT)

{
}

CJumpThruScript::CJumpThruScript(const CJumpThruScript& _other) :
	CScript((UINT)SCRIPT_TYPE::JUMPTHRUSCRIPT)

{
}

CJumpThruScript::~CJumpThruScript()
{
}

void CJumpThruScript::Begin()
{
}

void CJumpThruScript::Tick()
{
}

void CJumpThruScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (_OtherObject->GetLayerIdx() == int(LAYER::PLAYER))
	{
		Vec3 vColliderPos = _OwnCollider->GetWorldPos();
		Vec3 vOtherColliderPos = _OtherCollider->GetWorldPos();
		Vec3 vDir = vOtherColliderPos - vColliderPos;

		Vec3 playerPos = _OtherObject->Transform()->GetWorldPos();

		Vec2 overlapArea = CCollisionMgr::GetInst()->GetOverlapArea();

		CGameObject* pPlayer = _OtherObject;
		CRigidBody* playerRigidbody = pPlayer->GetScript<CRigidBody>();
		CPlayerScript* playerScript = pPlayer->GetScript<CPlayerScript>();

		// 플레이어가 점프스루에 위에서 충돌했을 때만 바닥처럼 작동
		if (overlapArea.x > overlapArea.y && vDir.y > 0 && playerRigidbody->GetVelocity().y < 0 && !playerRigidbody->IsOnGround())
		{
			//플레이어 위치 보정
			playerPos.y += overlapArea.y;
			pPlayer->Transform()->SetRelativePos(playerPos);

			playerRigidbody->OnLand();
			playerScript->AddGroundCollider(_OwnCollider);
		}
	}
}


void CJumpThruScript::Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CJumpThruScript::EndOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (_OtherObject->GetLayerIdx() == int(LAYER::PLAYER))
	{
		CGameObject* pPlayer = _OtherObject;
		CPlayerScript* playerScript = pPlayer->GetScript<CPlayerScript>();

		playerScript->RemoveGroundCollider(_OwnCollider);

		if (playerScript->IsEmptyGroundColliders())
		{
			CRigidBody* playerRigidbody = pPlayer->GetScript<CRigidBody>();
			playerRigidbody->SetOnGround(false);
		}
	}
}

void CJumpThruScript::SaveToFile(FILE* _File)
{
}

void CJumpThruScript::LoadFromFile(FILE* _File)
{
}
