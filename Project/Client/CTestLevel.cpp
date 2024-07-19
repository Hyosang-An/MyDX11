#include "pch.h"
#include "CTestLevel.h"

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>

#include <Engine/CCollisionMgr.h>

#include <Scripts/CPlayerScript.h>
#include <Scripts/CMissileScript.h>
#include <Scripts/CCameraMoveScript.h>

void CTestLevel::CreateTestLevel()
{
	// Material
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl");
	Ptr<CMaterial> pAlphaBlendMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DAlphaBlendMtrl");
	Ptr<CMaterial> pDebugShapeMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl");

	Ptr<CTexture> pTexture = CAssetMgr::GetInst()->Load<CTexture>(L"PlayerTex", L"texture//Character.png");
	//Ptr<CTexture> pTexture = CAssetMgr::GetInst()->Load<CTexture>(L"PlayerTex", L"texture//bloomTest.png");
	pAlphaBlendMtrl->SetTexParam(TEX_0, pTexture);

	// Level ����
	CLevel* TestLevel = new CLevel;

	// ���� ����
	ChangeLevel(TestLevel, LEVEL_STATE::PLAY);


	// �Ʒ� �κ��� CLevel �����ڿ��� ���ִ°� ���� �������ϴ� ������...
	TestLevel->GetLayer(0)->SetName(L"Default");
	TestLevel->GetLayer(1)->SetName(L"Background");
	TestLevel->GetLayer(2)->SetName(L"Tile");
	TestLevel->GetLayer(3)->SetName(L"Player");
	TestLevel->GetLayer(4)->SetName(L"Monster");
	TestLevel->GetLayer(5)->SetName(L"PlayerProjectile");
	TestLevel->GetLayer(6)->SetName(L"MonsterProjectile");

	// ī�޶� ������Ʈ
	CGameObject* CamObj = new CGameObject;
	CamObj->SetName(L"MainCamera");
	CamObj->AddComponent(new CTransform);
	CamObj->AddComponent(new CCamera);
	CamObj->AddComponent(new CCameraMoveScript);

	// �켱������ 0 : MainCamera �� ����
	CamObj->Camera()->SetPriority(0);

	// ī�޶� ���̾� ���� (31�� ���̾� ���� ��� ���̾ �Կ�)
	CamObj->Camera()->SetLayerAll();
	CamObj->Camera()->SetLayer(31, false);
	CamObj->Camera()->SetFar(100000.f);
	CamObj->Camera()->SetProjType(ORTHOGRAPHIC);

	TestLevel->AddObject(0, CamObj);

	CGameObject* pObject = nullptr;

	// ���� ������Ʈ �߰�
	pObject = new CGameObject;
	pObject->SetName(L"PointLight 1");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight2D);

	pObject->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	pObject->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light2D()->SetRadius(1000);
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));

	TestLevel->AddObject(0, pObject);



	// �÷��̾� ������Ʈ
	CGameObject* pPlayer = new CGameObject;
	pPlayer->SetName(L"Player");
	pPlayer->AddComponent(new CTransform);
	pPlayer->AddComponent(new CMeshRender);
	pPlayer->AddComponent(new CCollider2D);
	//pPlayer->AddComponent(new CFlipBookComponent);
	pPlayer->AddComponent(new CPlayerScript);

	pPlayer->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	pPlayer->Transform()->SetRelativeScale(200.f, 200.f, 1.f);

	pPlayer->Collider2D()->SetIndependentScale(false);
	pPlayer->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	pPlayer->Collider2D()->SetScale(Vec3(1.f, 1.f, 1.f));

	pPlayer->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pPlayer->MeshRender()->SetMaterial(pMtrl);

	//pPlayer->FlipBookComponent()->AddFlipBook(5, CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"Link_MoveDown"));
	//pPlayer->FlipBookComponent()->Play(5, 10, true);

	TestLevel->AddObject(3, pPlayer);


	// TileMap Object
	CGameObject* pTileMapObj = new CGameObject;
	pTileMapObj->SetName(L"TileMap");

	pTileMapObj->AddComponent(new CTransform);
	pTileMapObj->AddComponent(new CTileMap);

	pTileMapObj->Transform()->SetRelativePos(Vec3(-500.f, 250.f, 500.f));

	pTileMapObj->TileMap()->SetRowCol(20, 20);
	pTileMapObj->TileMap()->SetTileSize(Vec2(64.f, 64.f));

	Ptr<CTexture> pTileAtlas = CAssetMgr::GetInst()->Load<CTexture>(L"TileAtlasTex", L"texture\\TILE.bmp");
	pTileMapObj->TileMap()->SetAtlasTexture(pTileAtlas);
	pTileMapObj->TileMap()->SetAtlasTileResolution(Vec2(64.f, 64.f));

	TestLevel->AddObject(2, pTileMapObj);


	// PostProcess0 Object
	CGameObject* pPostProcess_1_Obj = new CGameObject;
	pPostProcess_1_Obj->SetName(L"PostProcess_0");
	pPostProcess_1_Obj->AddComponent(new CTransform);
	pPostProcess_1_Obj->AddComponent(new CMeshRender);

	pPostProcess_1_Obj->Transform()->SetRelativePos(0, 0, 450);
	pPostProcess_1_Obj->Transform()->SetRelativeScale(150.f, 150.f, 1.f);


	pPostProcess_1_Obj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pPostProcess_1_Obj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DistortionMtrl"));
	//pGrayFilterObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ConvexLensMtrl"));

	TestLevel->AddObject(0, pPostProcess_1_Obj);


	//// PostProcess Object2 (GaussianBlur)
	//CGameObject* pGaussianBlurObj = new CGameObject;
	//pGaussianBlurObj->SetName(L"PostProcess_1");
	//pGaussianBlurObj->AddComponent(new CTransform);
	//pGaussianBlurObj->AddComponent(new CMeshRender);

	//pGaussianBlurObj->Transform()->SetRelativePos(0, 0, 450);
	//pGaussianBlurObj->Transform()->SetRelativeScale(150.f, 150.f, 1.f);


	//pGaussianBlurObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pGaussianBlurObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BloomMtrl"));

	//TestLevel->AddObject(0, pGaussianBlurObj);


	// �浹 ����
	CCollisionMgr::GetInst()->CollisionCheck(3, 4); // Player vs Monster
	CCollisionMgr::GetInst()->CollisionCheck(5, 4); // Player Projectile vs Monster


	// ���� ����
	//m_CurLevel->Begin();
}
