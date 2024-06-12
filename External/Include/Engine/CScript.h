#pragma once
#include "CComponent.h"


#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CAssetMgr.h"
#include "components.h"

class CScript :
    public CComponent
{
public:


public:
    virtual void Tick() = 0;
    virtual void FinalTick() final override {}

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}
    virtual void Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}
    virtual void EndOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}

public:
    CScript();
    ~CScript();
};