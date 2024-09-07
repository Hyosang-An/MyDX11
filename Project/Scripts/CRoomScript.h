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
	CGameObject* m_PlayerSpawnPoint = nullptr;

public:
	void FirstSpawnPlayer();
	void RespawnPlayer();

public:
    virtual void Begin() override;
    virtual void Tick() override;

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
};

