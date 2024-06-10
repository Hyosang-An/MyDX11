#include "pch.h"
#include "CGameObject.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CLevelMgr.h"

#include "CLevel.h"
#include "CLayer.h"
#include "components.h"

CGameObject::CGameObject() :
	m_arrCom{},
	m_RenderCom{},
	m_Parent{}
{
}

CGameObject::~CGameObject()
{
	Delete_Array(m_arrCom);
	Delete_Vec(m_vecScript);
}

void CGameObject::AddComponent(CComponent* _Comopnent)
{
	COMPONENT_TYPE Type = _Comopnent->GetComponentType();

	if (COMPONENT_TYPE::SCRIPT == Type)
	{
		m_vecScript.push_back((CScript*)_Comopnent);
		_Comopnent->SetOwner(this);
	}
	else
	{
		assert(nullptr == m_arrCom[(UINT)Type]);

		m_arrCom[(UINT)Type] = _Comopnent;
		m_arrCom[(UINT)Type]->SetOwner(this);

		CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(_Comopnent);

		assert(!(pRenderCom && m_RenderCom));

		if (pRenderCom)
		{
			m_RenderCom = pRenderCom;
		}
	}
}

void CGameObject::AddChild(CGameObject* _ChildObject)
{
	m_vecChildren.push_back(_ChildObject);
	_ChildObject->m_Parent = this;
}

void CGameObject::DisconnectWithLayer()
{
	if (nullptr == m_Parent)
	{
		CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();
		CLayer* pLayer = pLevel->GetLayer(m_LayerIdx);
		pLayer->DisconnectWithObject(this);
	}

	m_LayerIdx = -1;
}

void CGameObject::Begin()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == m_arrCom[i])
			continue;

		m_arrCom[i]->Begin();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->Begin();
	}

	// 자식 오브젝트
	for (size_t i = 0; i < m_vecChildren.size(); ++i)
	{
		m_vecChildren[i]->Begin();
	}
}

void CGameObject::Tick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->Tick();
	}

	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->Tick();
	}

	// 자식 오브젝트
	for (size_t i = 0; i < m_vecChildren.size(); ++i)
	{
		m_vecChildren[i]->Tick();
	}
}

void CGameObject::FinalTick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->FinalTick();
	}

	// 레이어 등록
	assert(-1 != m_LayerIdx);
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CLayer* pLayer = pLevel->GetLayer(m_LayerIdx);
	pLayer->RegisterGameObject(this);

	// 자식 오브젝트
	for (size_t i = 0; i < m_vecChildren.size(); ++i)
	{
		m_vecChildren[i]->FinalTick();
	}
}

void CGameObject::Render()
{
	if (m_RenderCom)
		m_RenderCom->Render();
}