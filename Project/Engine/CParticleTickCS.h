#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CParticleTickCS :
    public CComputeShader
{
private:
    CStructuredBuffer* m_ParticleBuffer = nullptr;
    CStructuredBuffer* m_SpawnCountBuffer;

public:
    void SetParticleBuffer(CStructuredBuffer* _Buffer) { m_ParticleBuffer = _Buffer; }
    void SetSpawnCount(CStructuredBuffer* _Buffer) { m_SpawnCountBuffer = _Buffer; }

public:
    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;

public:
    CParticleTickCS();
    ~CParticleTickCS();
};

