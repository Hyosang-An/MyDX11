#pragma once
#include "CRenderComponent.h"

#include "CMyParticleTickCS.h"

class CStructuredBuffer;

// Particle
struct tMyParticle
{
    int		Active;

    int     Type; // 0 : ��� ���� ��ƼŬ, 1 : �뽬 ��ƼŬ

    Vec4	vColor;
    Vec3	vLocalPos;
    Vec3	vWorldPos;
    Vec3	vWorldInitPos;
    Vec3	vWorldInitScale;
    Vec3	vWorldCurrentScale;
    Vec3	vWorldRotation;

    Vec3	vForce;				// Particle �� ������ ��
    Vec3	vVelocity;			// Particle �� �ӵ�

    float	NoiseForceAccTime;	// Noise Force �ҿ� �����ϴ� ���� �ð�
    Vec3	NoiseForceDir;		// Noise Force �� ����

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
	UINT	Type;					// 0 : ��� ���� ��ƼŬ, 1 : �뽬 ��ƼŬ

	// ���� ����
	Vec3	ReferenceDir;			// ���� ����

	// Spawn
	UINT	SpawnRate;				// �ʴ� ��ƼŬ �������� (Spawn Per Second)
	Vec4	vSpawnColor;			// ���� ���� ����
	Vec4	vSpawnMinScale;			// ���� �� �ּ� ũ��
	Vec4	vSpawnMaxScale;			// ���� �� �ִ� ũ��

	float	MinLife;				// �ּ� ����
	float	MaxLife;				// �ִ� ����

	UINT	SpawnShape;				// ���� ���� ���� 0 : Box,  1: Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeScale.x == Radius

	UINT	BlockSpawnShape;		// �������� ���� ���� 0 : Box,  1: Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeScale.x == Radius

	UINT	SpaceType;				// ��ƼŬ ���� ��ġ ���� 0 : LocalSpace, 1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// �ѹ��� �߻���Ű�� Particle ��
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
	float	NoiseForceTerm;		// Noise Force �����Ű�� ��
	float	NoiseForceScale;	// Noise Force ũ��

	// Render Module
	Vec3	EndColor;			// ���� ����
	UINT	FadeOut;			// 0 : Off, 1 : Normalized Age
	float	FadeOutStartRatio;	// FadeOut ȿ���� ���۵Ǵ� Normalized Age ����
	UINT	VelocityAlignment;	// �ӵ� ���� 0 : Off, 1 : On


	// Module On / Off
	int		isModuleOn[(UINT)PARTICLE_MODULE::END];

	// Padding
	//float	Padding[4];
};


class CMyParticleSystem :
    public CRenderComponent
{
public:
	// Particle Type 0(�⺻��) : ��� ���� ��ƼŬ, 1 : �뽬 ��ƼŬ
	CMyParticleSystem(UINT _particleType = 0);
	CMyParticleSystem(const CMyParticleSystem& _Other);
	~CMyParticleSystem();
	virtual CMyParticleSystem* Clone() { return new CMyParticleSystem(*this); }

private:
    Ptr<CMyParticleTickCS>  m_TickCS;
    CStructuredBuffer*      m_ParticleBuffer = nullptr;		// ��� ��ƼŬ ����
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr;	// ��ƼŬ Ȱ��ȭ ���� ���޿� ����
	CStructuredBuffer*		m_ModuleBuffer = nullptr;					// Module Data Buffer

	tMyParticleModule       m_Module;               // ��ƼŬ�� ��� ����

	//int                     m_Type = 0;                 // ��ƼŬ Ÿ�� 0 : ��� ���� ��ƼŬ, 1 : �뽬 ��ƼŬ

	Ptr<CTexture>           m_ParticleTex;					// ��ƼŬ �ؽ���
    float                   m_Time = 0;                 // �����ð�
    float					m_prevSpawnCountFraction = 0.f;	// ���������ӿ��� 1�����Ӵ� �����Ǿ���ϴ� ��ƼŬ ������ �Ҽ��κ�

    int                     m_MaxParticleCount = 1000;	// ��ƼŬ �ִ� ����
	//float                   m_SpawnRate = 10.f;			// �ʴ� �����Ǿ���ϴ� ��ƼŬ ����

	bool                    m_DashParticleInit = false; // �뽬 ��ƼŬ �ʱ�ȭ ����
	float					m_DashParticleLifeTime = 3.f;	// �뽬 ��ƼŬ ����


public:
    void SetParticleTexture(Ptr<CTexture> _Texture);
    void CaculateSpawnCount();

	void SetType(int _Type) { m_Module.Type = _Type; }
	void SetReferenceDir(Vec3 _Dir);

public:
	virtual void Init() override;
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // ���Ͽ� ������Ʈ�� ������ ����
    virtual void LoadFromFile(FILE* _File) override; // ���Ͽ� ������Ʈ�� ������ �ε�
};

