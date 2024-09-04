#include "pch.h"
#include "CEditorCameraScript.h"
#include "Engine/CRenderMgr.h"

#include "ImGui/imgui.h"

CEditorCameraScript::CEditorCameraScript() :
	CScript(-1),
	m_Speed(500.f)
{

}

CEditorCameraScript::~CEditorCameraScript()
{

}

void CEditorCameraScript::Tick()
{
	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
	{
		OrthoGraphicMove();
	}

	else if (PROJ_TYPE::PERSPECTIVE == Camera()->GetProjType())
	{
		PerspectiveMove();
	}


	if (KEY_JUST_PRESSED(KEY::_0))
	{
		if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
			Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		else
			Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	}
}



void CEditorCameraScript::OrthoGraphicMove()
{
	float Speed = m_Speed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		Speed *= 3.f;
	}

	Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos.y += EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos.y -= EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos.x -= EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos.x += EngineDT * Speed;
	}

	Transform()->SetRelativePos(vPos);

	//double zoomRatio = Camera()->GetScale();

	if (KEY_PRESSED(KEY::CTRL) && 0 < ImGui::GetIO().MouseWheel)
	{
		m_ZoomRatio += 0.05f;
	}

	if (KEY_PRESSED(KEY::CTRL) && 0 > ImGui::GetIO().MouseWheel)
	{
		m_ZoomRatio -= 0.05f;
	}

	if (KEY_JUST_PRESSED(KEY::_9))
		m_ZoomRatio += 0.1;

	if (KEY_JUST_PRESSED(KEY::_8))
		m_ZoomRatio -= 0.1;

	Camera()->SetScale(1.f/m_ZoomRatio);


	wchar_t buffer[255]{}; // 결과를 저장할 버퍼
	swprintf_s(buffer, L"Zoom Ratio : %.0f%%", m_ZoomRatio * 100);


	// 줌 비율 출력 (백분율로)
	CRenderMgr::GetInst()->AddText(buffer, Vec2(10, 50), 20, Vec4(255.f, 255.f, 255.f, 255.f), 0);

}

void CEditorCameraScript::PerspectiveMove()
{
	float Speed = m_Speed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		Speed *= 3.f;
	}

	Vec3 vFront = Transform()->GetWorldDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetWorldDir(DIR::RIGHT);

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos += vFront * EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos -= vFront * EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos -= vRight * EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos += vRight * EngineDT * Speed;
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
	//	vRot.y += vMouseDragDelta.x * XM_PI * EngineDT * 10.f;
	//	vRot.x += vMouseDragDelta.y * XM_PI * EngineDT * 10.f;
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

	static bool mousecontrol = false;
	if (KEY_JUST_PRESSED(KEY::ESC))
		mousecontrol = !mousecontrol;

	if (mousecontrol)
	{
		Vec2 vMouseDragDelta = CKeyMgr::GetInst()->GetMouseDragDelta();

		Vec3 vRot = Transform()->GetRelativeRoatation();
		vRot.y += vMouseDragDelta.x * XM_PI * 0.001f;
		vRot.x += vMouseDragDelta.y * XM_PI * 0.001f;
		Transform()->SetRelativeRotation(vRot);
	}


}
