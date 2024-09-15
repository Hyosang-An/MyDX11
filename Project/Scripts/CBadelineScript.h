#pragma once
#include "Engine\CScript.h"

#include "CPlayerScript.h"

struct tPlayerTrail
{
	Vec3			Pos;
	PLAYER_STATE	State;
	int				SpriteIdx;
	bool			FacingRight;
	float			Time;
};

class CBadelineScript :
    public CScript
{
public:
	CBadelineScript();
	CBadelineScript(const CBadelineScript& _Other);
	~CBadelineScript();
	virtual CBadelineScript* Clone() { return new CBadelineScript(*this); }


private:
	CGameObject* m_Player = nullptr;

	Vec3 m_TrackingStartPos;
	Vec3 m_Vel_MoveToTrackingStartPos;
	bool m_bTracking = false;


	queue<tPlayerTrail> m_PlayerTrailQueue;

public:
	void SetPlayer(CGameObject* _Player) { m_Player = _Player; }

public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;

};

