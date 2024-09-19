#pragma once
#include "CRenderComponent.h"

#include "CMyParticleTickCS.h"

class CStructuredBuffer;

// Particle
struct tMyParticle
{
    int		Active;

    int     Type; // 0 : 배경 눈발 파티클, 1 : 대쉬 파티클

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


    float	Padding;
};

struct tMySpawnCount
{
    UINT    SpawnCount;
    UINT    padding[3];
};

// My Particle Module
struct tMyParticleModule
{
	// Type
	UINT	Type;					// 0 : 배경 눈발 파티클, 1 : 대쉬 파티클

	// 기준 방향
	Vec3	ReferenceDir;			// 기준 방향

	// Spawn
	UINT	SpawnRate;				// 초당 파티클 생성개수 (Spawn Per Second)
	Vec4	vSpawnColor;			// 생성 시점 색상
	Vec4	vSpawnMinScale;			// 생성 시 최소 크기
	Vec4	vSpawnMaxScale;			// 생성 시 최대 크기

	float	MinLife;				// 최소 수명
	float	MaxLife;				// 최대 수명

	UINT	SpawnShape;				// 스폰 생성 구역 0 : Box,  1: Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeScale.x == Radius

	UINT	BlockSpawnShape;		// 스폰되지 않을 구역 0 : Box,  1: Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeScale.x == Radius

	UINT	SpaceType;				// 파티클 생성 위치 기준 0 : LocalSpace, 1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// 한번에 발생시키는 Particle 수
	UINT	SpawnBurstRepeat;
	float	SpawnBurstRepeatTime;

	// Add Velocity
	UINT	AddVelocityType;		// 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
	Vec3	AddVelocityFixedDir;
	float	AddMinSpeed;
	float	AddMaxSpeed;

	// Scale Module
	float	StartScale;
	float	EndScale;

	// Drag Module
	float	DestNormalizedAge;
	float	TargetSpeed;

	// Noise Force Module
	float	NoiseForceTerm;		// Noise Force 적용시키는 텀
	float	NoiseForceScale;	// Noise Force 크기

	// Render Module
	Vec3	EndColor;			// 최종 색상
	UINT	FadeOut;			// 0 : Off, 1 : Normalized Age
	float	FadeOutStartRatio;	// FadeOut 효과가 시작되는 Normalized Age 지점
	UINT	VelocityAlignment;	// 속도 정렬 0 : Off, 1 : On


	// Module On / Off
	int		isModuleOn[(UINT)PARTICLE_MODULE::END];

	// Padding
	//float	Padding[4];
};


class CMyParticleSystem :
    public CRenderComponent
{
public:
	// Particle Type 0(기본값) : 배경 눈발 파티클, 1 : 대쉬 파티클
	CMyParticleSystem(UINT _particleType = 0);
	CMyParticleSystem(const CMyParticleSystem& _Other);
	~CMyParticleSystem();
	virtual CMyParticleSystem* Clone() { return new CMyParticleSystem(*this); }

private:
    Ptr<CMyParticleTickCS>  m_TickCS;
    CStructuredBuffer*      m_ParticleBuffer = nullptr;		// 모든 파티클 정보
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr;	// 파티클 활성화 숫자 전달용 버퍼
	CStructuredBuffer*		m_ModuleBuffer = nullptr;					// Module Data Buffer

	tMyParticleModule       m_Module;               // 파티클의 기능 정의

	//int                     m_Type = 0;                 // 파티클 타입 0 : 배경 눈발 파티클, 1 : 대쉬 파티클

	Ptr<CTexture>           m_ParticleTex;					// 파티클 텍스쳐
    float                   m_Time = 0;                 // 누적시간
    float					m_prevSpawnCountFraction = 0.f;	// 이전프레임에서 1프레임당 생성되어야하는 파티클 개수의 소수부분

    int                     m_MaxParticleCount = 1000;	// 파티클 최대 개수
	//float                   m_SpawnRate = 10.f;			// 초당 생성되어야하는 파티클 개수

	bool                    m_DashParticleInit = false; // 대쉬 파티클 초기화 여부
	float					m_DashParticleLifeTime = 3.f;	// 대쉬 파티클 수명


public:
    void SetParticleTexture(Ptr<CTexture> _Texture);
    void CaculateSpawnCount();

	void SetType(int _Type) { m_Module.Type = _Type; }
	void SetReferenceDir(Vec3 _Dir);

public:
	virtual void Init() override;
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) override; // 파일에 컴포넌트의 정보를 로드
};

