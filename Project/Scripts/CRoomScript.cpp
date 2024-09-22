#include "pch.h"
#include "CRoomScript.h"
#include "CPlayerScript.h"
#include "CRigidBody.h"
#include "CBadelineScript.h"
#include "CCameraMoveScript.h"

#include "Engine/CLevelMgr.h"
#include "Engine/CLevel.h"
#include "Engine/CLayer.h"

CRoomScript::CRoomScript() :
	CScript((UINT)SCRIPT_TYPE::ROOMSCRIPT)
{
	// 노출시킬 데이터
	AddScriptParam(SCRIPT_PARAM_TYPE::BOOL, "Start Room", &m_StartRoom);
	AddScriptParam(SCRIPT_PARAM_TYPE::BOOL, "Spawn Badeline", &m_BadelineSpawnRoom);
}

CRoomScript::CRoomScript(const CRoomScript& _Other) :
	CScript((UINT)SCRIPT_TYPE::ROOMSCRIPT)
{
}

CRoomScript::~CRoomScript()
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
			m_PlayerSpawnPos = Child->Transform()->GetWorldPos();

			// offset
			m_PlayerSpawnPos.y += 64;

			break;
		}
	}

	if (m_StartRoom)
	{
		FirstSpawnPlayer();
	}
}

void CRoomScript::FirstSpawnPlayer()
{
	auto pPlayer = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\Player.prefab")->Instantiate();
	pPlayer->FlipBookComponent()->Play(L"Idle");

	// 플레이어의 위치를 Room의 spawn로 이동
	pPlayer->Transform()->SetWorldPos(m_PlayerSpawnPos);

	// 메인 카메라의 위치를 플레이어 위치로 이동
	CGameObject* pMainCamera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	pMainCamera->Transform()->SetWorldPos(m_PlayerSpawnPos);

	// 레벨에 추가
	CLevelMgr::GetInst()->GetCurrentLevel()->AddObject(LAYER::PLAYER, pPlayer);

	// 플레이어의 Room을 이 Room으로 변경
	pPlayer->GetScript<CPlayerScript>()->ChangeRoom(GetOwner());

	// 메인 카메라의 위치를 ChangeRoomTarget으로 이동
	Vec3 changeRoomTargetPos = pMainCamera->GetScript<CCameraMoveScript>()->GetChangeRoomTargetPos();
	pMainCamera->Transform()->SetWorldPos(changeRoomTargetPos);
}

void CRoomScript::Tick()
{

	// Badeline 스폰시킬지 체크
	if (m_BadelineSpawnRoom && !m_BadenlineSpawned)
	{
		CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer((UINT)LAYER::PLAYER)->GetParentObjects().front();
		Vec3 PlayerPos = pPlayer->Transform()->GetWorldPos();

		// 플레이어와 SpawnPoint의 x좌표 차이가 10 이하일 때 Badeline 스폰
		if (abs(PlayerPos.x - m_PlayerSpawnPos.x) < 10)
		{
			m_BadenlineSpawned = true;

			// Badeline 스폰
			//auto pBadeline = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\Badeline.prefab")->Instantiate();

			CGameObject* pBadeline = new CGameObject;
			m_Badeline = pBadeline;
			pBadeline->SetName(L"Badeline");
			
			pBadeline->AddComponent(new CTransform);
			Vec3 badelineSpawnPos = m_PlayerSpawnPos + Vec3(-100, 200, 10); // 플레이어 스폰 위치에서 좌측으로 100, 위로 200 z축 방향으로 10만큼 오프셋
			//badelineSpawnPos.z = 10;
			pBadeline->Transform()->SetRelativePos(badelineSpawnPos);
			pBadeline->Transform()->SetRelativeScale(pPlayer->Transform()->GetRelativeScale());

			pBadeline->AddComponent(new CCollider2D);
			pBadeline->Collider2D()->SetIndependentScale(true);
			pBadeline->Collider2D()->SetOffset(Vec3(0, -64, 0));
			pBadeline->Collider2D()->SetScale(Vec3(48, 96, 1));


			pBadeline->AddComponent(new CMeshRender);
			pBadeline->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
			pBadeline->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl"));

			pBadeline->AddComponent(new CFlipBookComponent);

			pBadeline->AddComponent(new CBadelineScript);
			pBadeline->GetScript<CBadelineScript>()->SetPlayer(pPlayer);

			SpawnObject(pBadeline, (UINT)LAYER::MONSTER);
		}
	}

	// 플레이어가 다른 Room으로 넘어가면 Badeline 제거
	if (m_Badeline != nullptr)
	{
		CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer((UINT)LAYER::PLAYER)->GetParentObjects().front();
		if (pPlayer->GetScript<CPlayerScript>()->GetCurRoom() != GetOwner())
		{
			DeleteObject(m_Badeline);
			m_Badeline = nullptr;

			m_BadenlineSpawned = false;
		}
	}
}

void CRoomScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 플레이어와 충돌했을 때
	if (_OtherObject->GetLayerIdx() == (UINT)LAYER::PLAYER)
	{
		// 현재 레벨이 정지상태면 return
		if (CLevelMgr::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::STOP)
			return;

		CGameObject* pPlayer = _OtherObject;

		// 플레이어와 SpawnPointPos가 매우 가까우면 처음 스폰된 것이므로 return
		if ((pPlayer->Transform()->GetWorldPos() - m_PlayerSpawnPos).Length() < 10)
			return;

		// 기존의 플레이어가 속한 Room을 이쪽 Room으로 변경
		pPlayer->GetScript<CPlayerScript>()->ChangeRoom(GetOwner());

		// 플레이어가 Room의 Top, Bottom, Left, Right중 어느쪽으로 들어왔는지 체크
		Vec3 PlayerPos = pPlayer->Transform()->GetWorldPos();
		Vec3 RoomPos = GetOwner()->Collider2D()->GetWorldPos();
		Vec3 RoomScale = GetOwner()->Transform()->GetWorldScale();

		// 플레이어와 Top, Bottom, Left, Right 중 어느쪽이 가까운지 체크
		float TopDist = abs(PlayerPos.y - (RoomPos.y + RoomScale.y * 0.5f));
		float BottomDist = abs(PlayerPos.y - (RoomPos.y - RoomScale.y * 0.5f));
		float LeftDist = abs(PlayerPos.x - (RoomPos.x - RoomScale.x * 0.5f));
		float RightDist = abs(PlayerPos.x - (RoomPos.x + RoomScale.x * 0.5f));

		if (TopDist < BottomDist && TopDist < LeftDist && TopDist < RightDist)
		{
			// Top
			auto pos = pPlayer->Transform()->GetWorldPos();
			pos.y -= 48;
			pPlayer->Transform()->SetWorldPos(pos);
		}
		else if (BottomDist < TopDist && BottomDist < LeftDist && BottomDist < RightDist)
		{
			// Bottom
			auto pos = pPlayer->Transform()->GetWorldPos();
			//pos.y += 48;
			//pPlayer->Transform()->SetWorldPos(pos);

			// 플레이어를 점프시킴
			CRigidBody* pRigidbody = pPlayer->GetScript<CRigidBody>();
			Vec3 vVelocity = pRigidbody->GetVelocity();
			vVelocity.y = 1000;
			pRigidbody->SetVelocity(vVelocity);

			CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
			pPlayerScript->SetCurState(PLAYER_STATE::JUMP);
		}
		else if (LeftDist < TopDist && LeftDist < BottomDist && LeftDist < RightDist)
		{
			// Left
			auto pos = pPlayer->Transform()->GetWorldPos();
			pos.x += 48;
			pPlayer->Transform()->SetWorldPos(pos);
		}
		else
		{
			// Right
			auto pos = pPlayer->Transform()->GetWorldPos();
			pos.x -= 48;
			pPlayer->Transform()->SetWorldPos(pos);
		}
		
	}
	

	

}

void CRoomScript::SaveToFile(FILE* _File)
{
	fwrite(&m_StartRoom, sizeof(bool), 1, _File);
	fwrite(&m_BadelineSpawnRoom, sizeof(bool), 1, _File);
}

void CRoomScript::LoadFromFile(FILE* _File)
{
	fread(&m_StartRoom, sizeof(bool), 1, _File);
	fread(&m_BadelineSpawnRoom, sizeof(bool), 1, _File);
}

