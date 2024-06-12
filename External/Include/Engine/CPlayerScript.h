#pragma once
#include "CScript.h"
class CPlayerScript :
    public CScript
{
public:
    CPlayerScript();
    ~CPlayerScript();
    virtual CPlayerScript* Clone() { return new CPlayerScript(*this); }

private:
    float   m_Speed;

public:
    virtual void Tick() override;

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

};

