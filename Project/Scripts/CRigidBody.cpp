#include "pch.h"
#include "CRigidBody.h"

#include "CPlayerScript.h"

CRigidBody::CRigidBody() :
	CScript((UINT)SCRIPT_TYPE::RIGIDBODY)
{
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Max Speed", &m_fMaxSpeed);
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Max Falling Speed", &m_fMaxFallingSpeed);
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Gravity Accelaration", &m_fGravityAccelaration);

	// ���� �ӵ�
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
	// ���� �÷��̾� ����
	PLAYER_STATE curState = GetOwner()->GetScript<CPlayerScript>()->GetCurState();

	// �߷� �̿��� �ܷ����� ���� ���ӵ�
	Vec3 Acceleration = m_vForce / m_fMass;	

	// ���� ���� �����鼭, �뽬 �� �帲 �뽬 ���°� �ƴ� �� �߷� ���ӵ� ����
	if (m_bUseGravity && !m_bOnGround && 
		curState != PLAYER_STATE::DASH &&
		curState != PLAYER_STATE::DREAM_DASH)
	{
		Acceleration.y -= m_fGravityAccelaration;
	}

	// �ӵ� ������Ʈ
	m_vVelocity += Acceleration * DT;

	// �ִ� �ӵ� ����
	float speed = m_vVelocity.Length();
	if (m_fMaxSpeed > 0.f && m_fMaxSpeed < speed)
	{
		Vec3 vDir = m_vVelocity.Normalize();
		m_vVelocity = m_fMaxSpeed * vDir;
	}

	// �ִ� ���� �ӵ� ����
	if (m_bUseGravity && !m_bOnGround && curState != PLAYER_STATE::DASH)
	{
		if (m_fMaxFallingSpeed > 0.f && m_fMaxFallingSpeed < -m_vVelocity.y)
		{
			m_vVelocity.y = -m_fMaxFallingSpeed;
		}
	}

	// �̵�
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	vPos += m_vVelocity * DT;
	GetOwner()->Transform()->SetRelativePos(vPos);

	// �� �ʱ�ȭ
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