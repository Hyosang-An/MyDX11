#include "pch.h"
#include "CParticleSystem.h"


#include "CDevice.h"
#include "CStructuredBuffer.h"

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)
	, m_ParticleBuffer(nullptr)
	, m_MaxParticeCount(100)
{
	tParticle Particle = {};
	Particle.Active = true;
	Particle.vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticeCount, SB_TYPE::SRV_UAV, false, &Particle);

	tParticle Particle2 = {};
	m_ParticleBuffer->GetData(&Particle2);
}

CParticleSystem::~CParticleSystem()
{
	if (nullptr != m_ParticleBuffer)
		delete m_ParticleBuffer;
}

void CParticleSystem::FinalTick()
{

}

void CParticleSystem::Render()
{

}

void CParticleSystem::SaveToFile(FILE* _File)
{

}

void CParticleSystem::LoadFromFile(FILE* _File)
{

}