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
	// 남은 시간 비율에 따라 투명도를 조절한다.
	float alpha = 1.f - m_accTime / m_lifeTime;
	GetOwner()->MeshRender()->GetDynamicMaterial()->SetScalarParam(FLOAT_0, alpha);

	// 일정 시간이 지나면 사라지게 한다.
	m_accTime += DT;

	if (m_accTime >= m_lifeTime)
	{
		DeleteObject(GetOwner());
	}

}
