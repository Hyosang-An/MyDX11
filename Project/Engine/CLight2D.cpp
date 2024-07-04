#include "pch.h"
#include "CLight2D.h"

#include "CTransform.h"
#include "CRenderMgr.h"


CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
	, m_Info{}
{
	// 자신을 RenderMgr 에 등록시킴
	CRenderMgr::GetInst()->RegisterLight2D(this);
}

CLight2D::~CLight2D()
{
	CRenderMgr::GetInst()->DeregisterLight2D(this);
}

void CLight2D::Begin()
{

}

void CLight2D::FinalTick()
{
	m_Info.WorldPos = Transform()->GetWorldPos();
	m_Info.WorldDir = Transform()->GetWorldDir(DIR::RIGHT);

	//// 자신을 RenderMgr 에 등록시킴
	//CRenderMgr::GetInst()->RegisterLight2D(this);
}

void CLight2D::SetLightType(LIGHT_TYPE _Type)
{
	m_Info.Type = _Type;
}