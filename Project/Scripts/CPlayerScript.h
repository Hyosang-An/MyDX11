#pragma once
#include "Engine/CScript.h"

enum class PLAYER_STATE
{
    IDLE,
    RUN,
    JUMP,
    FALL,
    DASH,
    CLIMB,
    DREAM_DASH,
    CHANGE_ROOM,
    DEAD,
    END
};

class CPlayerScript :
    public CScript
{
public:
    CPlayerScript();
    ~CPlayerScript();
    virtual CPlayerScript* Clone() { return new CPlayerScript(*this); }

private:
	PLAYER_STATE    m_CurState = PLAYER_STATE::IDLE;
	PLAYER_STATE    m_PrevState = PLAYER_STATE::IDLE;
	bool            m_bFacingRight = true; // 바라보고 있는 방향 (true : 오른쪽, false : 왼쪽)
	bool			m_bOnGround = false;

	int             m_CurRoomIdx = 0;


	float           m_fMagnitudeOfMoveForce = 100;
	float           m_MaxSpeedX = 200;
	float           m_fJumpSpeed = 500;
    float		    m_fDashSpeed = 700;
	float		    m_fDashTime = 0.5;
    float		    m_fDashTimeRemained = m_fDashTime;

	float           m_MaxFallingSpeed = 600;

	bool            m_bCollisionWithWall = false;

	set<CCollider2D*> m_setGroundColliders;
	set<CCollider2D*> m_setWallColliders;

    // Script
	class CRigidBody* m_pRigidBody = nullptr;




public:
	PLAYER_STATE GetCurState() { return m_CurState; }

private:
    void KeyCheck();
	void UpdateState();
	void UpdateAnimation();

public:
    virtual void Begin() override;
    virtual void Tick() override;

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;
	virtual void Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;
	virtual void EndOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
};

