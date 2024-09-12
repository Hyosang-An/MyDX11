#include "pch.h"
#include "CPlayerScript.h"
#include "CRigidBody.h"
#include "Engine/CCollisionMgr.h"
#include "Engine/CLevelMgr.h"
#include "Engine/CLevel.h"

#include "CRoomScript.h"
#include "CCameraMoveScript.h"
#include "CDashTrailScript.h"

CPlayerScript::CPlayerScript() :
	CScript(UINT(SCRIPT_TYPE::PLAYERSCRIPT))
{
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "MagnitudeOfMoveForce", &m_MagnitudeOfMoveForce);
}

CPlayerScript::CPlayerScript(const CPlayerScript& _Other) :
	CScript(UINT(SCRIPT_TYPE::PLAYERSCRIPT))
{
	// _other에서 변수들 얕은 복사S
	//m_CurRoomIdx = 0;


	m_MagnitudeOfMoveForce = _Other.m_MagnitudeOfMoveForce;
	m_MaxRunSpeed = _Other.m_MaxRunSpeed;
	m_JumpSpeed = _Other.m_JumpSpeed;
	m_DashSpeed = _Other.m_DashSpeed;
	m_ClimbSpeed = _Other.m_ClimbSpeed;
	m_SpeedBeforeDash = _Other.m_SpeedBeforeDash;
	m_DashTime = _Other.m_DashTime;
	m_DashTimeRemained = _Other.m_DashTimeRemained;
	m_MaxFallingSpeed = _Other.m_MaxFallingSpeed;

	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "MagnitudeOfMoveForce", &m_MagnitudeOfMoveForce);
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Begin()
{
	// 2D 파트에서는 최적화하지 않을 것이므로 오브젝트 시작 시 모든 객체가 DynamicMaterial을 각자 들고 있어도 된다.
	GetRenderComponent()->GetDynamicMaterial();

	m_RigidBody = GetOwner()->GetScript<CRigidBody>();
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

void CPlayerScript::ChangeRoom(CGameObject* _Room)
{
	m_Room = _Room;

	// 메인카메라의 ChangeRoom 함수 호출
	auto mainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	mainCamera->GetScript<CCameraMoveScript>()->SetChangeRoom();
}

void CPlayerScript::KeyCheck()
{
	// 디버그
	auto owner = GetOwner();

	//OnGround 상태 업데이트
	m_bOnGround = m_RigidBody->IsOnGround();

	// Death 상태일 때는 입력을 받지 않는다.
	if (m_CurState == PLAYER_STATE::DEATH)
		return;

	// IDLE
	if (m_bOnGround && !KEY_PRESSED(KEY::RIGHT) && !KEY_PRESSED(KEY::LEFT) && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
	{
		m_CurState = PLAYER_STATE::IDLE;
	}

	// 바라보는 방향 업데이트
	if (KEY_JUST_PRESSED(KEY::RIGHT))
	{
		m_bFacingRight = true;
		
	}
	else if (KEY_JUST_PRESSED(KEY::LEFT))
	{
		m_bFacingRight = false;
	}

	if (m_bFacingRight)
	{
		if (m_bOnLeftWall)
			Transform()->SetRelativeRotation(Vec3(0, XM_PI, 0));
		else
			Transform()->SetRelativeRotation(Vec3(0, 0, 0));
	}
	else
	{
		if (m_bOnRightWall)
			Transform()->SetRelativeRotation(Vec3(0, 0, 0));
		else
			Transform()->SetRelativeRotation(Vec3(0, XM_PI, 0));
	}

	// RUN
	if (m_bOnGround && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
	{
		if ((KEY_PRESSED(KEY::RIGHT) && m_bFacingRight && !m_bOnRightWall) || (KEY_PRESSED(KEY::LEFT) && !m_bFacingRight && !m_bOnLeftWall))
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

	// CLIMB
	if ((m_bOnLeftWall || m_bOnRightWall) && (KEY_JUST_PRESSED(KEY::Z) || KEY_PRESSED(KEY::Z)) && m_CurState !=PLAYER_STATE::JUMP && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
	{
		m_CurState = PLAYER_STATE::CLIMB;
	}

	// DASH
	if (m_DashCount != 1 && m_bOnGround)
	{
		m_DashCount = 1;
		//m_DashTimeRemained = m_DashTime;
	}

	if (KEY_JUST_PRESSED(KEY::X) && m_DashCount > 0 && m_PrevState != PLAYER_STATE::DASH)
	{		
		--m_DashCount;
		m_SpeedBeforeDash = m_RigidBody->GetVelocity().Length();

		Vec3 vDir;

		// 8가지 방향에 대한 대쉬 처리
		if (KEY_PRESSED(KEY::RIGHT) && KEY_PRESSED(KEY::UP))
		{
			vDir = Vec3(1, 1, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}
		else if (KEY_PRESSED(KEY::RIGHT) && KEY_PRESSED(KEY::DOWN))
		{
			vDir = Vec3(1, -1, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}
		else if (KEY_PRESSED(KEY::LEFT) && KEY_PRESSED(KEY::UP))
		{
			vDir = Vec3(-1, 1, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}
		else if (KEY_PRESSED(KEY::LEFT) && KEY_PRESSED(KEY::DOWN))
		{
			vDir = Vec3(-1, -1, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}
		else if (KEY_PRESSED(KEY::RIGHT))
		{
			vDir = Vec3(1, 0, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}
		else if (KEY_PRESSED(KEY::LEFT))
		{
			vDir = Vec3(-1, 0, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}
		else if (KEY_PRESSED(KEY::UP))
		{
			Vec3 vDir = Vec3(0, 1, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}
		else if (KEY_PRESSED(KEY::DOWN))
		{
			vDir = Vec3(0, -1, 0).Normalize();
			m_RigidBody->SetVelocity(vDir * m_DashSpeed);
		}

		// 방향키를 누르지 않은 경우
		else
		{
			if (m_bFacingRight)
			{
				vDir = Vec3(1, 0, 0).Normalize();
				m_RigidBody->SetVelocity(vDir * m_DashSpeed);
			}
			else
			{
				vDir = Vec3(-1, 0, 0).Normalize();
				m_RigidBody->SetVelocity(vDir * m_DashSpeed);
			}
		}

		m_CurState = PLAYER_STATE::DASH;		
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
	// 아래 방향으로 떨어지면서, CLIMB나 대쉬나 드림 대쉬 상태가 아니면 FALL 상태로 전환
	if (m_RigidBody->GetVelocity().y < 0 && m_CurState !=PLAYER_STATE::CLIMB && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
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
			m_RigidBody->SetVelocity(Vec3(0, 0, 0));
			break;
		}

		case PLAYER_STATE::RUN:
		{
			if (KEY_PRESSED(KEY::RIGHT))
			{
				if (m_RigidBody->GetVelocity().x < m_MaxRunSpeed)
				{
					m_RigidBody->AddForce(Vec3(m_MagnitudeOfMoveForce, 0, 0));
				}
			}
			else if (KEY_PRESSED(KEY::LEFT))
			{
				if (m_RigidBody->GetVelocity().x > -m_MaxRunSpeed)
				{
					m_RigidBody->AddForce(Vec3(-m_MagnitudeOfMoveForce, 0, 0));
				}
			}

			break;
		}

		case PLAYER_STATE::JUMP:
		{
			if (m_PrevState != PLAYER_STATE::JUMP)
			{
				m_RigidBody->SetVelocity(Vec3(m_RigidBody->GetVelocity().x, m_JumpSpeed, 0));
				m_RigidBody->SetOnGround(false);
			}

			if (KEY_PRESSED(KEY::RIGHT))
			{
				if (m_RigidBody->GetVelocity().x < m_MaxRunSpeed)
				{
					m_RigidBody->AddForce(Vec3(m_MagnitudeOfMoveForce, 0, 0));
				}
			}
			else if (KEY_PRESSED(KEY::LEFT))
			{
				if (m_RigidBody->GetVelocity().x > -m_MaxRunSpeed)
				{
					m_RigidBody->AddForce(Vec3(-m_MagnitudeOfMoveForce, 0, 0));
				}
			}

			break;
		}

		case PLAYER_STATE::FALL:
		{
			if (KEY_PRESSED(KEY::RIGHT))
			{
				if (m_RigidBody->GetVelocity().x < m_MaxRunSpeed)
				{
					m_RigidBody->AddForce(Vec3(m_MagnitudeOfMoveForce, 0, 0));
				}
			}
			else if (KEY_PRESSED(KEY::LEFT))
			{
				if (m_RigidBody->GetVelocity().x > -m_MaxRunSpeed)
				{
					m_RigidBody->AddForce(Vec3(-m_MagnitudeOfMoveForce, 0, 0));
				}
			}

			break;
		}

		case PLAYER_STATE::DASH:
		{
			// 벽이 있으면 벽 방향 속도 0
			if (m_bFacingRight && m_bOnRightWall)
			{
				m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
			}
			else if (!m_bFacingRight && m_bOnLeftWall)
			{
				m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
			}

			// 바닥에 있으면 바닥 방향 속도 0
			if (m_RigidBody->GetVelocity().y < 0.f && m_RigidBody->IsOnGround())
			{
				m_RigidBody->SetVelocity(Vec3(m_RigidBody->GetVelocity().x, 0, 0));
			}

			if (m_PrevState != PLAYER_STATE::DASH)
			{
				m_DashTimeRemained = m_DashTime;
				m_DashTrailTimeSinceLastTrail = 0;
			}

			// 대쉬 잔상 생성
			if (m_DashTrailTimeSinceLastTrail >= m_DashTrailInterval)
			{
				m_DashTrailTimeSinceLastTrail -= m_DashTrailInterval;

				CGameObject* dashTrail = new CGameObject;
				dashTrail->SetName(L"DashTrail");

				dashTrail->AddComponent(new CTransform);
				dashTrail->AddComponent(new CMeshRender);
				dashTrail->AddComponent(new CFlipBookComponent);

				dashTrail->FlipBookComponent()->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\Dash\\Dash.flip"));
				dashTrail->FlipBookComponent()->Play(L"Dash", false);
				dashTrail->FlipBookComponent()->SetCurSprite(2);
				dashTrail->FlipBookComponent()->Pause();

				Vec3 vPos = GetOwner()->Transform()->GetWorldPos();
				dashTrail->Transform()->SetWorldPos(vPos);

				Vec3 vScale = GetOwner()->Transform()->GetWorldScale();
				dashTrail->Transform()->SetWorldScale(vScale);

				Vec3 vRot = GetOwner()->Transform()->GetRelativeRoatation();
				dashTrail->Transform()->SetRelativeRotation(vRot);

				dashTrail->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
				dashTrail->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"PlayerTrailMtrl"));

				dashTrail->AddComponent(new CDashTrailScript);

				// 현재 레벨에 추가
				CLevelMgr::GetInst()->GetCurrentLevel()->AddObject(LAYER::DEFAULT, dashTrail);

			}

			m_DashTrailTimeSinceLastTrail += DT;
			m_DashTimeRemained -= DT;

			if (m_DashTimeRemained <= 0)
			{
				if (m_bOnGround)
					m_CurState = PLAYER_STATE::IDLE;
				else
					m_CurState = PLAYER_STATE::FALL;

				// 속도 정상화
				Vec3 vDir = m_RigidBody->GetVelocity().Normalize();
				m_RigidBody->SetVelocity(vDir * m_SpeedBeforeDash);
				
			}

			break;
		}

		case PLAYER_STATE::CLIMB:
		{
			if (KEY_PRESSED(KEY::UP))
			{
				m_RigidBody->SetVelocity(Vec3(0, 100, 0));
			}
			else if (KEY_PRESSED(KEY::DOWN))
			{
				m_RigidBody->SetVelocity(Vec3(0, -100, 0));
			}
			else
			{
				m_RigidBody->SetVelocity(Vec3(0, 0, 0));
			}
			
			if ((KEY_RELEASED(KEY::Z) || (!m_bOnLeftWall && !m_bOnRightWall)))
			{
				if (m_bOnGround)
					m_CurState = PLAYER_STATE::IDLE;
				else
					m_CurState = PLAYER_STATE::FALL;
			}

			// 점프
			if (KEY_JUST_PRESSED(KEY::C))
			{
				Vec3 vVelocity;

				if (m_bOnLeftWall && KEY_PRESSED(KEY::RIGHT))
					vVelocity = Vec3(m_MaxRunSpeed, m_JumpSpeed, 0);
				else if (m_bOnRightWall && KEY_PRESSED(KEY::LEFT))
					vVelocity = Vec3(-m_MaxRunSpeed, m_JumpSpeed, 0);
				else
					vVelocity = Vec3(m_RigidBody->GetVelocity().x, m_JumpSpeed, 0);
				
				m_RigidBody->SetVelocity(vVelocity);
				m_RigidBody->SetOnGround(false);
				m_CurState = PLAYER_STATE::JUMP;
			}

			// 대쉬
			if (KEY_JUST_PRESSED(KEY::X) && m_DashCount > 0)
			{
				--m_DashCount;
				m_SpeedBeforeDash = m_RigidBody->GetVelocity().Length();

				Vec3 vDir;

				// 8가지 방향에 대한 대쉬 처리
				if (KEY_PRESSED(KEY::RIGHT) && KEY_PRESSED(KEY::UP))
				{
					vDir = Vec3(1, 1, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}
				else if (KEY_PRESSED(KEY::RIGHT) && KEY_PRESSED(KEY::DOWN))
				{
					vDir = Vec3(1, -1, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}
				else if (KEY_PRESSED(KEY::LEFT) && KEY_PRESSED(KEY::UP))
				{
					vDir = Vec3(-1, 1, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}
				else if (KEY_PRESSED(KEY::LEFT) && KEY_PRESSED(KEY::DOWN))
				{
					vDir = Vec3(-1, -1, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}
				else if (KEY_PRESSED(KEY::RIGHT))
				{
					vDir = Vec3(1, 0, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}
				else if (KEY_PRESSED(KEY::LEFT))
				{
					vDir = Vec3(-1, 0, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}
				else if (KEY_PRESSED(KEY::UP))
				{
					Vec3 vDir = Vec3(0, 1, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}
				else if (KEY_PRESSED(KEY::DOWN))
				{
					vDir = Vec3(0, -1, 0).Normalize();
					m_RigidBody->SetVelocity(vDir * m_DashSpeed);
				}

				// 방향키를 누르지 않은 경우
				else
				{
					if (m_bFacingRight)
					{
						vDir = Vec3(1, 0, 0).Normalize();
						m_RigidBody->SetVelocity(vDir * m_DashSpeed);
					}
					else
					{
						vDir = Vec3(-1, 0, 0).Normalize();
						m_RigidBody->SetVelocity(vDir * m_DashSpeed);
					}
				}

				m_CurState = PLAYER_STATE::DASH;
			}

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

		case PLAYER_STATE::DEATH:
		{
			// TODO : 죽음 처리
			m_RigidBody->SetVelocity(Vec3(0, 0, 0));

			if (GetOwner()->FlipBookComponent()->IsFinished())
			{
				//respawn
				auto respawnPos = m_Room->GetScript<CRoomScript>()->GetPlayerSpawnPos();
				GetOwner()->Transform()->SetWorldPos(respawnPos);

				m_CurState = PLAYER_STATE::IDLE;
			}

			break;
		}
	}
}

void CPlayerScript::UpdateAnimation()
{
	// CLIMB만 예외 처리
	if (m_CurState == PLAYER_STATE::CLIMB)
	{
		m_RigidBody->GetVelocity().y != 0 ? GetOwner()->FlipBookComponent()->Resume() : GetOwner()->FlipBookComponent()->Pause();
	}

	if (m_PrevState == m_CurState)
		return;

	switch (m_CurState)
	{
		case PLAYER_STATE::IDLE:
			GetOwner()->FlipBookComponent()->Play(L"Idle", true);
			break;
		case PLAYER_STATE::RUN:
			GetOwner()->FlipBookComponent()->Play(L"Run", true);
			break;
		case PLAYER_STATE::JUMP:
			GetOwner()->FlipBookComponent()->Play(L"Jump", false);
			break;
		case PLAYER_STATE::FALL:
			GetOwner()->FlipBookComponent()->Play(L"Fall", true);
			break;
		case PLAYER_STATE::DASH:
			GetOwner()->FlipBookComponent()->Play(L"Dash", false);
			break;
		case PLAYER_STATE::CLIMB:
			GetOwner()->FlipBookComponent()->Play(L"Climb", true);
			m_RigidBody->GetVelocity().y != 0 ? GetOwner()->FlipBookComponent()->Resume() : GetOwner()->FlipBookComponent()->Pause();
			break;
		case PLAYER_STATE::DREAM_DASH:
			break;
		case PLAYER_STATE::CHANGE_ROOM:
			break;
		case PLAYER_STATE::DEATH:
			GetOwner()->FlipBookComponent()->Play(L"Death", false);
			break;
		default:
			break;
	}
}

void CPlayerScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (m_RigidBody == nullptr)
	return;

	// 충돌한 오브젝트의 Layer에 따른 분류
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			//Vec3 vOverlap = CCollisionMgr::GetInst()->GetOverlap();
			Vec2 overlapArea = CCollisionMgr::GetInst()->GetOverlapArea();

			Vec3 vColliderPos = _OwnCollider->GetWorldPos();
			Vec3 vOtherColliderPos = _OtherCollider->GetWorldPos();
			Vec3 vDir = vOtherColliderPos - vColliderPos;

			Vec3 vObjWorldPos = GetOwner()->Transform()->GetWorldPos();

			// 벽과 충돌한건지 땅과 충돌한건지 판단
			if (abs(overlapArea.y) < abs(overlapArea.x))
			{		
				// 땅과 충돌한 경우
				if (vDir.y < 0 && m_RigidBody->GetVelocity().y < 0.f && !m_RigidBody->IsOnGround())
				{
					// 위치 보정
					vObjWorldPos.y += overlapArea.y;
					GetOwner()->Transform()->SetRelativePos(vObjWorldPos);

					m_RigidBody->OnLand();
					m_setGroundColliders.insert(_OtherCollider);

					//// 대쉬 초기화
					//m_DashTimeRemained = m_DashTime;

					//// 대쉬 상태에서 땅에 닿으면 대쉬 상태 해제
					//if (m_CurState == PLAYER_STATE::DASH)
					//{
					//	m_CurState = PLAYER_STATE::IDLE;
					//}

					//// 드림 대쉬 상태에서 땅에 닿으면 DEAD
					//if (m_CurState == PLAYER_STATE::DREAM_DASH)
					//{
					//	m_CurState = PLAYER_STATE::DEATH;
					//}
				}

				// 천장과 충돌한 경우
				else
				{
					// 위치 보정
					vObjWorldPos.y -= overlapArea.y;
					GetOwner()->Transform()->SetRelativePos(vObjWorldPos);

					m_RigidBody->SetVelocity(Vec3(m_RigidBody->GetVelocity().x, 0.f, 0.f));

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
						m_CurState = PLAYER_STATE::DEATH;
					}
				}
			}
			else
			{
				// 벽과 충돌한 경우
				m_RigidBody->SetVelocity(Vec3(0.f, m_RigidBody->GetVelocity().y, 0.f));
				if (vDir.x > 0)
				{
					// 위치 보정
					vObjWorldPos.x -= overlapArea.x;

					m_setRightWallColliders.insert(_OtherCollider);
					m_bOnRightWall = true;
				}
				else
				{
					// 위치 보정
					vObjWorldPos.x += overlapArea.x;

					m_setLeftWallColliders.insert(_OtherCollider);
					m_bOnLeftWall = true;
				}
				GetOwner()->Transform()->SetRelativePos(vObjWorldPos);

				// 대쉬 상태에서 벽에 닿으면 해당 방향의 속도 0
				if (m_CurState == PLAYER_STATE::DASH)
				{
					if (m_bFacingRight && m_bOnRightWall)
					{
						m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
					}
					else if (!m_bFacingRight && m_bOnLeftWall)
					{
						m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
					}

					/*if (m_bOnGround)
					{
						m_CurState = PLAYER_STATE::IDLE;
					}
					else
					{
						m_CurState = PLAYER_STATE::FALL;
					}*/
				}

				// 드림 대쉬 상태에서 벽에 닿으면 DEAD
				if (m_CurState == PLAYER_STATE::DREAM_DASH)
				{
					m_CurState = PLAYER_STATE::DEATH;
				}

			}
			break;
		}

		case LAYER::SPIKE:
		{
			// TODO : 죽음 처리
			m_CurState = PLAYER_STATE::DEATH;
			m_RigidBody->SetVelocity(Vec3(0, 0, 0));
		}
		break;
		

	}
}

void CPlayerScript::Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (m_RigidBody == nullptr)
		return;

	Vec2 overlapArea = CCollisionMgr::GetInst()->GetOverlapArea();
	Vec3 vObjWorldPos = GetOwner()->Transform()->GetWorldPos();

	// 충돌한 오브젝트의 Layer에 따른 분류
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			// 벽과 충돌한 경우
			if (overlapArea.x < overlapArea.y)
			{
				// 벽 방향 속도 0
				if (m_bFacingRight && m_bOnRightWall)
				{
					// 위치 보정
					vObjWorldPos.x -= overlapArea.x;
					GetOwner()->Transform()->SetRelativePos(vObjWorldPos);

					m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
				}
				else if (!m_bFacingRight && m_bOnLeftWall)
				{
					// 위치 보정
					vObjWorldPos.x += overlapArea.x;
					GetOwner()->Transform()->SetRelativePos(vObjWorldPos);

					m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
				}

				// 바닥 방향 속도 0 (땅과 충돌한 경우)
				if (m_RigidBody->GetVelocity().y < 0.f && m_RigidBody->IsOnGround())
				{
					// 위치 보정
					vObjWorldPos.y += overlapArea.y;
					GetOwner()->Transform()->SetRelativePos(vObjWorldPos);

				}
			}
			// 땅과 충돌한 경우
			else
			{
				// 위치 보정
				vObjWorldPos.y += overlapArea.y;
				GetOwner()->Transform()->SetRelativePos(vObjWorldPos);

				m_RigidBody->OnLand();
			}
		}
		
		break;
	}
}

void CPlayerScript::EndOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (m_RigidBody == nullptr)
		return;

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
					m_RigidBody->SetOnGround(false);
			}

			else if (m_setLeftWallColliders.find(_OtherCollider) != m_setLeftWallColliders.end())
			{
				m_setLeftWallColliders.erase(_OtherCollider);

				// 벽에서 떨어진 경우
				if (m_setLeftWallColliders.empty())
					m_bOnLeftWall = false;
			}

			else if (m_setRightWallColliders.find(_OtherCollider) != m_setRightWallColliders.end())
			{
				m_setRightWallColliders.erase(_OtherCollider);

				// 벽에서 떨어진 경우
				if (m_setRightWallColliders.empty())
					m_bOnRightWall = false;
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