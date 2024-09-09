#include "pch.h"
#include "CDashTrailScript.h"

CDashTrailScript::CDashTrailScript() :
	CScript((UINT)SCRIPT_TYPE::DASHTRAILSCRIPT)
{
}

CDashTrailScript::~CDashTrailScript()
{
}

void CDashTrailScript::Tick()
{
	// ���� �ð� ������ ���� ������ �����Ѵ�.
	float alpha = 1.f - m_accTime / m_lifeTime;
	GetOwner()->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, alpha);

	// ���� �ð��� ������ ������� �Ѵ�.
	m_accTime += DT;

	if (m_accTime >= m_lifeTime)
	{
		DeleteObject(GetOwner());
	}

}
