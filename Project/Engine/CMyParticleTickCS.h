#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CMyParticleTickCS :
    public CComputeShader
{
public:
	CMyParticleTickCS();
	~CMyParticleTickCS();

private:
	CStructuredBuffer* m_ParticleBuffer = nullptr;
	CStructuredBuffer* m_SpawnCountBuffer = nullptr;
	CStructuredBuffer* m_ModuleBuffer = nullptr;

	Ptr<CTexture>       m_NoiseTex;
	Vec3                m_ParticleWorldPos;

public:
	void SetParticleBuffer(CStructuredBuffer* _Buffer) { m_ParticleBuffer = _Buffer; }
	void SetSpawnCountBuffer(CStructuredBuffer* _Buffer) { m_SpawnCountBuffer = _Buffer; }
	void SetModuleBuffer(CStructuredBuffer* _ModuleBuffer) { m_ModuleBuffer = _ModuleBuffer; }

	void SetParticleWorldPos(Vec3 _Pos) { m_ParticleWorldPos = _Pos; }
	void SetNoiseTexture(Ptr<CTexture> _Noise) { m_NoiseTex = _Noise; }

public:
	virtual int Binding() override;
	virtual void CalcGroupNum() override;
	virtual void Clear() override;
};
