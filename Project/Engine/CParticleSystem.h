#pragma once
#include "CRenderComponent.h"

#include "CParticleTickCS.h"

class CStructuredBuffer;

// Particle
struct tParticle
{
	Vec3	vLocalPos;
	Vec3	vWorldPos;
	Vec4	vColor;

    Vec3	vForce;
    Vec3	vVelocity;

    float	Mass;
    int		Active;

    int		Padding[2];
};

class CParticleSystem :
    public CRenderComponent
{
private:
    CStructuredBuffer*      m_ParticleBuffer;
    int                     m_MaxParticleCount;
    Ptr<CParticleTickCS>    m_TickCS;

    Ptr<CTexture>           m_ParticleTex;

public:
    void SetParticleTexture(Ptr<CTexture> _Texture) { m_ParticleTex = _Texture; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // ���Ͽ� ������Ʈ�� ������ ����
    virtual void LoadFromFile(FILE* _File) override; // ���Ͽ� ������Ʈ�� ������ �ε�

public:
    CParticleSystem();
    ~CParticleSystem();
    virtual CParticleSystem* Clone() { return new CParticleSystem(*this); }
};

