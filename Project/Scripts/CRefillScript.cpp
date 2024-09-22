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
}

void CRefillScript::Tick()
{
}

void CRefillScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 플레이어와 충돌 시
	if (_OtherObject->GetLayerIdx() == (int)LAYER::PLAYER)
	{
		// 플레이어의 스크립트를 가져옴
		CPlayerScript* pPlayerScript = (CPlayerScript*)_OtherObject->GetScript<CPlayerScript>();

		// 플레이어의 대쉬 횟수 증가
		pPlayerScript->AddDashCount();

		// 오브젝트를 삭제
		DeleteObject(GetOwner());
	}
}

void CRefillScript::SaveToFile(FILE* _File)
{
}

void CRefillScript::LoadFromFile(FILE* _File)
{
}
