#include "pch.h"
#include "CPlayerScript.h"
#include "CRigidBody.h"
#include "Engine/CCollisionMgr.h"

CPlayerScript::CPlayerScript() :
	CScript(UINT(SCRIPT_TYPE::PLAYERSCRIPT))
{
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "MagnitudeOfMoveForce", &m_MagnitudeOfMoveForce);
}

CPlayerScript::CPlayerScript(const CPlayerScript& _Other) :
	CScript(UINT(SCRIPT_TYPE::PLAYERSCRIPT))
{
	// _other���� ������ ���� ����
	m_CurRoomIdx = 0;


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
	// 2D ��Ʈ������ ����ȭ���� ���� ���̹Ƿ� ������Ʈ ���� �� ��� ��ü�� DynamicMaterial�� ���� ��� �־ �ȴ�.
	GetRenderComponent()->GetDynamicMaterial();

	m_RigidBody = GetOwner()->GetScript<CRigidBody>();
}

void CPlayerScript::Tick()
{
	 // �̵�: �����
	 // ����: c
	 // �뽬: x
	 // �Ŵ޸���: z 

	 // Ű���� �Է� üũ
	 KeyCheck();

	 // ���� ������Ʈ
	 UpdateState();

	 // �ִϸ��̼� ������Ʈ
	 UpdateAnimation();

	 m_PrevState = m_CurState;
}

void CPlayerScript::KeyCheck()
{
	// �����
	auto owner = GetOwner();

	//OnGround ���� ������Ʈ
	m_bOnGround = m_RigidBody->IsOnGround();

	// IDLE
	if (m_bOnGround && !KEY_PRESSED(KEY::RIGHT) && !KEY_PRESSED(KEY::LEFT) && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
	{
		m_CurState = PLAYER_STATE::IDLE;
	}

	// �ٶ󺸴� ���� ������Ʈ
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
	if ((m_bOnLeftWall || m_bOnRightWall) && (KEY_JUST_PRESSED(KEY::Z) || KEY_PRESSED(KEY::Z)) && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
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

		// 8���� ���⿡ ���� �뽬 ó��
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

		// ����Ű�� ������ ���� ���
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
	// TODO : �帲 �뽬 ����

	// CHANGE ROOM
	// TODO : �� �ٲٱ� ����

	// DEAD
	// TODO : ���� ó�� ����

	// FALL
	//if (m_pRigidBody->GetVelocity().y < 0 && m_CurState != )
	//{
	//	m_CurState = PLAYER_STATE::FALL;
	//}
	// �Ʒ� �������� �������鼭, CLIMB�� �뽬�� �帲 �뽬 ���°� �ƴϸ� FALL ���·� ��ȯ
	if (m_RigidBody->GetVelocity().y < 0 && m_CurState !=PLAYER_STATE::CLIMB && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
	{
		m_CurState = PLAYER_STATE::FALL;
	}
}

void CPlayerScript::UpdateState()
{
	// Switch���� ����Ͽ� ���¿� ���� ���� ó��
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
			// ���� ������ �� ���� �ӵ� 0
			if (m_bFacingRight && m_bOnRightWall)
			{
				m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
			}
			else if (!m_bFacingRight && m_bOnLeftWall)
			{
				m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
			}

			// �ٴڿ� ������ �ٴ� ���� �ӵ� 0
			if (m_RigidBody->GetVelocity().y < 0.f && m_RigidBody->IsOnGround())
			{
				m_RigidBody->SetVelocity(Vec3(m_RigidBody->GetVelocity().x, 0, 0));
			}

			if (m_PrevState != PLAYER_STATE::DASH)
			{
				m_DashTimeRemained = m_DashTime;
			}

			m_DashTimeRemained -= DT;

			if (m_DashTimeRemained <= 0)
			{
				if (m_bOnGround)
					m_CurState = PLAYER_STATE::IDLE;
				else
					m_CurState = PLAYER_STATE::FALL;

				// �ӵ� ����ȭ
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

			// ����
			if (KEY_JUST_PRESSED(KEY::C))
			{
				Vec3 vVelocity;
				if (m_bFacingRight)
				{
					vVelocity = Vec3(m_MaxRunSpeed, m_JumpSpeed, 0);
				}
				else
				{
					vVelocity = Vec3(-m_MaxRunSpeed, m_JumpSpeed, 0);
				}
				
				m_RigidBody->SetVelocity(vVelocity);
				m_RigidBody->SetOnGround(false);
				m_CurState = PLAYER_STATE::JUMP;
			}

			// �뽬
			if (KEY_JUST_PRESSED(KEY::X) && m_DashCount > 0)
			{
				--m_DashCount;
				m_SpeedBeforeDash = m_RigidBody->GetVelocity().Length();

				Vec3 vDir;

				// 8���� ���⿡ ���� �뽬 ó��
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

				// ����Ű�� ������ ���� ���
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
			// TODO : �帲 �뽬 ���� ó��
			break;
		}

		case PLAYER_STATE::CHANGE_ROOM:
		{
			// TODO : �� �ٲٴ� ���� ó��
			break;
		}

		case PLAYER_STATE::DEAD:
		{
			// TODO : ���� ó��
			break;
		}
	}
}

void CPlayerScript::UpdateAnimation()
{
	// CLIMB�� ���� ó��
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
		case PLAYER_STATE::DEAD:
			break;
		case PLAYER_STATE::END:
			break;
		default:
			break;
	}
}

void CPlayerScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (m_RigidBody == nullptr)
	return;

	// �浹�� ������Ʈ�� Layer�� ���� �з�
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			Vec3 vOverlap = CCollisionMgr::GetInst()->GetOverlap();


			Vec3 vColliderPos = _OwnCollider->GetWorldPos();
			Vec3 vOtherColliderPos = _OtherCollider->GetWorldPos();
			Vec3 vDir = vOtherColliderPos - vColliderPos;

			// �ϴ� �İ�� ���̸�ŭ ����
			//if (vOverlap.Dot(vDir) > 0)
			//{
			//	vOverlap = -vOverlap;
			//}

			//Vec3 vObjPos = Transform()->GetWorldPos();
			//vObjPos += vOverlap;
			//Transform()->SetWorldPos(vObjPos);

			// ���� �浹�Ѱ��� ���� �浹�Ѱ��� �Ǵ�
			if (abs(vOverlap.y) > abs(vOverlap.x))
			{
				// ���� �浹�� ���
				if (vDir.y < 0 && m_RigidBody->GetVelocity().y < 0.f && !m_RigidBody->IsOnGround())
				{
					m_RigidBody->OnLand();
					m_setGroundColliders.insert(_OtherCollider);

					// �뽬 �ʱ�ȭ
					m_DashTimeRemained = m_DashTime;

					// �뽬 ���¿��� ���� ������ �뽬 ���� ����
					if (m_CurState == PLAYER_STATE::DASH)
					{
						m_CurState = PLAYER_STATE::IDLE;
					}

					// �帲 �뽬 ���¿��� ���� ������ DEAD
					if (m_CurState == PLAYER_STATE::DREAM_DASH)
					{
						m_CurState = PLAYER_STATE::DEAD;
					}
				}
				else
				{
					// õ��� �浹�� ���
					m_RigidBody->SetVelocity(Vec3(m_RigidBody->GetVelocity().x, 0.f, 0.f));

					// ����	���¿��� õ�忡 ������ ���� ���� ����
					if (m_CurState == PLAYER_STATE::JUMP)
					{
						m_CurState = PLAYER_STATE::FALL;
					}

					// �뽬 ���¿��� õ�忡 ������ �뽬 ���� ����
					if (m_CurState == PLAYER_STATE::DASH)
					{
						m_CurState = PLAYER_STATE::FALL;
					}

					// �帲 �뽬 ���¿��� õ�忡 ������ DEAD
					if (m_CurState == PLAYER_STATE::DREAM_DASH)
					{
						m_CurState = PLAYER_STATE::DEAD;
					}
				}
			}
			else
			{
				// ���� �浹�� ���
				m_RigidBody->SetVelocity(Vec3(0.f, m_RigidBody->GetVelocity().y, 0.f));
				if (vDir.x > 0)
				{
					m_setRightWallColliders.insert(_OtherCollider);
					m_bOnRightWall = true;
				}
				else
				{
					m_setLeftWallColliders.insert(_OtherCollider);
					m_bOnLeftWall = true;
				}

				// �뽬 ���¿��� ���� ������ �ش� ������ �ӵ� 0
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

					if (m_bOnGround)
					{
						m_CurState = PLAYER_STATE::IDLE;
					}
					else
					{
						m_CurState = PLAYER_STATE::FALL;
					}
				}

				// �帲 �뽬 ���¿��� ���� ������ DEAD
				if (m_CurState == PLAYER_STATE::DREAM_DASH)
				{
					m_CurState = PLAYER_STATE::DEAD;
				}

			}
			break;
		}

		case LAYER::SPIKE:
		{
			// TODO : ���� ó��

		}
		break;
		

	}
}

void CPlayerScript::Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (m_RigidBody == nullptr)
		return;

	// �浹�� ������Ʈ�� Layer�� ���� �з�
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			// �� ���� �ӵ� 0
			if (m_bFacingRight && m_bOnRightWall)
			{
				m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
			}
			else if (!m_bFacingRight && m_bOnLeftWall)
			{
				m_RigidBody->SetVelocity(Vec3(0, m_RigidBody->GetVelocity().y, 0));
			}

			// �ٴ� ���� �ӵ� 0
			if (m_RigidBody->GetVelocity().y < 0.f && m_RigidBody->IsOnGround())
			{
				m_RigidBody->SetVelocity(Vec3(m_RigidBody->GetVelocity().x, 0, 0));
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
			// Ground����, Wall���� �Ǵ�
			if (m_setGroundColliders.find(_OtherCollider) != m_setGroundColliders.end())
			{
				m_setGroundColliders.erase(_OtherCollider);

				// ������ ������ ���
				if (m_setGroundColliders.empty())
					m_RigidBody->SetOnGround(false);
			}

			else if (m_setLeftWallColliders.find(_OtherCollider) != m_setLeftWallColliders.end())
			{
				m_setLeftWallColliders.erase(_OtherCollider);

				// ������ ������ ���
				if (m_setLeftWallColliders.empty())
					m_bOnLeftWall = false;
			}

			else if (m_setRightWallColliders.find(_OtherCollider) != m_setRightWallColliders.end())
			{
				m_setRightWallColliders.erase(_OtherCollider);

				// ������ ������ ���
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