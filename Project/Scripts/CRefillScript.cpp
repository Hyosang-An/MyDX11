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
	// �÷��̾�� �浹 ��
	if (_OtherObject->GetLayerIdx() == (int)LAYER::PLAYER)
	{
		// �÷��̾��� ��ũ��Ʈ�� ������
		CPlayerScript* pPlayerScript = (CPlayerScript*)_OtherObject->GetScript<CPlayerScript>();

		// �÷��̾��� �뽬 Ƚ�� ����
		pPlayerScript->AddDashCount();

		// ������Ʈ�� ����
		DeleteObject(GetOwner());
	}
}

void CRefillScript::SaveToFile(FILE* _File)
{
}

void CRefillScript::LoadFromFile(FILE* _File)
{
}
