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
    DEATH,
    END
};

class CPlayerScript :
    public CScript
{
public:
    CPlayerScript();
	CPlayerScript(const CPlayerScript& _Other);
    ~CPlayerScript();
    virtual CPlayerScript* Clone() { return new CPlayerScript(*this); }

private:
	PLAYER_STATE    m_CurState = PLAYER_STATE::IDLE;
	PLAYER_STATE    m_PrevState = PLAYER_STATE::IDLE;
	bool            m_bFacingRight = true; // 바라보고 있는 방향 (true : 오른쪽, false : 왼쪽)
	bool			m_bOnGround = false;

	//int             m_CurRoomIdx = 0;

	CGameObject*	m_Room = nullptr;


	float           m_MagnitudeOfMoveForce = 1000;
	float           m_MaxRunSpeed = 200;
	float           m_JumpSpeed = 500;
    float		    m_DashSpeed = 700;
	float           m_ClimbSpeed = 100;

	float           m_SpeedBeforeDash = 0;
    
	int             m_DashCount = 1;
	float		    m_DashTime = 0.5;
    float		    m_DashTimeRemained = m_DashTime;

	float			m_DashTrailInterval = 0.15;
	float			m_DashTrailTimeSinceLastTrail = 0;

	float           m_MaxFallingSpeed = 600;

	bool            m_bOnLeftWall = false;
	bool            m_bOnRightWall = false;

	set<CCollider2D*> m_setGroundColliders;
	set<CCollider2D*> m_setLeftWallColliders;
	set<CCollider2D*> m_setRightWallColliders;

    // Script
	class CRigidBody* m_RigidBody = nullptr;




public:
	PLAYER_STATE GetCurState() { return m_CurState; }
	void ChangeRoom(CGameObject* _Room);
	CGameObject* GetRoom() { return m_Room; }

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

