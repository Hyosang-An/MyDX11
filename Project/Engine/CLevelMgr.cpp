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
	// Material
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl");
	Ptr<CMaterial> pAlphaBlendMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DAlphaBlendMtrl");
	Ptr<CMaterial> pDebugShapeMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl");

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
	CamObj->Camera()->SetFar(100000.f);
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
	pObject->MeshRender()->SetMaterial(pMtrl);
	pObject->MeshRender()->GetMaterial()->SetScalarParam(INT_1, 1);
	pObject->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, 0.01f);
	pObject->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, Vec4(0.f, 1.f, 0.f, 1.f));

	// Child Object
	CGameObject* pChild = new CGameObject;
	pChild->SetName(L"Child");

	pChild->AddComponent(new CTransform);
	pChild->AddComponent(new CMeshRender);

	pChild->Transform()->SetRelativePos(200.f, 0.f, 0.f);
	pChild->Transform()->SetRelativeScale(0.8f, 0.8f, 1.f);

	pChild->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pChild->MeshRender()->SetMaterial(pMtrl);

	pObject->AddChild(pChild);

	m_CurLevel->AddObject(0, pObject);


	m_CurLevel->Begin();
}

void CLevelMgr::Progress()
{
	m_CurLevel->Tick();
	m_CurLevel->ClearObject();
	m_CurLevel->FinalTick();
}
