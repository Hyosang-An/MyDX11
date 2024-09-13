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

    Vec3	vForce;				// Particle �� ������ ��
    Vec3	vVelocity;			// Particle �� �ӵ�

    float	NoiseForceAccTime;	// Noise Force �ҿ� �����ϴ� ���� �ð�
    Vec3	NoiseForceDir;		// Noise Force �� ����

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
    CStructuredBuffer*      m_ParticleBuffer = nullptr;		// ��� ��ƼŬ ����
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr;	// ��ƼŬ Ȱ��ȭ ���� ���޿� ����

	Ptr<CTexture>           m_ParticleTex;					// ��ƼŬ �ؽ���
    float                   m_Time = 0;                 // �����ð�
    float					m_prevSpawnCountFraction = 0.f;	// ���������ӿ��� 1�����Ӵ� �����Ǿ���ϴ� ��ƼŬ ������ �Ҽ��κ�

    int                     m_MaxParticleCount = 1000;	// ��ƼŬ �ִ� ����
	float                   m_SpawnRate = 10.f;			// �ʴ� �����Ǿ���ϴ� ��ƼŬ ����



public:
    void SetParticleTexture(Ptr<CTexture> _Texture);
    void CaculateSpawnCount();

public:

    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // ���Ͽ� ������Ʈ�� ������ ����
    virtual void LoadFromFile(FILE* _File) override; // ���Ͽ� ������Ʈ�� ������ �ε�
};

