#pragma once
#include "Engine\CScript.h"
class CRigidBody :
    public CScript
{
public:
	CRigidBody();
	CRigidBody(const CRigidBody& _other);
	~CRigidBody();
	virtual CRigidBody* Clone() { return new CRigidBody(*this); }

private:
	float	m_fMass = 1.f;
	Vec3	m_vVelocity = Vec3(0.f, 0.f, 0.f);
	Vec3	m_vAcceleration = Vec3(0.f, 0.f, 0.f);
	Vec3	m_vForce = Vec3(0.f, 0.f, 0.f);

	bool	m_bUseGravity = true;
	float	m_fGravityAccelaration = 2500.f;

	float	m_fMaxSpeed = 0.f; // 0.f이면 제한 없음
	float   m_fMaxFallingSpeed = 1000.f; // 0.f이면 제한 없음

	bool	m_bOnGround = false;
	bool	m_bIsDangle = false;
	bool	m_bIsDash = false;

public:
	void SetMass(float _fMass) { m_fMass = _fMass; }
	void SetVelocity(Vec3 _vVelocity);
	void SetAcceleration(Vec3 _vAcceleration) { m_vAcceleration = _vAcceleration; }
	void SetForce(Vec3 _vForce) { m_vForce = _vForce; }
	void AddForce(Vec3 _vForce) { m_vForce += _vForce; }

	void SetUseGravity(bool _bUseGravity) { m_bUseGravity = _bUseGravity; }
	void SetGravityAccelaration(float _fGravityAccelaration) { m_fGravityAccelaration = _fGravityAccelaration; }

	void SetMaxSpeed(float _fMaxSpeed) { m_fMaxSpeed = _fMaxSpeed; }
	void SetMaxFallingSpeed(float _fMaxFallingSpeed) { m_fMaxFallingSpeed = _fMaxFallingSpeed; }

	void OnLand();
	void OnWallHang();

	void SetOnGround(bool _bOnGround);
	bool IsOnGround() { return m_bOnGround; }

	Vec3 GetVelocity() { return m_vVelocity; }

public:
    virtual void Begin() override;
    virtual void Tick() override;

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
};

