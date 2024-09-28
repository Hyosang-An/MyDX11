#include "pch.h"
#include "CRefillScript.h"
#include "CPlayerScript.h"

CRefillScript::CRefillScript() :
	CScript((UINT)SCRIPT_TYPE::REFILLSCRIPT)
{
}

CRefillScript::CRefillScript(const CRefillScript& _Other) :
	CScript((UINT)SCRIPT_TYPE::REFILLSCRIPT)
{
}

CRefillScript::~CRefillScript()
{
}

void CRefillScript::Begin()
{
	MeshRender()->GetDynamicMaterial();
}

void CRefillScript::Tick()
{
	if (m_Active == false)
	{
		m_accTimeToReActive += DT;

		if (m_accTimeToReActive >= m_ReActiveTime)
		{
			m_Active = true;
			m_accTimeToReActive = 0.f;

			MeshRender()->GetDynamicMaterial()->SetScalarParam(SCALAR_PARAM::INT_3, 0);
		}
	}
}

void CRefillScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 플레이어와 충돌 시
	if (_OtherObject->GetLayerIdx() == (int)LAYER::PLAYER)
	{
		// 비활성화 상태일 때는 아무것도 하지 않음
		if (m_Active == false)
			return;

		// 플레이어의 스크립트를 가져옴
		CPlayerScript* pPlayerScript = (CPlayerScript*)_OtherObject->GetScript<CPlayerScript>();

		// 플레이어의 대쉬 횟수 증가
		pPlayerScript->AddDashCount();

		// 오브젝트를 비활성화
		//DeleteObject(GetOwner());
		m_Active = false;
		MeshRender()->GetDynamicMaterial()->SetScalarParam(SCALAR_PARAM::INT_3, -1);
	}
}

void CRefillScript::SaveToFile(FILE* _File)
{
}

void CRefillScript::LoadFromFile(FILE* _File)
{
}
