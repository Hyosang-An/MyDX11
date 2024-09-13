#pragma once
#include "CRenderComponent.h"

#include "CMyParticleTickCS.h"

class CStructuredBuffer;

// Particle
struct tMyParticle
{
    int		Active;

    Vec4	vColor;
    Vec3	vLocalPos;
    Vec3	vWorldPos;
    Vec3	vWorldInitPos;
    Vec3	vWorldInitScale;
    Vec3	vWorldCurrentScale;
    Vec3	vWorldRotation;

    Vec3	vForce;				// Particle 에 누적된 힘
    Vec3	vVelocity;			// Particle 의 속도

    float	NoiseForceAccTime;	// Noise Force 텀에 도달하는 누적 시간
    Vec3	NoiseForceDir;		// Noise Force 의 방향

    float	Mass;
    float	Life;
    float	Age;
    float	NormalizedAge;

	float   RandomValue;

    Vec2	Padding;
};

struct tMySpawnCount
{
    UINT    SpawnCount;
    UINT    padding[3];
};


class CMyParticleSystem :
    public CRenderComponent
{
public:
	CMyParticleSystem();
	CMyParticleSystem(const CMyParticleSystem& _Other);
	~CMyParticleSystem();
	virtual CMyParticleSystem* Clone() { return new CMyParticleSystem(*this); }

private:
    Ptr<CMyParticleTickCS>  m_TickCS;
    CStructuredBuffer*      m_ParticleBuffer = nullptr;		// 모든 파티클 정보
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr;	// 파티클 활성화 숫자 전달용 버퍼

	Ptr<CTexture>           m_ParticleTex;					// 파티클 텍스쳐
    float                   m_Time = 0;                 // 누적시간
    float					m_prevSpawnCountFraction = 0.f;	// 이전프레임에서 1프레임당 생성되어야하는 파티클 개수의 소수부분

    int                     m_MaxParticleCount = 1000;	// 파티클 최대 개수
	float                   m_SpawnRate = 10.f;			// 초당 생성되어야하는 파티클 개수



public:
    void SetParticleTexture(Ptr<CTexture> _Texture);
    void CaculateSpawnCount();

public:

    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) override; // 파일에 컴포넌트의 정보를 로드
};

