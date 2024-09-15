#include "pch.h"
#include "CMyParticleSystem.h"

#include "CTimeMgr.h"

#include "CDevice.h"
#include "CAssetMgr.h"
#include "CStructuredBuffer.h"


#include "CTransform.h"

CMyParticleSystem::CMyParticleSystem(UINT _particleType) :
	CRenderComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	// Mesh / Material 
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"MyParticleRenderMtrl"));

	// ParticleTick ComputeShader
	m_TickCS = (CMyParticleTickCS*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"MyParticleTickCS").Get();

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tMyParticle), m_MaxParticleCount, SB_TYPE::SRV_UAV);

	m_SpawnCountBuffer = new CStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tMySpawnCount), 1, SB_TYPE::SRV_UAV);

	// ��� ���� ����
	m_Module.Type = _particleType; // ���� ��ƼŬ
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::SPAWN] = true;
	m_Module.SpawnRate = 20;

	// Render Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::RENDER] = true;
	m_Module.EndColor = Vec3(1.f, 1.f, 1.f);
	m_Module.FadeOut = true;
	m_Module.FadeOutStartRatio = 0.9f;
	m_Module.VelocityAlignment = true;

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tMyParticleModule), 1, SB_TYPE::SRV_UAV, &m_Module);
	
	//m_ModuleBuffer->SetData(&m_Module);
}

CMyParticleSystem::CMyParticleSystem(const CMyParticleSystem& _Other) :
	CRenderComponent(_Other)
{
	m_TickCS = _Other.m_TickCS;
	m_ParticleBuffer = nullptr;
	m_SpawnCountBuffer = nullptr;
	m_ParticleTex = _Other.m_ParticleTex;
	m_Time = 0;
	m_prevSpawnCountFraction = 0;
	m_MaxParticleCount = _Other.m_MaxParticleCount;

	if (_Other.m_ParticleBuffer)
	{
		m_ParticleBuffer = new CStructuredBuffer;
		m_ParticleBuffer->Create(sizeof(tMyParticle), m_MaxParticleCount, SB_TYPE::SRV_UAV);
	}

	if (_Other.m_SpawnCountBuffer)
	{
		m_SpawnCountBuffer = new CStructuredBuffer;
		m_SpawnCountBuffer->Create(sizeof(tMySpawnCount), 1, SB_TYPE::SRV_UAV);
	}
}

CMyParticleSystem::~CMyParticleSystem()
{
	DELETE_PTR(m_ParticleBuffer);
	DELETE_PTR(m_SpawnCountBuffer);
	DELETE_PTR(m_ModuleBuffer);
}

void CMyParticleSystem::SetParticleTexture(Ptr<CTexture> _Texture)
{
	m_ParticleTex = _Texture;
	GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);
}

void CMyParticleSystem::CaculateSpawnCount()
{
	m_Time += EngineDT;
	tMySpawnCount count = {  };

	float SpawnCount = m_Module.SpawnRate * EngineDT + m_prevSpawnCountFraction;
	count.SpawnCount = floor(SpawnCount);
	m_prevSpawnCountFraction = SpawnCount - count.SpawnCount;
	
	// SpawnCount �� Buffer �� ����	
	m_SpawnCountBuffer->SetData(&count);
}

void CMyParticleSystem::FinalTick()
{
	if (m_Module.Type == 0) // ���� ��ƼŬ
	{
		// SpawnCount ���
		CaculateSpawnCount();

		// ComputeShader (TickCS�� ParticleSystem ��ü���� ���� �����ϴ°��� �ƴ϶� Asset���� �ϳ��� �����ϱ� ������ �ش�CS�� �� ������ �����͸� Set���ְ� ����ѵ� Clear�ؾ��Ѵ�. (���(Excute�ϸ鼭 ó��))
		m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
		m_TickCS->SetParticleBuffer(m_ParticleBuffer);
		m_TickCS->SetSpawnCountBuffer(m_SpawnCountBuffer);
		m_TickCS->SetModuleBuffer(m_ModuleBuffer);

		m_TickCS->Execute(); // Buffer ���ε� �� ComputeShader ������ �� ���ε� �� Clear
	}

	else if (m_Module.Type == 1) // �뽬 ��ƼŬ
	{
		tMySpawnCount count = {  };

		if (!m_DashParticleInit)
		{
			m_DashParticleInit = true;
			count.SpawnCount = 3;
		}
		// SpawnCount �� Buffer �� ����	
		m_SpawnCountBuffer->SetData(&count);

		// ComputeShader (TickCS�� ParticleSystem ��ü���� ���� �����ϴ°��� �ƴ϶� Asset���� �ϳ��� �����ϱ� ������ �ش�CS�� �� ������ �����͸� Set���ְ� ����ѵ� Clear�ؾ��Ѵ�. (���(Excute�ϸ鼭 ó��))
		m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
		m_TickCS->SetParticleBuffer(m_ParticleBuffer);
		m_TickCS->SetSpawnCountBuffer(m_SpawnCountBuffer);
		m_TickCS->SetModuleBuffer(m_ModuleBuffer);

		m_TickCS->Execute(); // Buffer ���ε� �� ComputeShader ������ �� ���ε� �� Clear


		// �����ð� 3�ʰ� ������ ��ƼŬ ����
		if (m_Time > 3.f)
		{
			//DeleteObject(GetOwner());
		}
	}

	m_Time += EngineDT;

}

void CMyParticleSystem::Render()
{
	// ��ġ���� ���ε�
	Transform()->Binding();

	// ��ƼŬ ���� ���ε�
	m_ParticleBuffer->Binding(20);	// t20 (SRV)

	// ��� ���� ���ε�
	m_ModuleBuffer->Binding(21);	// t21 (SRV)

	// �������� ���ε�
	GetMaterial()->Binding();

	// ������
	GetMesh()->Render_Particle(m_MaxParticleCount);

	// ��ƼŬ ���� ���ε� ����
	m_ParticleBuffer->Clear(20);	// t20
}

void CMyParticleSystem::SaveToFile(FILE* _File)
{
}

void CMyParticleSystem::LoadFromFile(FILE* _File)
{
}
