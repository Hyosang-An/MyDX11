#include "pch.h"
#include "CDashShockWaveEffectScript.h"

CDashShockWaveEffectScript::CDashShockWaveEffectScript() :
	CScript((UINT)SCRIPT_TYPE::DASHSHOCKWAVEEFFECTSCRIPT)
{
}

CDashShockWaveEffectScript::CDashShockWaveEffectScript(const CDashShockWaveEffectScript& _Other) :
	CScript((UINT)SCRIPT_TYPE::DASHSHOCKWAVEEFFECTSCRIPT)
{
}

CDashShockWaveEffectScript::~CDashShockWaveEffectScript()
{
}

void CDashShockWaveEffectScript::Begin()
{
	// Dynamic Material
	auto dynamicMtrl = GetRenderComponent()->GetDynamicMaterial();

	// mtrl�� �Ķ���ͷ� ���� ������Ʈ�� ������ �ð��� �Ѱ��ش�.
	dynamicMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_Time);
}

void CDashShockWaveEffectScript::Tick()
{
	m_Time += DT;

	auto dynamicMtrl = GetRenderComponent()->GetDynamicMaterial();
	dynamicMtrl->SetScalarParam(SCALAR_PARAM::FLOAT_0, m_Time);

	if (m_Time >= m_LifeTime)
	{
		DeleteObject(GetOwner());
	}
}

void CDashShockWaveEffectScript::SaveToFile(FILE* _File)
{
}

void CDashShockWaveEffectScript::LoadFromFile(FILE* _File)
{
}
