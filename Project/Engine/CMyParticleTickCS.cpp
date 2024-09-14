#include "pch.h"
#include "CMyParticleTickCS.h"

#include "CAssetMgr.h"
#include "CStructuredBuffer.h"
#include "CDevice.h"
#include "CConstBuffer.h"

CMyParticleTickCS::CMyParticleTickCS() :
	CComputeShader(1024, 1, 1, L"shader\\my_particletick.fx", "CS_MyParticleTick")
{
}

CMyParticleTickCS::~CMyParticleTickCS()
{
}

int CMyParticleTickCS::Binding()
{
	if (nullptr == m_ParticleBuffer || nullptr == m_SpawnCountBuffer || nullptr == m_NoiseTex)
		return E_FAIL;

	m_ParticleBuffer->Binding_CS_UAV(0);
	m_SpawnCountBuffer->Binding_CS_UAV(1);
	m_NoiseTex->Binding_CS_SRV(20);

	m_Const.iArr[0] = m_ParticleBuffer->GetElementCount(); // Particle Max Count
	m_Const.v4Arr[0] = m_ParticleWorldPos;

	// 상수데이터 전달
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding_CS();

	return S_OK;
}

void CMyParticleTickCS::CalcGroupNum()
{
	m_GroupX = m_ParticleBuffer->GetElementCount() / m_ThreadPerGroupX;

	if (m_ParticleBuffer->GetElementCount() % m_ThreadPerGroupX)
		m_GroupX += 1;

	m_GroupY = 1;
	m_GroupZ = 1;
}

void CMyParticleTickCS::Clear()
{
	m_ParticleBuffer->Clear_CS_UAV();
	m_ParticleBuffer = nullptr;

	m_NoiseTex->Clear_CS_SRV();
}
