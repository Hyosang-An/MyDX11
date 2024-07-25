#pragma once
#include "CEntity.h"

#include "CGameObject.h"

class CComponent :
    public CEntity
{
    friend class CGameObject;
public:
    CComponent(COMPONENT_TYPE _Type);
    CComponent(const CComponent& _Origin);
    ~CComponent();
    virtual CComponent* Clone() = 0;

private:
    const COMPONENT_TYPE    m_Type;
    CGameObject*            m_Owner;

public:
    COMPONENT_TYPE GetComponentType() { return m_Type; }
    CGameObject* GetOwner() { return m_Owner; }

    virtual void Init() {}
    virtual void Begin();
    virtual void Tick();
    virtual void FinalTick() = 0;

    virtual void SaveToFile(FILE* _File) = 0;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) = 0; // 파일에 컴포넌트의 정보를 로드

    CTransform* Transform() { return m_Owner->Transform(); }
    CMeshRender* MeshRender() { return m_Owner->MeshRender(); }
    CCamera* Camera() { return m_Owner->Camera(); }
    CCollider2D* Collider2D() { return m_Owner->Collider2D(); }
    CFlipBookComponent* FlipBookComponent() { return m_Owner->FlipBookComponent(); }
    CLight2D* Light2D() { return m_Owner->Light2D(); }
    


private:
    void SetOwner(CGameObject* _obj) { m_Owner = _obj; }
};

