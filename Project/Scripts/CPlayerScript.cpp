#include "pch.h"
#include "CPlayerScript.h"
#include "CRigidBody.h"
#include "Engine/CCollisionMgr.h"

CPlayerScript::CPlayerScript() :
	CScript(UINT(SCRIPT_TYPE::PLAYERSCRIPT))
{
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "PlayerSpeed", &m_fMagnitudeOfMoveForce);
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Begin()
{
	// 2D 파트에서는 최적화하지 않을 것이므로 오브젝트 시작 시 모든 객체가 DynamicMaterial을 각자 들고 있어도 된다.
	GetRenderComponent()->GetDynamicMaterial();

	m_pRigidBody = GetOwner()->GetScript<CRigidBody>();
}

void CPlayerScript::Tick()
{
	 // 이동: 방향기
	 // 점프: c
	 // 대쉬: x
	 // 매달리기: z 

	 // 키보드 입력 체크
	 KeyCheck();

	 // 상태 업데이트
	 UpdateState();

	 // 애니메이션 업데이트
	 UpdateAnimation();






	 m_PrevState = m_CurState;
}

void CPlayerScript::KeyCheck()
{
	//OnGround 상태 업데이트
	m_bOnGround = m_pRigidBody->IsOnGround();

	// IDLE
	if (m_bOnGround && !KEY_PRESSED(KEY::RIGHT) && !KEY_PRESSED(KEY::LEFT))
	{
		m_CurState = PLAYER_STATE::IDLE;
	}

	// 바라보는 방향 업데이트
	if (KEY_JUST_PRESSED(KEY::RIGHT))
	{
		m_bFacingRight = true;
		Transform()->SetRelativeRotation(Vec3(0, 0, 0));
	}
	else if (KEY_JUST_PRESSED(KEY::LEFT))
	{
		m_bFacingRight = false;
		auto vRot = Vec3(0, XM_PI, 0);
		Transform()->SetRelativeRotation(vRot);
	}

	// RUN
	if (m_bOnGround && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
	{
		if ((KEY_PRESSED(KEY::RIGHT) && m_bFacingRight) || (KEY_PRESSED(KEY::LEFT) && !m_bFacingRight))
		{

			m_CurState = PLAYER_STATE::RUN;
		}
		else
		{
			m_CurState = PLAYER_STATE::IDLE;
		}
	}

	// JUMP
	if (KEY_JUST_PRESSED(KEY::C) && m_bOnGround)
	{
		//m_pRigidBody->SetVelocity(Vec3(m_pRigidBody->GetVelocity().x, m_fJumpSpeed, 0));
		m_CurState = PLAYER_STATE::JUMP;
	}

	// DASH
	if (KEY_JUST_PRESSED(KEY::X) && m_fDashTimeRemained > 0 && m_PrevState != PLAYER_STATE::DASH)
	{		
		// 8가지 방향에 대한 대쉬 처리
		if (KEY_PRESSED(KEY::RIGHT) && KEY_PRESSED(KEY::UP))
		{
			Vec3 vDir = Vec3(1, 1, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else if (KEY_PRESSED(KEY::RIGHT) && KEY_PRESSED(KEY::DOWN))
		{
			Vec3 vDir = Vec3(1, -1, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else if (KEY_PRESSED(KEY::LEFT) && KEY_PRESSED(KEY::UP))
		{
			Vec3 vDir = Vec3(-1, 1, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else if (KEY_PRESSED(KEY::LEFT) && KEY_PRESSED(KEY::DOWN))
		{
			Vec3 vDir = Vec3(-1, -1, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else if (KEY_PRESSED(KEY::RIGHT))
		{
			Vec3 vDir = Vec3(1, 0, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else if (KEY_PRESSED(KEY::LEFT))
		{
			Vec3 vDir = Vec3(-1, 0, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else if (KEY_PRESSED(KEY::UP))
		{
			Vec3 vDir = Vec3(0, 1, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else if (KEY_PRESSED(KEY::DOWN))
		{
			Vec3 vDir = Vec3(0, -1, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}
		else
		{
			Vec3 vDir = Vec3(1, 0, 0).Normalize();
			m_pRigidBody->SetVelocity(vDir * m_fDashSpeed);
		}

		m_CurState = PLAYER_STATE::DASH;		
	}

	// CLIMB
	if (m_bCollisionWithWall && (KEY_JUST_PRESSED(KEY::Z) || KEY_PRESSED(KEY::Z)))
	{
		m_CurState = PLAYER_STATE::CLIMB;
	}


	// DREAM DASH
	// TODO : 드림 대쉬 구현

	// CHANGE ROOM
	// TODO : 방 바꾸기 구현

	// DEAD
	// TODO : 죽음 처리 구현

	// FALL
	//if (m_pRigidBody->GetVelocity().y < 0 && m_CurState != )
	//{
	//	m_CurState = PLAYER_STATE::FALL;
	//}
	// 아래 방향으로 떨어지면서, 대쉬나 드림 대쉬 상태가 아니면 FALL 상태로 전환
	if (m_pRigidBody->GetVelocity().y < 0 && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
	{
		m_CurState = PLAYER_STATE::FALL;
	}
}

void CPlayerScript::UpdateState()
{
	// Switch문을 사용하여 상태에 따른 상태 처리
	switch (m_CurState)
	{
		case PLAYER_STATE::IDLE:
		{
			m_pRigidBody->SetVelocity(Vec3(0, 0, 0));
			break;
		}

		case PLAYER_STATE::RUN:
		{
			if (KEY_PRESSED(KEY::RIGHT))
			{
				if (m_pRigidBody->GetVelocity().x < m_MaxSpeedX)
				{
					m_pRigidBody->AddForce(Vec3(m_fMagnitudeOfMoveForce, 0, 0));
				}
			}
			else if (KEY_PRESSED(KEY::LEFT))
			{
				if (m_pRigidBody->GetVelocity().x > -m_MaxSpeedX)
				{
					m_pRigidBody->AddForce(Vec3(-m_fMagnitudeOfMoveForce, 0, 0));
				}
			}

			break;
		}

		case PLAYER_STATE::JUMP:
		{
			if (m_PrevState != PLAYER_STATE::JUMP)
				m_pRigidBody->SetVelocity(Vec3(m_pRigidBody->GetVelocity().x, m_fJumpSpeed, 0));

			break;
		}

		case PLAYER_STATE::FALL:
		{
			if (KEY_PRESSED(KEY::RIGHT))
			{
				if (m_pRigidBody->GetVelocity().x < m_MaxSpeedX)
				{
					m_pRigidBody->AddForce(Vec3(m_fMagnitudeOfMoveForce, 0, 0));
				}
			}
			else if (KEY_PRESSED(KEY::LEFT))
			{
				if (m_pRigidBody->GetVelocity().x > -m_MaxSpeedX)
				{
					m_pRigidBody->AddForce(Vec3(-m_fMagnitudeOfMoveForce, 0, 0));
				}
			}

			break;
		}

		case PLAYER_STATE::DASH:
		{
			m_fDashTimeRemained -= DT;

			if (m_fDashTimeRemained <= 0)
			{
				if (m_bOnGround)
					m_CurState = PLAYER_STATE::IDLE;
				else
					m_CurState = PLAYER_STATE::FALL;

				// 속도 정상화
				Vec3 vDir = m_pRigidBody->GetVelocity().Normalize();
				if (vDir.y >= 0)
				{
					vDir.y = 0;
					m_pRigidBody->SetVelocity(vDir * m_MaxSpeedX);
				}
			}

			break;
		}

		case PLAYER_STATE::CLIMB:
		{
			// TODO : 매달리는 상태 처리
			break;
		}

		case PLAYER_STATE::DREAM_DASH:
		{
			// TODO : 드림 대쉬 상태 처리
			break;
		}

		case PLAYER_STATE::CHANGE_ROOM:
		{
			// TODO : 방 바꾸는 상태 처리
			break;
		}

		case PLAYER_STATE::DEAD:
		{
			// TODO : 죽음 처리
			break;
		}
	}
}

void CPlayerScript::UpdateAnimation()
{
}

void CPlayerScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 충돌한 오브젝트의 Layer에 따른 분류
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			// 일단 파고들어간 깊이만큼 보정
			Vec3 vOverlap = CCollisionMgr::GetInst()->GetOverlap();
			Vec3 vColliderPos = _OwnCollider->GetWorldPos();
			Vec3 vOtherColliderPos = _OtherCollider->GetWorldPos();
			Vec3 vDir = vOtherColliderPos - vColliderPos;

			if (vOverlap.Dot(vDir) > 0)
			{
				vOverlap = -vOverlap;
			}

			Vec3 vObjPos = Transform()->GetWorldPos();
			vObjPos += vOverlap;
			Transform()->SetWorldPos(vObjPos);

			// 벽과 충돌한건지 땅과 충돌한건지 판단
			if (abs(vOverlap.y) > abs(vOverlap.x))
			{
				// 땅과 충돌한 경우
				if (m_pRigidBody->GetVelocity().y < 0.f && !m_pRigidBody->IsOnGround())
				{
					m_pRigidBody->OnLand();
					m_setGroundColliders.insert(_OtherCollider);

					// 대쉬 초기화
					m_fDashTimeRemained = m_fDashTime;

					// 대쉬 상태에서 땅에 닿으면 대쉬 상태 해제
					if (m_CurState == PLAYER_STATE::DASH)
					{
						m_CurState = PLAYER_STATE::IDLE;
					}

					// 드림 대쉬 상태에서 땅에 닿으면 DEAD
					if (m_CurState == PLAYER_STATE::DREAM_DASH)
					{
						m_CurState = PLAYER_STATE::DEAD;
					}
				}
				else
				{
					// 천장과 충돌한 경우
					m_pRigidBody->SetVelocity(Vec3(m_pRigidBody->GetVelocity().x, 0.f, 0.f));

					// 점프	상태에서 천장에 닿으면 점프 상태 해제
					if (m_CurState == PLAYER_STATE::JUMP)
					{
						m_CurState = PLAYER_STATE::FALL;
					}

					// 대쉬 상태에서 천장에 닿으면 대쉬 상태 해제
					if (m_CurState == PLAYER_STATE::DASH)
					{
						m_CurState = PLAYER_STATE::FALL;
					}

					// 드림 대쉬 상태에서 천장에 닿으면 DEAD
					if (m_CurState == PLAYER_STATE::DREAM_DASH)
					{
						m_CurState = PLAYER_STATE::DEAD;
					}
				}
			}
			else
			{
				// 벽과 충돌한 경우
				m_pRigidBody->SetVelocity(Vec3(0.f, m_pRigidBody->GetVelocity().y, 0.f));
				m_bCollisionWithWall = true;
				m_setWallColliders.insert(_OtherCollider);

				// 대쉬 상태에서 벽에 닿으면 대쉬 상태 해제
				if (m_CurState == PLAYER_STATE::DASH)
				{
					if (m_bOnGround)
					{
						m_CurState = PLAYER_STATE::IDLE;
					}
					else
					{
						m_CurState = PLAYER_STATE::FALL;
					}
				}

				// 드림 대쉬 상태에서 벽에 닿으면 DEAD
				if (m_CurState == PLAYER_STATE::DREAM_DASH)
				{
					m_CurState = PLAYER_STATE::DEAD;
				}

			}
			break;
		}

		case LAYER::SPIKE:
		{
			// TODO : 죽음 처리

		}
		break;
		

	}
}

void CPlayerScript::Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 충돌한 오브젝트의 Layer에 따른 분류
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			// 일단 파고들어간 깊이만큼 보정
			Vec3 vOverlap = CCollisionMgr::GetInst()->GetOverlap();
			Vec3 vColliderPos = _OwnCollider->GetWorldPos();
			Vec3 vOtherColliderPos = _OtherCollider->GetWorldPos();
			Vec3 vDir = vOtherColliderPos - vColliderPos;

			if (vOverlap.Dot(vDir) > 0)
			{
				vOverlap = -vOverlap;
			}

			Vec3 vObjPos = Transform()->GetWorldPos();
			vObjPos += vOverlap;
			Transform()->SetWorldPos(vObjPos);
		}
		
		break;
	}
}

void CPlayerScript::EndOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			// Ground인지, Wall인지 판단
			if (m_setGroundColliders.find(_OtherCollider) != m_setGroundColliders.end())
			{
				m_setGroundColliders.erase(_OtherCollider);

				// 땅에서 떨어진 경우
				if (m_setGroundColliders.empty())
					m_pRigidBody->SetOnGround(false);
			}
			else if (m_setWallColliders.find(_OtherCollider) != m_setWallColliders.end())
			{
				m_setWallColliders.erase(_OtherCollider);

				// 벽에서 떨어진 경우
				if (m_setWallColliders.empty())
					m_bCollisionWithWall = false;
			}
		}
	}
}

void CPlayerScript::SaveToFile(FILE* _File)
{

}

void CPlayerScript::LoadFromFile(FILE* _File)
{

}