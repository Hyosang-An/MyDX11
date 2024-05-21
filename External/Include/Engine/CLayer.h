#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{

public:
    CLayer(int _LayerIdx);
    ~CLayer();
    virtual CLayer* Clone() { return new CLayer(*this); }

private:
    vector<CGameObject*>    m_Parents;
    const int               m_LayerIdx;

public:
    void Begin();
    void Tick();
    void FinalTick();

public:
    int GetLayerIdx() { return m_LayerIdx; }
    void AddObject(CGameObject* _Object) { m_Parents.push_back(_Object); }
    const vector<CGameObject*>& GetParentObjects() { return m_Parents; }




};

