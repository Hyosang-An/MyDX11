#include "pch.h"
#include "CCameraMoveScript.h"

#include "CPlayerScript.h"

#include "Engine/CLevelMgr.h"
#include "Engine/CLayer.h"
#include "Engine/CLevel.h"



CCameraMoveScript::CCameraMoveScript() :
	CScript((UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT),
	m_CamTrackingSpeed(500.f)
{
	// 노출시킬 데이터
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Tracking Speed", &m_CamTrackingSpeed);
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "Room Change Speed", &m_CamRoomChangeSpeed);
}

CCameraMoveScript::~CCameraMoveScript()
{
}


void CCameraMoveScript::Begin()
{

}

void CCameraMoveScript::Tick()
{
}

void CCameraMoveScript::FinalTick()
{
	if (m_Player !=nullptr && m_Player->IsDead())
	{
		m_Player = nullptr;
		return;
	}

	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
	{
		OrthoGraphicMove();
	}
}

void CCameraMoveScript::OrthoGraphicMove()
{
	// 카메라가 플레이어를 가운데로 초점을 맞추도록해야 함.
	// 플레이어가 화면 중심을 벗어나면 카메라가 플레이어를 따라가도록 해야 함.

	// 플레이어 오브젝트를 찾아서 저장
	if (nullptr == m_Player)
	{
		m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
		return;
	}
	// 플레이어가 현재 속한 Room을 가져온다
	m_Room = m_Player->GetScript<CPlayerScript>()->GetRoom();
	if (nullptr == m_Room)
	{		
		return;
	}

	if (m_Room->Collider2D() == nullptr)
	{
		MessageBox(NULL, L"Room의 Collider2D가 없습니다.", L"경고", MB_OK);
		return;
	}

	if (m_bChangeRoom)
	{
		ChangeRoom();
	}
	else
	{
		TrackPlayer();
	}


	// 백그라운드 위치 업데이트
	Vec3 camPos = Transform()->GetWorldPos();
	CLayer* layer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer((int)LAYER::BACKGROUND);
	auto& bgs = layer->GetParentObjects();
	for (auto bg : bgs)
	{
		bg->Transform()->SetWorldPos(Vec3(camPos.x, camPos.y, bg->Transform()->GetWorldPos().z));
	}


}

void CCameraMoveScript::PerspectiveMove()
{
	float Speed = m_CamTrackingSpeed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		Speed *= 3.f;
	}

	Vec3 vFront = Transform()->GetWorldDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetWorldDir(DIR::RIGHT);

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos += vFront * DT * Speed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos -= vFront * DT * Speed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos -= vRight * DT * Speed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos += vRight * DT * Speed;
	}

	Transform()->SetRelativePos(vPos);

	//if (KEY_PRESSED(KEY::RBTN))
	//{
	//	CKeyMgr::GetInst()->MouseCapture(true);

	//	// 마우스가 이동하는 방향
	//	//vMouseDragDelta.x; ==> y축 회전;
	//	//vMouseDragDelta.y; ==> x축 회전
	//	Vec2 vMouseDragDelta = CKeyMgr::GetInst()->GetMouseDragDelta();

	//	Vec3 vRot = Transform()->GetRelativeRoatation();
	//	vRot.y += vMouseDragDelta.x * XM_PI * DT * 10.f;
	//	vRot.x += vMouseDragDelta.y * XM_PI * DT * 10.f;
	//	Transform()->SetRelativeRotation(vRot);
	//}
	//else if (KEY_RELEASED(KEY::RBTN))
	//{
	//	CKeyMgr::GetInst()->MouseCapture(false);
	//}




	if (KEY_JUST_PRESSED(KEY::RBTN))
	{
		CKeyMgr::GetInst()->MouseCapture(true);
	}

	else if (KEY_RELEASED(KEY::RBTN))
	{
		CKeyMgr::GetInst()->MouseCapture(false);
	}

	Vec2 vMouseDragDelta = CKeyMgr::GetInst()->GetMouseDragDelta();

	Vec3 vRot = Transform()->GetRelativeRoatation();
	vRot.y += vMouseDragDelta.x * XM_PI * 0.001f;
	vRot.x += vMouseDragDelta.y * XM_PI * 0.001f;
	Transform()->SetRelativeRotation(vRot);
	

}

void CCameraMoveScript::TrackPlayer()
{
	// Room의 Row, Col을 가져온다
	Vec2 vRoomRowCol = m_Room->TileMap()->GetRowCol();

	// Room의 Col이 40보다 작거나 Row가 23보다 작으면 경고 MessageBox를 띄운다
	if (vRoomRowCol.x < 23 || vRoomRowCol.y < 40)
	{
		if (vRoomRowCol.x < 23)
		{
			wstring str = L"Room의 Row가 23보다 작습니다.";
			int row = (int)vRoomRowCol.x;
			str += L"\n현재 Row: " + to_wstring(row);
			MessageBox(NULL, str.c_str(), L"경고", MB_OK);
		}

		if (vRoomRowCol.y < 40)
		{
			wstring str = L"Room의 Col이 40보다 작습니다.";
			int col = (int)vRoomRowCol.y;
			str += L"\n현재 Col: " + to_wstring(col);
			MessageBox(NULL, str.c_str(), L"경고", MB_OK);
		}

		return;
	}


	Vec3 vPlayerWorldPos = m_Player->Transform()->GetWorldPos();
	Vec3 vCameraWorldPos = Transform()->GetWorldPos();

	Vec3 vDiff = vPlayerWorldPos - vCameraWorldPos;

	// 카메라 이동 속도는 차이의 크기에 비례하도록 한다.
	float CameraSpeed = vDiff.Length() * 2.f;

	vCameraWorldPos += vDiff.Normalize() * CameraSpeed * DT;
	Transform()->SetWorldPos(vCameraWorldPos);


	// 카메라가 보여주는 영역이 현재 Room을 넘어가지 않도록 한다
	Vec3 vCameraPos = Transform()->GetWorldPos();
	float fCameraLeftBorder = vCameraPos.x - Camera()->GetProjectionWidth() * 0.5f;
	float fCameraRightBorder = vCameraPos.x + Camera()->GetProjectionWidth() * 0.5f;
	float fCameraTopBorder = vCameraPos.y + Camera()->GetProjectionHeight() * 0.5f;
	float fCameraBottomBorder = vCameraPos.y - Camera()->GetProjectionHeight() * 0.5f;

	Vec3 vRoomPos = m_Room->Collider2D()->GetWorldPos();
	float fRoomLeftBorder = vRoomPos.x - m_Room->Transform()->GetWorldScale().x * 0.5f;
	float fRoomRightBorder = vRoomPos.x + m_Room->Transform()->GetWorldScale().x * 0.5f;
	float fRoomTopBorder = vRoomPos.y + m_Room->Transform()->GetWorldScale().y * 0.5f;
	float fRoomBottomBorder = vRoomPos.y - m_Room->Transform()->GetWorldScale().y * 0.5f;


	// 카메라가 Room의 왼쪽 경계를 넘어가지 않도록 한다
	if (fCameraLeftBorder < fRoomLeftBorder)
	{
		vCameraPos.x += fRoomLeftBorder - fCameraLeftBorder;
		Transform()->SetWorldPos(vCameraPos);
	}

	// 카메라가 Room의 오른쪽 경계를 넘어가지 않도록 한다
	if (fCameraRightBorder > fRoomRightBorder)
	{
		vCameraPos.x -= fCameraRightBorder - fRoomRightBorder;
		Transform()->SetWorldPos(vCameraPos);
	}

	// 카메라가 Room의 위쪽 경계를 넘어가지 않도록 한다
	if (fCameraTopBorder > fRoomTopBorder)
	{
		vCameraPos.y -= fCameraTopBorder - fRoomTopBorder;
		Transform()->SetWorldPos(vCameraPos);
	}

	// 카메라가 Room의 아래쪽 경계를 넘어가지 않도록 한다
	if (fCameraBottomBorder < fRoomBottomBorder)
	{
		vCameraPos.y += fRoomBottomBorder - fCameraBottomBorder;
		Transform()->SetWorldPos(vCameraPos);
	}
}

void CCameraMoveScript::ChangeRoom()
{
	m_Player;
	m_Room;

	// 카메라가 m_ChangeRoomTargetPos로 이동한다
	Vec3 vCameraPos = Transform()->GetWorldPos();
	Vec3 vDiff = m_ChangeRoomTargetPos - vCameraPos;

	// 카메라 이동 속도는 m_CamRoomChangeSpeed.
	Transform()->SetWorldPos(vCameraPos + vDiff.Normalize() * m_CamRoomChangeSpeed * EngineDT);

	// 카메라가 m_ChangeRoomTargetPos에 도착하면 m_bChangeRoom을 false로 바꾼다
	if ((m_ChangeRoomTargetPos - vCameraPos).Length() < 1.f)
	{
		m_bChangeRoom = false;
		ChangeLevelState(LEVEL_STATE::PLAY);
	}
}

void CCameraMoveScript::SetChangeRoom()
{
	m_bChangeRoom = true;
	ChangeLevelState(LEVEL_STATE::PAUSE);

	// m_ChangeRoomTargetPos를 설정한다

	// 플레이어 오브젝트를 찾아서 저장
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	if (nullptr == m_Player)
	{
		MessageBox(NULL, L"플레이어 오브젝트를 찾을 수 없습니다.", L"경고", MB_OK);
		return;
	}

	// 플레이어가 현재 속한 Room을 가져온다
	m_Room = m_Player->GetScript<CPlayerScript>()->GetRoom();
	if (nullptr == m_Room)
	{
		MessageBox(NULL, L"플레이어가 속한 Room을 찾을 수 없습니다.", L"경고", MB_OK);
		return;
	}

	// 카메라의 leftBorder, rightBorder, topBorder, bottomBorder를 가져온다
	Vec3 vCameraPos = Transform()->GetWorldPos();
	float fCameraLeftBorder = vCameraPos.x - Camera()->GetProjectionWidth() * 0.5f;
	float fCameraRightBorder = vCameraPos.x + Camera()->GetProjectionWidth() * 0.5f;
	float fCameraTopBorder = vCameraPos.y + Camera()->GetProjectionHeight() * 0.5f;
	float fCameraBottomBorder = vCameraPos.y - Camera()->GetProjectionHeight() * 0.5f;


	// Room의 leftBorder, rightBorder, topBorder, bottomBorder를 가져온다
	Vec3 vRoomCenterPos = m_Room->Collider2D()->GetWorldPos();
	float fRoomLeftBorder = vRoomCenterPos.x - m_Room->Transform()->GetWorldScale().x * 0.5f;
	float fRoomRightBorder = vRoomCenterPos.x + m_Room->Transform()->GetWorldScale().x * 0.5f;
	float fRoomTopBorder = vRoomCenterPos.y + m_Room->Transform()->GetWorldScale().y * 0.5f;
	float fRoomBottomBorder = vRoomCenterPos.y - m_Room->Transform()->GetWorldScale().y * 0.5f;

	Vec3 vDir = m_Player->Transform()->GetWorldPos() - vRoomCenterPos;

	// 카메라가 Room의 어느쪽 벽에 붙어야하는지 결정한다
	m_ChangeRoomTargetPos = vCameraPos;

	// 카메라의 TopBorder가 Room의 TopBorder보다 높으면
	if (fCameraTopBorder > fRoomTopBorder)
	{
		// 카메라가 Room의 TopBorder에 붙어야함.
		m_ChangeRoomTargetPos.y = fRoomTopBorder - Camera()->GetProjectionHeight() * 0.5f;
	}

	// 카메라의 BottomBorder가 Room의 BottomBorder보다 낮으면
	if (fCameraBottomBorder < fRoomBottomBorder)
	{
		// 카메라가 Room의 BottomBorder에 붙어야함.
		m_ChangeRoomTargetPos.y = fRoomBottomBorder + Camera()->GetProjectionHeight() * 0.5f;
	}

	// 카메라의 LeftBorder가 Room의 LeftBorder보다 왼쪽에 있으면
	if (fCameraLeftBorder < fRoomLeftBorder)
	{
		// 카메라가 Room의 LeftBorder에 붙어야함.
		m_ChangeRoomTargetPos.x = fRoomLeftBorder + Camera()->GetProjectionWidth() * 0.5f;
	}

	// 카메라의 RightBorder가 Room의 RightBorder보다 오른쪽에 있으면
	if (fCameraRightBorder > fRoomRightBorder)
	{
		// 카메라가 Room의 RightBorder에 붙어야함.
		m_ChangeRoomTargetPos.x = fRoomRightBorder - Camera()->GetProjectionWidth() * 0.5f;
	}


	//if (vDir.x > 0.f)
	//{
	//	// 카메라가 Room의 오른쪽 벽에 붙어야함.
	//	m_ChangeRoomTargetPos.x = fRoomRightBorder - Camera()->GetProjectionWidth() * 0.5f;
	//}
	//else if (vDir.x < 0.f)
	//{
	//	// 카메라가 Room의 왼쪽 벽에 붙어야함.
	//	m_ChangeRoomTargetPos.x = fRoomLeftBorder + Camera()->GetProjectionWidth() * 0.5f;
	//}

	//if (vDir.y > 0.f)
	//{
	//	// 카메라가 Room의 위쪽 벽에 붙어야함.
	//	m_ChangeRoomTargetPos.y = fRoomTopBorder - Camera()->GetProjectionHeight() * 0.5f;
	//}
	//else if (vDir.y < 0.f)
	//{
	//	// 카메라가 Room의 아래쪽 벽에 붙어야함.
	//	m_ChangeRoomTargetPos.y = fRoomBottomBorder + Camera()->GetProjectionHeight() * 0.5f;
	//}
}


void CCameraMoveScript::SaveToFile(FILE* _File)
{
	fwrite(&m_CamTrackingSpeed, sizeof(float), 1, _File);
}

void CCameraMoveScript::LoadFromFile(FILE* _File)
{
	fread(&m_CamTrackingSpeed, sizeof(float), 1, _File);
}