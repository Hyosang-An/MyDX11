#pragma once
#include "CEntity.h"

#include "CGameObject.h"

class CComponent :
    public CEntity
{
    friend class CGameObject;
public:
    CComponent(COMPONENT_TYPE _Type);
    ~CComponent();
    virtual CComponent* Clone() = 0;

private:
    const COMPONENT_TYPE    m_Type;
    CGameObject*            m_Owner;

public:
    COMPONENT_TYPE GetComponentType() { return m_Type; }
    CGameObject* GetOwner() { return m_Owner; }

    virtual void Begin();
    virtual void Tick();
    virtual void FinalTick() = 0;

    class CTransform* Transform() { return m_Owner->Transform(); }
    class CMeshRender* MeshRender() { return m_Owner->MeshRender(); }
    class CCamera* Camera() { return m_Owner->Camera(); }
    class CCollider2D* Collider2D() { return m_Owner->Collider2D(); }


private:
    void SetOwner(CGameObject* _obj) { m_Owner = _obj; }
};

