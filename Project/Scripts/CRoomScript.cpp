#include "pch.h"
#include "CRoomScript.h"
#include "CPlayerScript.h"


CRoomScript::CRoomScript() :
	CScript((UINT)SCRIPT_TYPE::ROOMSCRIPT)
{
}

CRoomScript::CRoomScript(const CRoomScript& _Other) :
	CScript((UINT)SCRIPT_TYPE::ROOMSCRIPT)
{
}

CRoomScript::~CRoomScript()
{
	if (m_PlayerSpawnPoint)
	{
		delete m_PlayerSpawnPoint;
		m_PlayerSpawnPoint = nullptr;
	}
}

void CRoomScript::FirstSpawnPlayer()
{
	auto pPlayer = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\Player.prefab")->Instantiate();
	pPlayer->FlipBookComponent()->Play(L"Idle");

	// 플레이어의 위치를 Room의 spawn로 이동
	pPlayer->Transform()->SetWorldPos(m_PlayerSpawnPoint->Transform()->GetWorldPos());
}

void CRoomScript::RespawnPlayer()
{
}

void CRoomScript::Begin()
{
	// Room의 자식 중 PlayerSpawnPoint를 찾아서 저장
	auto &children = GetOwner()->GetChildren();
	for (auto& Child : children)
	{
		if (Child->GetName() == L"PlayerSpawnPoint")
		{
			m_PlayerSpawnPoint = Child;
			break;
		}
	}
}

void CRoomScript::Tick()
{
}

void CRoomScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 플레이어와 충돌했을 때
	if (_OtherObject->GetLayerIdx() == (UINT)LAYER::PLAYER)
	{
		// 기존의 플레이어가 속한 Room을 이쪽 Room으로 변경
		_OtherObject->GetScript<CPlayerScript>()->ChangeRoom(GetOwner());
	}
}

void CRoomScript::SaveToFile(FILE* _File)
{
}

void CRoomScript::LoadFromFile(FILE* _File)
{
}

