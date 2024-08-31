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
	// 2D ��Ʈ������ ����ȭ���� ���� ���̹Ƿ� ������Ʈ ���� �� ��� ��ü�� DynamicMaterial�� ���� ��� �־ �ȴ�.
	GetRenderComponent()->GetDynamicMaterial();

	m_pRigidBody = GetOwner()->GetScript<CRigidBody>();
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
	//OnGround ���� ������Ʈ
	m_bOnGround = m_pRigidBody->IsOnGround();

	// IDLE
	if (m_bOnGround && !KEY_PRESSED(KEY::RIGHT) && !KEY_PRESSED(KEY::LEFT))
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
		// 8���� ���⿡ ���� �뽬 ó��
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
	// �Ʒ� �������� �������鼭, �뽬�� �帲 �뽬 ���°� �ƴϸ� FALL ���·� ��ȯ
	if (m_pRigidBody->GetVelocity().y < 0 && m_CurState != PLAYER_STATE::DASH && m_CurState != PLAYER_STATE::DREAM_DASH)
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

				// �ӵ� ����ȭ
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
			// TODO : �Ŵ޸��� ���� ó��
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
}

void CPlayerScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// �浹�� ������Ʈ�� Layer�� ���� �з�
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			// �ϴ� �İ�� ���̸�ŭ ����
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

			// ���� �浹�Ѱ��� ���� �浹�Ѱ��� �Ǵ�
			if (abs(vOverlap.y) > abs(vOverlap.x))
			{
				// ���� �浹�� ���
				if (m_pRigidBody->GetVelocity().y < 0.f && !m_pRigidBody->IsOnGround())
				{
					m_pRigidBody->OnLand();
					m_setGroundColliders.insert(_OtherCollider);

					// �뽬 �ʱ�ȭ
					m_fDashTimeRemained = m_fDashTime;

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
					m_pRigidBody->SetVelocity(Vec3(m_pRigidBody->GetVelocity().x, 0.f, 0.f));

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
				m_pRigidBody->SetVelocity(Vec3(0.f, m_pRigidBody->GetVelocity().y, 0.f));
				m_bCollisionWithWall = true;
				m_setWallColliders.insert(_OtherCollider);

				// �뽬 ���¿��� ���� ������ �뽬 ���� ����
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
	// �浹�� ������Ʈ�� Layer�� ���� �з�
	switch (LAYER(_OtherObject->GetLayerIdx()))
	{
		case LAYER::WALL_OR_GROUND:
		{
			// �ϴ� �İ�� ���̸�ŭ ����
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
			// Ground����, Wall���� �Ǵ�
			if (m_setGroundColliders.find(_OtherCollider) != m_setGroundColliders.end())
			{
				m_setGroundColliders.erase(_OtherCollider);

				// ������ ������ ���
				if (m_setGroundColliders.empty())
					m_pRigidBody->SetOnGround(false);
			}
			else if (m_setWallColliders.find(_OtherCollider) != m_setWallColliders.end())
			{
				m_setWallColliders.erase(_OtherCollider);

				// ������ ������ ���
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