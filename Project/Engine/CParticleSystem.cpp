#include "pch.h"
#include "CParticleSystem.h"

#include "CTimeMgr.h"

#include "CDevice.h"
#include "CAssetMgr.h"
#include "CStructuredBuffer.h"

#include "CTransform.h"

//#include <algorithm> // std::shuffle
#include <random>    // std::default_random_engine
#include <chrono>    // std::chrono::system_clock

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	// Mesh / Material 
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ParticleRenderMtrl"));

	// ParticleTick ComputeShader
	m_TickCS = (CParticleTickCS*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"ParticleTickCS").Get();


	// Test�� Texture
	m_ParticleTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\particle\\TX_GlowScene_2.png");
	GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);

	// ��ƼŬ 100�� �ʱ� ����
	vector<tParticle> vecParticle = vector<tParticle>(m_MaxParticleCount);

	float Angle = XM_2PI / m_MaxParticleCount;

	for (int i = 0; i < m_MaxParticleCount; ++i)
	{
		vecParticle[i].Active = false;
		vecParticle[i].vLocalPos = Vec3(0.f, 0.f, 0.f);
		vecParticle[i].vWorldPos = Vec3(0.f, 0.f, 0.f);
		vecParticle[i].vWorldScale = Vec3(20.f, 20.f, 0.f);
		vecParticle[i].vColor = Vec4(0.9f, 0.34f, 0.5f, 1.f);
		vecParticle[i].Life = 2;
		vecParticle[i].vVelocity = Vec3(cosf(Angle * (float)i), sinf(Angle * (float)i), 0.f) * 10.f;
	}

	// ���� �ð����� �õ� ����
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	// ���� ���� ����
	std::default_random_engine engine(seed);

	// ���� ����
	std::shuffle(vecParticle.begin(), vecParticle.end(), engine);

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SB_TYPE::SRV_UAV, true, vecParticle.data());

	m_SpawnCountBuffer = new CStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::SRV_UAV, true, nullptr);
}

CParticleSystem::~CParticleSystem()
{
	DELETE(m_ParticleBuffer);
	DELETE(m_SpawnCountBuffer);
}

void CParticleSystem::FinalTick()
{
	// SpawnCount
	m_Time += EngineDT;

	tSpawnCount count = {};

	if (0.02f <= m_Time)
	{
		count.iSpawnCount = 2;
		m_Time = 0.f;
	}
	m_SpawnCountBuffer->SetData(&count);

	m_TickCS->SetParticleBuffer(m_ParticleBuffer);
	m_TickCS->SetSpawnCount(m_SpawnCountBuffer);
	m_TickCS->Execute();
}

void CParticleSystem::Render()
{
	// ��ġ���� ���ε�
	Transform()->Binding();

	// ��ƼŬ ���� ���ε�
	m_ParticleBuffer->Binding(20);	// t20

	// �������� ���ε�
	GetMaterial()->Binding();

	// ������
	GetMesh()->Render_Particle(m_MaxParticleCount);

	// ��ƼŬ ���� ���ε� ����
	m_ParticleBuffer->Clear(20);	// t20
}

void CParticleSystem::SaveToFile(FILE* _File)
{

}

void CParticleSystem::LoadFromFile(FILE* _File)
{

}