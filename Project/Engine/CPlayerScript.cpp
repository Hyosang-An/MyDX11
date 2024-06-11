#include "pch.h"
#include "CPlayerScript.h"

CPlayerScript::CPlayerScript()
	: m_Speed(200.f)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::LEFT))
	{
		vPos.x -= DT * m_Speed;
	}

	if (KEY_PRESSED(KEY::RIGHT))
	{
		vPos.x += DT * m_Speed;
	}

	if (KEY_PRESSED(KEY::Z))
	{
		//MeshRender()->GetMaterial()->SetScalarParam(INT_0, 1);
		Vec3 vRot = Transform()->GetRelativeRoatation();
		vRot.z += DT * XM_PI;
		Transform()->SetRelativeRotation(vRot);

		/*	Vec3 vScale = Transform()->GetRelativeScale();
			vScale += DT * 100.f * Vec3(1.f, 1.f, 1.f);
			Transform()->SetRelativeScale(vScale);*/
	}
	else
	{
		//MeshRender()->GetMaterial()->SetScalarParam(INT_0, 0);
	}

	if (KEY_JUST_PRESSED(KEY::SPACE))
	{
		// DrawDebugRect(Transform()->GetWorldMat(), Vec4(1.f, 0.4f, 0.6f, 1.f), 3.f, true);		
		// DrawDebugCircle(Transform()->GetRelativePos(), 100.f, Vec4(0.4f, 1.f, 0.6f, 1.f), 5.f, false);
	}

	Transform()->SetRelativePos(vPos);
}
