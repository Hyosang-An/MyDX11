#include "pch.h"
#include "CPlayerScript.h"

#include "CMissileScript.h"

CPlayerScript::CPlayerScript()
	: m_Speed(400.f)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::LEFT))
		vPos.x -= DT * m_Speed;
	if (KEY_PRESSED(KEY::RIGHT))
		vPos.x += DT * m_Speed;
	if (KEY_PRESSED(KEY::UP))
		vPos.y += DT * m_Speed;
	if (KEY_PRESSED(KEY::DOWN))
		vPos.y -= DT * m_Speed;


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
		// 미사일 발사
		CGameObject* pMissileObject = new CGameObject;
		pMissileObject->AddComponent(new CTransform);
		pMissileObject->AddComponent(new CMeshRender);
		pMissileObject->AddComponent(new CCollider2D);
		pMissileObject->AddComponent(new CMissileScript);

		Vec3 vMissilePos = Transform()->GetRelativePos();
		vMissilePos.y += Transform()->GetRelativeScale().y / 2.f;

		pMissileObject->Transform()->SetRelativePos(vMissilePos);
		pMissileObject->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));

		pMissileObject->Collider2D()->SetScale(Vec3(1.f, 1.f, 1.f));

		CreateObject(pMissileObject, 5);
	}

	Transform()->SetRelativePos(vPos);
}

void CPlayerScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	DeleteObject(_OtherObject);

	Vec3 vScale = Transform()->GetRelativeScale();

	vScale += Vec3(10.f, 10.f, 0.f);
	Collider2D()->SetScale(Collider2D()->GetScale() + Vec3(10.f, 10.f, 0.f));

	Transform()->SetRelativeScale(vScale);
}