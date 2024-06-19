#include "pch.h"
#include "CLevelMgr.h"

#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "components.h"

#include "CAssetMgr.h"
#include "CCollisionMgr.h"
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

	// Level 생성
	m_CurLevel = new CLevel;
	// 아래 부분은 CLevel 생성자에서 해주는게 낫지 않을까하는 생각이...
	m_CurLevel->GetLayer(0)->SetName(L"Default");
	m_CurLevel->GetLayer(1)->SetName(L"Background");
	m_CurLevel->GetLayer(2)->SetName(L"Tile");
	m_CurLevel->GetLayer(3)->SetName(L"Player");
	m_CurLevel->GetLayer(4)->SetName(L"Monster");
	m_CurLevel->GetLayer(5)->SetName(L"PlayerProjectile");
	m_CurLevel->GetLayer(6)->SetName(L"MonsterProjectile");

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
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CFlipBookComponent);
	pObject->AddComponent(new CPlayerScript);

	pObject->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	pObject->Transform()->SetRelativeScale(200.f, 200.f, 1.f);

	pObject->Collider2D()->SetIndependentScale(true);
	pObject->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	pObject->Collider2D()->SetScale(Vec3(200.f, 200.f, 1.f));

	pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(pMtrl);

	pObject->FlipBookComponent()->AddFlipBook(5, CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"Link_MoveDown"));
	pObject->FlipBookComponent()->Play(5, 10, true);

	//// Child Object
	//CGameObject* pChild = new CGameObject;
	//pChild->SetName(L"Child");

	//pChild->AddComponent(new CTransform);
	//pChild->AddComponent(new CMeshRender);
	//pChild->AddComponent(new CCollider2D);

	//pChild->Transform()->SetRelativePos(400.f, 0.f, 0.f);
	//pChild->Transform()->SetRelativeScale(100.f, 100.f, 1.f);
	//pChild->Transform()->SetIndependentScale(true);

	//pChild->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	//pChild->Collider2D()->SetScale(Vec3(1.2f, 1.2f, 1.f));

	//pChild->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pChild->MeshRender()->SetMaterial(pMtrl);

	//pObject->AddChild(pChild);

	m_CurLevel->AddObject(3, pObject);

	//// Monster Object
	//CGameObject* pMonster = new CGameObject;
	//pMonster->SetName(L"Monster");

	//pMonster->AddComponent(new CTransform);
	//pMonster->AddComponent(new CMeshRender);
	//pMonster->AddComponent(new CCollider2D);

	//pMonster->Transform()->SetRelativePos(-400.f, 0.f, 100.f);
	//pMonster->Transform()->SetRelativeScale(150.f, 150.f, 1.f);

	//pMonster->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	//pMonster->Collider2D()->SetScale(Vec3(1.2f, 1.2f, 1.f));

	//pMonster->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pMonster->MeshRender()->SetMaterial(pMtrl);

	//m_CurLevel->AddObject(4, pMonster);


	// TileMap Object
	CGameObject* pTileMapObj = new CGameObject;
	pTileMapObj->SetName(L"TileMap");

	pTileMapObj->AddComponent(new CTransform);
	pTileMapObj->AddComponent(new CTileMap);

	pTileMapObj->Transform()->SetRelativePos(Vec3(-500.f, 250.f, 500.f));

	pTileMapObj->TileMap()->SetRowCol(4, 4);
	pTileMapObj->TileMap()->SetTileSize(Vec2(64.f, 64.f));

	Ptr<CTexture> pTileAtlas = CAssetMgr::GetInst()->Load<CTexture>(L"TileAtlasTex", L"texture\\TILE.bmp");
	pTileMapObj->TileMap()->SetAtlasTexture(pTileAtlas);
	pTileMapObj->TileMap()->SetAtlasTileResolution(Vec2(64.f, 64.f));

	m_CurLevel->AddObject(2, pTileMapObj);

	// 충돌 지정
	CCollisionMgr::GetInst()->CollisionCheck(3, 4); // Player vs Monster
	CCollisionMgr::GetInst()->CollisionCheck(5, 4); // Player Projectile vs Monster

	// 레벨 시작
	m_CurLevel->Begin();
}

void CLevelMgr::Progress()
{
	if (m_CurLevel->GetState() == LEVEL_STATE::PLAY)
	{
		m_CurLevel->Tick();
	}

	// 레벨이 정지해 있어도 finaltick은 돌려야함
	m_CurLevel->ClearObject();
	m_CurLevel->FinalTick();
}
