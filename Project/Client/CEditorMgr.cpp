#include "pch.h"
#include "CEditorMgr.h"

#include <Engine/CRenderMgr.h>

#include "CGameObjectEx.h"
#include <Engine/components.h>

CEditorMgr::CEditorMgr()
{

}

CEditorMgr::~CEditorMgr()
{
	Delete_Vec(m_vecEditorObject);
}

void CEditorMgr::Init()
{
	CreateEditorObject();
}

void CEditorMgr::Tick()
{
	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->FinalTick();
	}
}



void CEditorMgr::CreateEditorObject()
{
	CGameObject* pEditorCamera = new CGameObjectEx;
	pEditorCamera->SetName(L"EditorCamera");
	pEditorCamera->AddComponent(new CTransform);
	pEditorCamera->AddComponent(new CCamera);
	//pEditorCamera->AddComponent(new CEditorCameraScript);

	pEditorCamera->Camera()->SetLayerAll();
	pEditorCamera->Camera()->SetLayer(31, false);
	pEditorCamera->Camera()->SetFar(100000.f);
	pEditorCamera->Camera()->SetProjType(ORTHOGRAPHIC);

	m_vecEditorObject.push_back(pEditorCamera);

	CRenderMgr::GetInst()->RegisterEditorCamera(pEditorCamera->Camera());
}
