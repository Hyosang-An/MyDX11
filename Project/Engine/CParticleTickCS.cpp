#include "pch.h"
#include "CParticleTickCS.h"

#include "CStructuredBuffer.h"

CParticleTickCS::CParticleTickCS()
	: CComputeShader(1024, 1, 1, L"shader\\particletick.fx", "CS_ParticleTick")
{

}

CParticleTickCS::~CParticleTickCS()
{
}


int CParticleTickCS::Binding()
{
	if (nullptr == m_ParticleBuffer || nullptr == m_SpawnCountBuffer)
		return E_FAIL;

	m_ParticleBuffer->Binding_CS_UAV(0);
	m_SpawnCountBuffer->Binding_CS_UAV(1);
	m_Const.iArr[0] = m_ParticleBuffer->GetElementCount();

	return S_OK;
}

void CParticleTickCS::CalcGroupNum()
{
	m_GroupX = m_ParticleBuffer->GetElementCount() / m_ThreadPerGroupX;

	if (m_ParticleBuffer->GetElementCount() % m_ThreadPerGroupX)
		m_GroupX += 1;

	m_GroupY = 1;
	m_GroupZ = 1;
}

void CParticleTickCS::Clear()
{
	m_ParticleBuffer->Clear_CS_UAV();
	m_ParticleBuffer = nullptr;
}
