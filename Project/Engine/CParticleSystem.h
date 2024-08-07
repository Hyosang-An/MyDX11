#pragma once
#include "CRenderComponent.h"

#include "CParticleTickCS.h"

class CStructuredBuffer;

// Particle
struct tParticle
{
	Vec3	vLocalPos;
	Vec3	vWorldPos;
    Vec3	vWorldScale;
	Vec4	vColor;

    Vec3	vForce;
    Vec3	vVelocity;

    float	Mass;
    int		Active;

    int		Padding[3];
};

struct tSpawnCount
{
    UINT    iSpawnCount;
    UINT    padding[3];
};

class CParticleSystem :
    public CRenderComponent
{
private:
    Ptr<CParticleTickCS>    m_TickCS;
    CStructuredBuffer*      m_ParticleBuffer = nullptr;   // ��� ��ƼŬ ����
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr; // ��ƼŬ Ȱ��ȭ ���� ���޿� ����

    Ptr<CTexture>           m_ParticleTex;
    float                   m_Time = 0;                 // �����ð�

    int                     m_MaxParticleCount = 100;

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

