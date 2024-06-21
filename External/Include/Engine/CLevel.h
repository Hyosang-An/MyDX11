#pragma once
#include "CEntity.h"

class CLayer;
class CGameObject;

class CLevel :
    public CEntity
{
public:
    CLevel();
    ~CLevel();
    virtual CLevel* Clone() { return new CLevel(*this); }

private:
    CLayer*         m_Layer[MAX_LAYER];
    LEVEL_STATE     m_State = LEVEL_STATE::STOP;




private:
    void ChangeState(LEVEL_STATE _NextState);

public:
    void AddObject(int LayerIdx, CGameObject* _Object, bool _bMoveChild = false);
    CLayer* GetLayer(int _LayerIdx) { return m_Layer[_LayerIdx]; }
    LEVEL_STATE GetState() { return m_State; }
    CGameObject* FindObjectByName(const wstring& _Name);

    void ClearObject();

public:
    void Begin();       // 레벨이 시작되는 순간 or 레벨에 추가(합류) 하는 순간
    void Tick();
    void FinalTick();

    friend class CTaskMgr;
};

