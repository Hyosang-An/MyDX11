#pragma once
#include "Engine\CScript.h"
class CRefillScript :
    public CScript
{
public:
	CRefillScript();
	CRefillScript(const CRefillScript& _Other);
	~CRefillScript();
	virtual CRefillScript* Clone() { return new CRefillScript(*this); }

private:


public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;

};

