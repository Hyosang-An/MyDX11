#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CParticleTickCS :
    public CComputeShader
{
private:
    CStructuredBuffer* m_ParticleBuffer = nullptr;
    CStructuredBuffer* m_SpawnCountBuffer = nullptr;
    Ptr<CTexture>       m_NoiseTex;
    Vec3                m_ParticleWorldPos;

public:
    void SetParticleBuffer(CStructuredBuffer* _Buffer) { m_ParticleBuffer = _Buffer; }
    void SetSpawnCount(CStructuredBuffer* _Buffer) { m_SpawnCountBuffer = _Buffer; }
    void SetParticleWorldPos(Vec3 _Pos) { m_ParticleWorldPos = _Pos; }
    void SetNoiseTexture(Ptr<CTexture> _Noise) { m_NoiseTex = _Noise; }

public:
    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;

public:
    CParticleTickCS();
    ~CParticleTickCS();
};

