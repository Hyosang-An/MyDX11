#include "pch.h"
#include "CMissileScript.h"

CMissileScript::CMissileScript()
	: m_Speed(1000.f)
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	vPos.y += DT * m_Speed;

	Transform()->SetRelativePos(vPos);
}