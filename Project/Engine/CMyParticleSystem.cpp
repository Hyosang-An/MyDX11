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

	if (_particleType == 1)
		m_MaxParticleCount = 100;

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tMyParticle), m_MaxParticleCount, SB_TYPE::SRV_UAV);

	m_SpawnCountBuffer = new CStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tMySpawnCount), 1, SB_TYPE::SRV_UAV);

	// 모듈 정보 세팅
	m_Module.Type = _particleType; // 눈발 파티클
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::SPAWN] = true;
	m_Module.SpawnRate = 30;

	// Spawn Shape
	m_Module.SpawnShape = 0;




	// Render Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::RENDER] = true;
	m_Module.EndColor = Vec3(1.f, 1.f, 1.f);
	m_Module.FadeOut = true;
	m_Module.FadeOutStartRatio = 0.5f;
	m_Module.VelocityAlignment = true;

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tMyParticleModule), 1, SB_TYPE::SRV_ONLY, &m_Module); // SRV_Only도 가능한지 나중에 테스트해보기 -> 가능함
	
	//m_ModuleBuffer->SetData(&m_Module);
}

CMyParticleSystem::CMyParticleSystem(const CMyParticleSystem& _Other) :
	CRenderComponent(_Other)
{
	m_TickCS = (CMyParticleTickCS*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"MyParticleTickCS").Get();

	m_Module = _Other.m_Module;
	m_MaxParticleCount = _Other.m_MaxParticleCount;

	m_ParticleBuffer = nullptr;
	m_SpawnCountBuffer = nullptr;
	m_ParticleTex = _Other.m_ParticleTex;
	m_Time = 0;
	m_prevSpawnCountFraction = 0;

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

	if (_Other.m_ModuleBuffer)
	{
		m_ModuleBuffer = new CStructuredBuffer;
		m_ModuleBuffer->Create(sizeof(tMyParticleModule), 1, SB_TYPE::SRV_UAV, &m_Module);
	}
}

CMyParticleSystem::~CMyParticleSystem()
{
	DELETE_PTR(m_ParticleBuffer);
	DELETE_PTR(m_SpawnCountBuffer);
	DELETE_PTR(m_ModuleBuffer);
}

void CMyParticleSystem::Init()
{
	// Particle Scale 설정

	if (m_Module.Type == 0)
	{

	}
	else if (m_Module.Type == 1)
	{
		// Noise Force Module
		m_Module.isModuleOn[(UINT)PARTICLE_MODULE::NOISE_FORCE] = true;
		m_Module.NoiseForceTerm = 0.3f;		// Noise Force 적용시키는 텀
		m_Module.NoiseForceScale = 0.8f;		// Noise Force 크기 (현재 속도에 대한 비율)

		// Drag Module (감속)
		m_Module.isModuleOn[(UINT)PARTICLE_MODULE::DRAG] = true;
		m_Module.DestNormalizedAge = 1.f;
		m_Module.TargetSpeed = 20.f;
	}

	m_Module.SpawnShapeScale = Transform()->GetRelativeScale();
	m_ModuleBuffer->SetData(&m_Module);
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
	
	// SpawnCount 를 Buffer 에 전달	
	m_SpawnCountBuffer->SetData(&count);
}

void CMyParticleSystem::SetReferenceDir(Vec3 _Dir)
{
	m_Module.ReferenceDir = _Dir.Normalize();
	m_ModuleBuffer->SetData(&m_Module);
}



void CMyParticleSystem::FinalTick()
{
	if (m_Module.Type == 0) // 눈발 파티클
	{
		// SpawnCount 계산
		CaculateSpawnCount();

		// ComputeShader (TickCS는 ParticleSystem 객체마다 따로 존재하는것이 아니라 Asset으로 하나만 존재하기 때문에 해당CS를 쓸 때마다 데이터를 Set해주고 계산한뒤 Clear해야한다. (계산(Excute하면서 처리))
		m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
		m_TickCS->SetParticleBuffer(m_ParticleBuffer);
		m_TickCS->SetSpawnCountBuffer(m_SpawnCountBuffer);
		m_TickCS->SetModuleBuffer(m_ModuleBuffer);

		m_TickCS->Execute(); // Buffer 바인딩 후 ComputeShader 실행한 뒤 바인딩 값 Clear
	}

	else if (m_Module.Type == 1) // 대쉬 파티클
	{
		tMySpawnCount count = {  };

		if (!m_DashParticleInit)
		{
			m_DashParticleInit = true;
			count.SpawnCount = 3;
		}
		// SpawnCount 를 Buffer 에 전달	
		m_SpawnCountBuffer->SetData(&count);

		// ComputeShader (TickCS는 ParticleSystem 객체마다 따로 존재하는것이 아니라 Asset으로 하나만 존재하기 때문에 해당CS를 쓸 때마다 데이터를 Set해주고 계산한뒤 Clear해야한다. (계산(Excute하면서 처리))
		m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
		m_TickCS->SetParticleBuffer(m_ParticleBuffer);
		m_TickCS->SetSpawnCountBuffer(m_SpawnCountBuffer);
		m_TickCS->SetModuleBuffer(m_ModuleBuffer);

		m_TickCS->Execute(); // Buffer 바인딩 후 ComputeShader 실행한 뒤 바인딩 값 Clear


		// 누적시간이 m_DashParticleLifeTime을 지나면 파티클 삭제
		if (m_Time > m_DashParticleLifeTime)
		{
			DeleteObject(GetOwner());
		}
	}

	m_Time += EngineDT;

}

void CMyParticleSystem::Render()
{
	// 위치정보 바인딩
	Transform()->Binding();

	// 파티클 버퍼 바인딩
	m_ParticleBuffer->Binding(20);	// t20 (SRV)

	// 모듈 버퍼 바인딩
	m_ModuleBuffer->Binding(21);	// t21 (SRV)

	// 재질정보 바인딩
	GetMaterial()->Binding();

	// 렌더링
	GetMesh()->Render_Particle(m_MaxParticleCount);

	// 파티클 버퍼 바인딩 해제
	m_ParticleBuffer->Clear(20);	// t20
}

void CMyParticleSystem::SaveToFile(FILE* _File)
{
	fwrite(&m_Module, sizeof(tMyParticleModule), 1, _File);
	fwrite(&m_MaxParticleCount, sizeof(int), 1, _File);
}

void CMyParticleSystem::LoadFromFile(FILE* _File)
{
	fread(&m_Module, sizeof(tMyParticleModule), 1, _File);
	fread(&m_MaxParticleCount, sizeof(int), 1, _File);

	//m_ModuleBuffer->SetData(&m_Module);
}
