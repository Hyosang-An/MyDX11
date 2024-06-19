#include "pch.h"
#include "CRenderMgr.h"

#include "CCamera.h"
#include "CTimeMgr.h"
#include "CAssetMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CLevelMgr.h"
#include "CLevel.h"

CRenderMgr::CRenderMgr()
{
}

CRenderMgr::~CRenderMgr()
{
	if (nullptr != m_DebugObject)
		delete m_DebugObject;
}



void CRenderMgr::Init()
{
	// ����� �������� ���� ������Ʈ
	m_DebugObject = new CGameObject;
	m_DebugObject->AddComponent(new CTransform);
	m_DebugObject->AddComponent(new CMeshRender);
	m_DebugObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"));
}

void CRenderMgr::Tick()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (nullptr == pCurLevel)
		return;

	// Level �� Player ������ ���, Level ���� �ִ� ī�޶� �������� �������ϱ�
	if (PLAY == pCurLevel->GetState())
	{
		for (size_t i = 0; i < m_vecCam.size(); ++i)
		{
			if (nullptr == m_vecCam[i])
				continue;

			m_vecCam[i]->Render();
		}
	}

	// Level �� Stop �̳� Pause �� ���, Editor �� ī�޶� �������� ������ �ϱ�
	else
	{
		if (nullptr != m_EditorCamera)
		{
			m_EditorCamera->Render();
		}
	}

	RenderDebugShape();
}

void CRenderMgr::RegisterCamera(CCamera* _cam, int _camPriority)
{
	// ī�޶� �켱������ ���� ������ ������ ���õǾ�� �Ѵ�.
	if (m_vecCam.size() <= _camPriority + 1)
		m_vecCam.resize(_camPriority + 1);

	// ī�޶� �켱������ �´� ��ġ�� �ִ´�
	m_vecCam[_camPriority] = _cam;
}

void CRenderMgr::RenderDebugShape()
{
	list<tDebugShapeInfo>::iterator iter = m_DebugShapeList.begin();

	for (; iter != m_DebugShapeList.end(); )
	{
		// ����� ��û ��翡 �´� �޽ø� ������
		switch ((*iter).Shape)
		{
			case DEBUG_SHAPE::RECT:
				m_DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
				break;
			case DEBUG_SHAPE::CIRCLE:
				m_DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
				break;
			case DEBUG_SHAPE::LINE:

				break;
			case DEBUG_SHAPE::CUBE:

				break;
			case DEBUG_SHAPE::SPHERE:

				break;
		}

		// ��ġ ����
		m_DebugObject->Transform()->SetWorldMatrix((*iter).matWorld);

		// ���� ����
		m_DebugObject->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, (*iter).vColor);

		// �������� ���ο� ����, ���̴��� �������� ����� �����Ѵ�.
		if ((*iter).DepthTest)
			m_DebugObject->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::LESS);
		else
			m_DebugObject->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

		// ������
		m_DebugObject->MeshRender()->Render();


		// ������ ���� ����� ������ ����
		(*iter).Age += DT;
		if ((*iter).LifeTime < (*iter).Age)
		{
			iter = m_DebugShapeList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}