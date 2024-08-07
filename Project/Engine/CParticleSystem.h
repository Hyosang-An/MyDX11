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
    CStructuredBuffer*      m_ParticleBuffer = nullptr;   // 모든 파티클 정보
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr; // 파티클 활성화 숫자 전달용 버퍼

    Ptr<CTexture>           m_ParticleTex;
    float                   m_Time = 0;                 // 누적시간

    int                     m_MaxParticleCount = 100;

public:
    void SetParticleTexture(Ptr<CTexture> _Texture) { m_ParticleTex = _Texture; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) override; // 파일에 컴포넌트의 정보를 로드

public:
    CParticleSystem();
    ~CParticleSystem();
    virtual CParticleSystem* Clone() { return new CParticleSystem(*this); }
};

