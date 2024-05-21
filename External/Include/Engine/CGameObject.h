#pragma once
#include "CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;

class CGameObject :
    public CEntity
{
public:
    CGameObject();
    ~CGameObject();
    virtual CGameObject* Clone() { return new CGameObject(*this); }

private:
    CComponent*         m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*   m_RenderCom;

    vector<CScript*>    m_vecScript;

public:
    void AddComponent(CComponent* _Comopnent);
    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[(UINT)_Type]; }

    class CTransform* Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
    class CMeshRender* MeshRender() { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
    class CCamera* Camera() { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }

public:
    void Begin();
    void Tick();
    void FinalTick();
    void Render();



};

