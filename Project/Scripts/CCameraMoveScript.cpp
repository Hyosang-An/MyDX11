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
	// �����ų ������
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
	// ī�޶� �÷��̾ ����� ������ ���ߵ����ؾ� ��.
	// �÷��̾ ȭ�� �߽��� ����� ī�޶� �÷��̾ ���󰡵��� �ؾ� ��.

	// �÷��̾� ������Ʈ�� ã�Ƽ� ����
	if (nullptr == m_Player)
	{
		m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
		return;
	}
	// �÷��̾ ���� ���� Room�� �����´�
	m_Room = m_Player->GetScript<CPlayerScript>()->GetRoom();
	if (nullptr == m_Room)
	{		
		return;
	}

	if (m_Room->Collider2D() == nullptr)
	{
		MessageBox(NULL, L"Room�� Collider2D�� �����ϴ�.", L"���", MB_OK);
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


	// ��׶��� ��ġ ������Ʈ
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

	//	// ���콺�� �̵��ϴ� ����
	//	//vMouseDragDelta.x; ==> y�� ȸ��;
	//	//vMouseDragDelta.y; ==> x�� ȸ��
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
	// Room�� Row, Col�� �����´�
	Vec2 vRoomRowCol = m_Room->TileMap()->GetRowCol();

	// Room�� Col�� 40���� �۰ų� Row�� 23���� ������ ��� MessageBox�� ����
	if (vRoomRowCol.x < 23 || vRoomRowCol.y < 40)
	{
		if (vRoomRowCol.x < 23)
		{
			wstring str = L"Room�� Row�� 23���� �۽��ϴ�.";
			int row = (int)vRoomRowCol.x;
			str += L"\n���� Row: " + to_wstring(row);
			MessageBox(NULL, str.c_str(), L"���", MB_OK);
		}

		if (vRoomRowCol.y < 40)
		{
			wstring str = L"Room�� Col�� 40���� �۽��ϴ�.";
			int col = (int)vRoomRowCol.y;
			str += L"\n���� Col: " + to_wstring(col);
			MessageBox(NULL, str.c_str(), L"���", MB_OK);
		}

		return;
	}


	Vec3 vPlayerWorldPos = m_Player->Transform()->GetWorldPos();
	Vec3 vCameraWorldPos = Transform()->GetWorldPos();

	Vec3 vDiff = vPlayerWorldPos - vCameraWorldPos;

	// ī�޶� �̵� �ӵ��� ������ ũ�⿡ ����ϵ��� �Ѵ�.
	float CameraSpeed = vDiff.Length() * 2.f;

	vCameraWorldPos += vDiff.Normalize() * CameraSpeed * DT;
	Transform()->SetWorldPos(vCameraWorldPos);


	// ī�޶� �����ִ� ������ ���� Room�� �Ѿ�� �ʵ��� �Ѵ�
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


	// ī�޶� Room�� ���� ��踦 �Ѿ�� �ʵ��� �Ѵ�
	if (fCameraLeftBorder < fRoomLeftBorder)
	{
		vCameraPos.x += fRoomLeftBorder - fCameraLeftBorder;
		Transform()->SetWorldPos(vCameraPos);
	}

	// ī�޶� Room�� ������ ��踦 �Ѿ�� �ʵ��� �Ѵ�
	if (fCameraRightBorder > fRoomRightBorder)
	{
		vCameraPos.x -= fCameraRightBorder - fRoomRightBorder;
		Transform()->SetWorldPos(vCameraPos);
	}

	// ī�޶� Room�� ���� ��踦 �Ѿ�� �ʵ��� �Ѵ�
	if (fCameraTopBorder > fRoomTopBorder)
	{
		vCameraPos.y -= fCameraTopBorder - fRoomTopBorder;
		Transform()->SetWorldPos(vCameraPos);
	}

	// ī�޶� Room�� �Ʒ��� ��踦 �Ѿ�� �ʵ��� �Ѵ�
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

	// ī�޶� m_ChangeRoomTargetPos�� �̵��Ѵ�
	Vec3 vCameraPos = Transform()->GetWorldPos();
	Vec3 vDiff = m_ChangeRoomTargetPos - vCameraPos;

	// ī�޶� �̵� �ӵ��� m_CamRoomChangeSpeed.
	Transform()->SetWorldPos(vCameraPos + vDiff.Normalize() * m_CamRoomChangeSpeed * EngineDT);

	// ī�޶� m_ChangeRoomTargetPos�� �����ϸ� m_bChangeRoom�� false�� �ٲ۴�
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

	// m_ChangeRoomTargetPos�� �����Ѵ�

	// �÷��̾� ������Ʈ�� ã�Ƽ� ����
	m_Player = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
	if (nullptr == m_Player)
	{
		MessageBox(NULL, L"�÷��̾� ������Ʈ�� ã�� �� �����ϴ�.", L"���", MB_OK);
		return;
	}

	// �÷��̾ ���� ���� Room�� �����´�
	m_Room = m_Player->GetScript<CPlayerScript>()->GetRoom();
	if (nullptr == m_Room)
	{
		MessageBox(NULL, L"�÷��̾ ���� Room�� ã�� �� �����ϴ�.", L"���", MB_OK);
		return;
	}

	// ī�޶��� leftBorder, rightBorder, topBorder, bottomBorder�� �����´�
	Vec3 vCameraPos = Transform()->GetWorldPos();
	float fCameraLeftBorder = vCameraPos.x - Camera()->GetProjectionWidth() * 0.5f;
	float fCameraRightBorder = vCameraPos.x + Camera()->GetProjectionWidth() * 0.5f;
	float fCameraTopBorder = vCameraPos.y + Camera()->GetProjectionHeight() * 0.5f;
	float fCameraBottomBorder = vCameraPos.y - Camera()->GetProjectionHeight() * 0.5f;


	// Room�� leftBorder, rightBorder, topBorder, bottomBorder�� �����´�
	Vec3 vRoomCenterPos = m_Room->Collider2D()->GetWorldPos();
	float fRoomLeftBorder = vRoomCenterPos.x - m_Room->Transform()->GetWorldScale().x * 0.5f;
	float fRoomRightBorder = vRoomCenterPos.x + m_Room->Transform()->GetWorldScale().x * 0.5f;
	float fRoomTopBorder = vRoomCenterPos.y + m_Room->Transform()->GetWorldScale().y * 0.5f;
	float fRoomBottomBorder = vRoomCenterPos.y - m_Room->Transform()->GetWorldScale().y * 0.5f;

	Vec3 vDir = m_Player->Transform()->GetWorldPos() - vRoomCenterPos;

	// ī�޶� Room�� ����� ���� �پ���ϴ��� �����Ѵ�
	m_ChangeRoomTargetPos = vCameraPos;

	// ī�޶��� TopBorder�� Room�� TopBorder���� ������
	if (fCameraTopBorder > fRoomTopBorder)
	{
		// ī�޶� Room�� TopBorder�� �پ����.
		m_ChangeRoomTargetPos.y = fRoomTopBorder - Camera()->GetProjectionHeight() * 0.5f;
	}

	// ī�޶��� BottomBorder�� Room�� BottomBorder���� ������
	if (fCameraBottomBorder < fRoomBottomBorder)
	{
		// ī�޶� Room�� BottomBorder�� �پ����.
		m_ChangeRoomTargetPos.y = fRoomBottomBorder + Camera()->GetProjectionHeight() * 0.5f;
	}

	// ī�޶��� LeftBorder�� Room�� LeftBorder���� ���ʿ� ������
	if (fCameraLeftBorder < fRoomLeftBorder)
	{
		// ī�޶� Room�� LeftBorder�� �پ����.
		m_ChangeRoomTargetPos.x = fRoomLeftBorder + Camera()->GetProjectionWidth() * 0.5f;
	}

	// ī�޶��� RightBorder�� Room�� RightBorder���� �����ʿ� ������
	if (fCameraRightBorder > fRoomRightBorder)
	{
		// ī�޶� Room�� RightBorder�� �پ����.
		m_ChangeRoomTargetPos.x = fRoomRightBorder - Camera()->GetProjectionWidth() * 0.5f;
	}


	//if (vDir.x > 0.f)
	//{
	//	// ī�޶� Room�� ������ ���� �پ����.
	//	m_ChangeRoomTargetPos.x = fRoomRightBorder - Camera()->GetProjectionWidth() * 0.5f;
	//}
	//else if (vDir.x < 0.f)
	//{
	//	// ī�޶� Room�� ���� ���� �پ����.
	//	m_ChangeRoomTargetPos.x = fRoomLeftBorder + Camera()->GetProjectionWidth() * 0.5f;
	//}

	//if (vDir.y > 0.f)
	//{
	//	// ī�޶� Room�� ���� ���� �پ����.
	//	m_ChangeRoomTargetPos.y = fRoomTopBorder - Camera()->GetProjectionHeight() * 0.5f;
	//}
	//else if (vDir.y < 0.f)
	//{
	//	// ī�޶� Room�� �Ʒ��� ���� �پ����.
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