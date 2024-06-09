#include "pch.h"
#include "CLevelMgr.h"

#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "components.h"

#include "CAssetMgr.h"
#include "assets.h"

#include "CPlayerScript.h"
#include "CCameraMoveScript.h"

CLevelMgr::CLevelMgr()
	: m_CurLevel(nullptr)
{

}

CLevelMgr::~CLevelMgr()
{
	if (nullptr != m_CurLevel)
		delete m_CurLevel;
}

void CLevelMgr::Init()
{
	// Std2DMtrl
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl");
	Ptr<CMaterial> pAlphaBlendMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DAlphaBlendMtrl");

	Ptr<CTexture> pTexture = CAssetMgr::GetInst()->Load<CTexture>(L"PlayerTex", L"texture//Character.png");
	pAlphaBlendMtrl->SetTexParam(TEX_0, pTexture);

	m_CurLevel = new CLevel;

	// 카메라 오브젝트
	CGameObject* CamObj = new CGameObject;
	CamObj->SetName(L"MainCamera");
	CamObj->AddComponent(new CTransform);
	CamObj->AddComponent(new CCamera);
	CamObj->AddComponent(new CCameraMoveScript);

	// 우선순위를 0 : MainCamera 로 설정
	CamObj->Camera()->SetPriority(0);

	// 카메라 레이어 설정 (31번 레이어 제외 모든 레이어를 촬영)
	CamObj->Camera()->SetLayerAll();
	CamObj->Camera()->SetLayer(31, false);
	CamObj->Camera()->SetFar(10000.f);
	CamObj->Camera()->SetProjType(ORTHOGRAPHIC);

	m_CurLevel->AddObject(0, CamObj);

	CGameObject* pObject = nullptr;

	// 플레이어 오브젝트
	pObject = new CGameObject;
	pObject->SetName(L"Player");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CPlayerScript);

	pObject->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	pObject->Transform()->SetRelativeScale(200.f, 200.f, 1.f);
	pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(pAlphaBlendMtrl);

	m_CurLevel->AddObject(0, pObject);


	// 몬스터 오브젝트	
	pObject = new CGameObject;
	pObject->SetName(L"Monster");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetRelativePos(100.f, 0.f, 200.f);
	pObject->Transform()->SetRelativeScale(200.f, 200.f, 1.f);

	pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(pMtrl);

	m_CurLevel->AddObject(0, pObject);

	m_CurLevel->Begin();
}

void CLevelMgr::Progress()
{
	m_CurLevel->Tick();
	m_CurLevel->FinalTick();
}
