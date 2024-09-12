#pragma once
#include "Engine\CScript.h"
class CJumpThruScript :
    public CScript
{
public:
	CJumpThruScript();
	CJumpThruScript(const CJumpThruScript& _other);
	~CJumpThruScript();
	virtual CJumpThruScript* Clone() { return new CJumpThruScript(*this); }

public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;
	virtual void Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;
	virtual void EndOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;

};

