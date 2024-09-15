#pragma once
#include "Engine\CScript.h"
class CRoomScript :
    public CScript
{
public:
	CRoomScript();
	CRoomScript(const CRoomScript& _Other);
	~CRoomScript();
	virtual CRoomScript* Clone() { return new CRoomScript(*this); }

private:
	bool m_StartRoom = false;
	bool m_BadelineSpawnRoom = false;
	bool m_BadenlineSpawned = false;

	Vec3 m_PlayerSpawnPos;

public:
	void FirstSpawnPlayer();
	
	Vec3 GetPlayerSpawnPos() { return m_PlayerSpawnPos; }

public:
    virtual void Begin() override;
    virtual void Tick() override;

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
};

