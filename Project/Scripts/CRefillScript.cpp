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
	// �÷��̾�� �浹 ��
	if (_OtherObject->GetLayerIdx() == (int)LAYER::PLAYER)
	{
		// ��Ȱ��ȭ ������ ���� �ƹ��͵� ���� ����
		if (m_Active == false)
			return;

		// �÷��̾��� ��ũ��Ʈ�� ������
		CPlayerScript* pPlayerScript = (CPlayerScript*)_OtherObject->GetScript<CPlayerScript>();

		// �÷��̾��� �뽬 Ƚ�� ����
		pPlayerScript->AddDashCount();

		// ������Ʈ�� ��Ȱ��ȭ
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
