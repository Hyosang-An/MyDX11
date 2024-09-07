#include "pch.h"
#include "CRoomScript.h"
#include "CPlayerScript.h"

#include "Engine/CLevelMgr.h"
#include "Engine/CLevel.h"

CRoomScript::CRoomScript() :
	CScript((UINT)SCRIPT_TYPE::ROOMSCRIPT)
{
	// �����ų ������
	AddScriptParam(SCRIPT_PARAM_TYPE::BOOL, "Start Room", &m_StartRoom);
}

CRoomScript::CRoomScript(const CRoomScript& _Other) :
	CScript((UINT)SCRIPT_TYPE::ROOMSCRIPT)
{
}

CRoomScript::~CRoomScript()
{

}

void CRoomScript::FirstSpawnPlayer()
{
	auto pPlayer = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\Player.prefab")->Instantiate();
	pPlayer->FlipBookComponent()->Play(L"Idle");

	// �÷��̾��� ��ġ�� Room�� spawn�� �̵�
	pPlayer->Transform()->SetWorldPos(m_PlayerSpawnPos);

	// ������ �߰�
	CLevelMgr::GetInst()->GetCurrentLevel()->AddObject(LAYER::PLAYER, pPlayer);

	// �÷��̾��� Room�� �� Room���� ����
	pPlayer->GetScript<CPlayerScript>()->ChangeRoom(GetOwner());
}

void CRoomScript::Begin()
{
	// Room�� �ڽ� �� PlayerSpawnPoint�� ã�Ƽ� ����
	auto &children = GetOwner()->GetChildren();
	for (auto& Child : children)
	{
		if (Child->GetName() == L"PlayerSpawnPoint")
		{
			m_PlayerSpawnPos = Child->Transform()->GetWorldPos();

			// offset
			m_PlayerSpawnPos.y += 64;
		}
	}

	if (m_StartRoom)
	{
		FirstSpawnPlayer();
	}
}

void CRoomScript::Tick()
{
}

void CRoomScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// �÷��̾�� �浹���� ��
	if (_OtherObject->GetLayerIdx() == (UINT)LAYER::PLAYER)
	{
		// ������ �÷��̾ ���� Room�� ���� Room���� ����
		_OtherObject->GetScript<CPlayerScript>()->ChangeRoom(GetOwner());
	}
}

void CRoomScript::SaveToFile(FILE* _File)
{
	fwrite(&m_StartRoom, sizeof(bool), 1, _File);
}

void CRoomScript::LoadFromFile(FILE* _File)
{
	fread(&m_StartRoom, sizeof(bool), 1, _File);
}

