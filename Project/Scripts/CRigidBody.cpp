#include "pch.h"
#include "CRigidBody.h"

#include "CPlayerScript.h"

CRigidBody::CRigidBody() :
	CScript((UINT)SCRIPT_TYPE::RIGIDBODY)
{
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Max Speed", &m_fMaxSpeed);
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Max Falling Speed", &m_fMaxFallingSpeed);
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Gravity Accelaration", &m_fGravityAccelaration);

	// 현재 속도
	AddScriptParam(SCRIPT_PARAM_TYPE::VEC3, "Velocity", &m_vVelocity);
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::Begin()
{
	GetOwner()->GetScript<CPlayerScript>();
}

void CRigidBody::Tick()
{
	// 현재 플레이어 상태
	PLAYER_STATE curState = GetOwner()->GetScript<CPlayerScript>()->GetCurState();

	// 중력 이외의 외력으로 인한 가속도
	Vec3 Acceleration = m_vForce / m_fMass;	

	// 땅에 있지 않으면서, 대쉬 및 드림 대쉬 상태가 아닐 때 중력 가속도 적용
	if (m_bUseGravity && !m_bOnGround && 
		curState != PLAYER_STATE::DASH &&
		curState != PLAYER_STATE::DREAM_DASH)
	{
		Acceleration.y -= m_fGravityAccelaration;
	}

	// 속도 업데이트
	m_vVelocity += Acceleration * DT;

	// 최대 속도 제한
	float speed = m_vVelocity.Length();
	if (m_fMaxSpeed > 0.f && m_fMaxSpeed < speed)
	{
		Vec3 vDir = m_vVelocity.Normalize();
		m_vVelocity = m_fMaxSpeed * vDir;
	}

	// 최대 낙하 속도 제한
	if (m_bUseGravity && !m_bOnGround && curState != PLAYER_STATE::DASH)
	{
		if (m_fMaxFallingSpeed > 0.f && m_fMaxFallingSpeed < -m_vVelocity.y)
		{
			m_vVelocity.y = -m_fMaxFallingSpeed;
		}
	}

	// 이동
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	vPos += m_vVelocity * DT;
	GetOwner()->Transform()->SetRelativePos(vPos);

	// 힘 초기화
	m_vForce = Vec3(0.f, 0.f, 0.f);
}

void CRigidBody::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CRigidBody::SaveToFile(FILE* _File)
{
}

void CRigidBody::LoadFromFile(FILE* _File)
{
}

void CRigidBody::OnLand()
{
	m_bOnGround = true;
	m_bIsDangle = false;

	m_vVelocity.y = 0.f;
}

void CRigidBody::OnWallHang()
{
}