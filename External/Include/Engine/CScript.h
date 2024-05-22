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

public:
    CScript();
    ~CScript();
};