#pragma once
#include "CComponent.h"


class CRenderComponent :
    public CComponent
{
public:
    CRenderComponent(COMPONENT_TYPE _Type);
    ~CRenderComponent();

private:
    Ptr<CMesh>          m_Mesh;
    Ptr<CMaterial>      m_Mtrl;

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetMaterial(Ptr<CMaterial> _mtrl) { m_Mtrl = _mtrl; }

    Ptr<CMesh> GetMesh() { return m_Mesh; }
    Ptr<CMaterial> GetMaterial() { return m_Mtrl; }


public:
    virtual void FinalTick() = 0;
    virtual void Render() = 0;
    virtual CRenderComponent* Clone() = 0;
};

