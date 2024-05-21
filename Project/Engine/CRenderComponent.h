#pragma once
#include "CComponent.h"


class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>          m_Mesh;
    Ptr<CGraphicShader> m_Shader;

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetShader(Ptr<CGraphicShader> _Shader) { m_Shader = _Shader; }

    Ptr<CMesh> GetMesh() { return m_Mesh; }
    Ptr<CGraphicShader> GetShader() { return m_Shader; }


public:
    virtual void FinalTick() = 0;
    virtual void Render() = 0;
    virtual CRenderComponent* Clone() = 0;

public:
    CRenderComponent(COMPONENT_TYPE _Type);
    ~CRenderComponent();
};

