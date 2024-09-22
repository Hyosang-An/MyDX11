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
#include <Scripts/CRigidBody.h>
#include <Scripts/CMissileScript.h>
#include <Scripts/CCameraMoveScript.h>
#include <Scripts/CDashShockWaveEffectScript.h>

#include "CLevelSaveLoad.h"

#include <Engine/CSetColorCS.h>

#include <Engine/CStructuredBuffer.h>

void CTestLevel::CreateTestLevel()
{
	// Material
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl");
	Ptr<CMaterial> pAlphaBlendMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DAlphaBlendMtrl");
	Ptr<CMaterial> pDebugShapeMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl");

	Ptr<CTexture> pTexture = CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture//Character.png");
	pAlphaBlendMtrl->SetTexParam(TEX_0, pTexture);


	//CreatePrefab();

	// Sound ���
	//Ptr<CSound> pSound = CAssetMgr::GetInst()->FindAsset<CSound>(L"sound\\BGM_title_screen.wav");
	//pSound->Play(0, 1.f, false);

	// ��ǻƮ ���̴� �׽�Ʈ�� �ؽ��� ����
	//Ptr<CTexture> pTestTex = CAssetMgr::GetInst()->CreateTexture(L"ComputeShaderTestTex"
	//	, 1026, 1026, DXGI_FORMAT_R8G8B8A8_UNORM
	//	, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	//CSetColorCS cs;
	//cs.SetTargetTexture(pTestTex);
	//cs.SetClearColor(Vec4(0.f, 1.f, 0.f, 1.f));
	//cs.Execute();
	//pMtrl->SetTexParam(TEX_0, pTestTex);


	// Level ����
	CLevel* TestLevel = new CLevel;


	// �Ʒ� �κ��� CLevel �����ڿ��� ���ִ°� ���� �������ϴ� ������...
	//TestLevel->GetLayer(0)->SetName(L"Default");
	//TestLevel->GetLayer(1)->SetName(L"Background");
	//TestLevel->GetLayer(2)->SetName(L"Tile");
	//TestLevel->GetLayer(3)->SetName(L"Player");
	//TestLevel->GetLayer(4)->SetName(L"Monster");
	//TestLevel->GetLayer(5)->SetName(L"PlayerProjectile");
	//TestLevel->GetLayer(6)->SetName(L"MonsterProjectile");

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



	////�÷��̾� ������Ʈ
	//CGameObject* pPlayer = new CGameObject;
	//pPlayer->SetName(L"Player");
	//pPlayer->AddComponent(new CTransform);
	//pPlayer->AddComponent(new CMeshRender);
	//pPlayer->AddComponent(new CCollider2D);
	//pPlayer->AddComponent(new CFlipBookComponent);
	//pPlayer->AddComponent(new CPlayerScript);
	//pPlayer->AddComponent(new CRigidBody);

	//pPlayer->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	//pPlayer->Transform()->SetRelativeScale(200.f, 200.f, 1.f);

	//pPlayer->Collider2D()->SetIndependentScale(false);
	//pPlayer->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	//pPlayer->Collider2D()->SetScale(Vec3(1.f, 1.f, 1.f));

	//pPlayer->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pPlayer->MeshRender()->SetMaterial(pMtrl);

	//pPlayer->FlipBookComponent()->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\Link_MoveDown.flip"));
	//pPlayer->FlipBookComponent()->Play(L"Link_MoveDown", 10, true);

	//TestLevel->AddObject(3, pPlayer);

	// �÷��̾� ������Ʈ
	//CGameObject* pPlayer = new CGameObject;
	//pPlayer->SetName(L"Player");
	//pPlayer->AddComponent(new CTransform);
	//pPlayer->AddComponent(new CMeshRender);
	//pPlayer->AddComponent(new CCollider2D);
	//pPlayer->AddComponent(new CFlipBookComponent);
	//pPlayer->AddComponent(new CPlayerScript);
	//pPlayer->AddComponent(new CRigidBody);

	//pPlayer->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	//pPlayer->Transform()->SetRelativeScale(350, 350, 1.f);

	//pPlayer->Collider2D()->SetIndependentScale(true);
	//pPlayer->Collider2D()->SetOffset(Vec3(0.f, -64.f, 0.f));
	//pPlayer->Collider2D()->SetScale(Vec3(60.f, 96.f, 1.f));

	//pPlayer->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pPlayer->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl"));

	//pPlayer->FlipBookComponent()->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\Idle\\Idle.flip"));
	//pPlayer->FlipBookComponent()->Play(L"Idle", 10, true);

	//TestLevel->AddObject(3, pPlayer);


	//// Monster Object
	//CGameObject* pMonster = new CGameObject;
	//pMonster->SetName(L"Monster");

	//pMonster->AddComponent(new CTransform);
	//pMonster->AddComponent(new CMeshRender);
	//pMonster->AddComponent(new CCollider2D);

	//pMonster->Transform()->SetRelativePos(-400.f, 0.f, 100.f);
	//pMonster->Transform()->SetRelativeScale(150.f, 150.f, 1.f);

	//pMonster->Collider2D()->SetOffsetPixel(Vec3(0.f, 0.f, 0.f));
	//pMonster->Collider2D()->SetScale(Vec3(1.2f, 1.2f, 1.f));

	//pMonster->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pMonster->MeshRender()->SetMaterial(pMtrl);

	//TestLevel->AddObject(4, pMonster);


	// TileMap Object
	CGameObject* pTileMapObj = new CGameObject;
	pTileMapObj->SetName(L"TileMap");

	pTileMapObj->AddComponent(new CTransform);
	pTileMapObj->AddComponent(new CTileMap);

	pTileMapObj->Transform()->SetRelativePos(Vec3(-500.f, 250.f, 500.f));

	pTileMapObj->TileMap()->SetRowCol(20, 20);
	pTileMapObj->TileMap()->SetTileSize(Vec2(64.f, 64.f)); // �ػ󵵰� �ƴ� ���ӻ� Scale

	Ptr<CTexture> pTileAtlas = CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\TILE.bmp");
	pTileMapObj->TileMap()->SetAtlasTexture(pTileAtlas, Vec2(64.f, 64.f));

	auto& tileInfoVec = pTileMapObj->TileMap()->GetTileInfoVec();
	for (int i = 0; i < tileInfoVec.size(); i++)
	{
		tileInfoVec[i].ImgIdx = i;
	}

	TestLevel->AddObject(2, pTileMapObj);


	//// PostProcess0 Object
	//CGameObject* pPostProcess_1_Obj = new CGameObject;
	//pPostProcess_1_Obj->SetName(L"PostProcess_0");
	//pPostProcess_1_Obj->AddComponent(new CTransform);
	//pPostProcess_1_Obj->AddComponent(new CMeshRender);

	//pPostProcess_1_Obj->Transform()->SetRelativePos(0, 0, 450);
	//pPostProcess_1_Obj->Transform()->SetRelativeScale(150.f, 150.f, 1.f);


	//pPostProcess_1_Obj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pPostProcess_1_Obj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DistortionMtrl"));
	////pGrayFilterObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ConvexLensMtrl"));

	//TestLevel->AddObject(0, pPostProcess_1_Obj);


	// Particle Object
	//CGameObject* pParticleObj = new CGameObject;
	//pParticleObj->SetName(L"Particle");

	//pParticleObj->AddComponent(new CTransform);
	//pParticleObj->AddComponent(new CParticleSystem);

	//pParticleObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));

	//TestLevel->AddObject(0, pParticleObj);


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



	// PostProcess Object2 (DashShockWaveObj)
	CGameObject* pDashShockWaveObj = new CGameObject;
	pDashShockWaveObj->SetName(L"DashShockWaveObj");
	pDashShockWaveObj->AddComponent(new CTransform);
	pDashShockWaveObj->AddComponent(new CMeshRender);

	pDashShockWaveObj->Transform()->SetRelativePos(200, -200, 450);
	pDashShockWaveObj->Transform()->SetRelativeScale(500.f, 500.f, 1.f);


	pDashShockWaveObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh"));
	pDashShockWaveObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DashShockWaveMtrl"));

	pDashShockWaveObj->AddComponent(new CDashShockWaveEffectScript);

	TestLevel->AddObject(0, pDashShockWaveObj);

	ChangeLevel(TestLevel, LEVEL_STATE::STOP);

	// �浹 ����
	CCollisionMgr::GetInst()->CollisionCheck(3, 4); // Player vs Monster
	CCollisionMgr::GetInst()->CollisionCheck(5, 4); // Player Projectile vs Monster


	

}

void CTestLevel::CreatePrefab()
{
	//CGameObject* pProto = new CGameObject;

	//pProto->SetName(L"MissilePrefab");

	//pProto->AddComponent(new CTransform);
	//pProto->AddComponent(new CMeshRender);
	//pProto->AddComponent(new CMissileScript);

	//pProto->Transform()->SetRelativeScale(100.f, 100.f, 1.f);

	//pProto->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pProto->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl"));

	//Ptr<CPrefab> pPrefab = new CPrefab;
	//pPrefab->SetProtoObject(pProto);

	////CAssetMgr::GetInst()->AddAsset<CPrefab>(L"MissilePref", pPrefab);




	//wstring FilePath = CPathMgr::GetInst()->GetContentPath();
	//FilePath += L"prefab\\Missile.prefab";
	//pPrefab->Save(FilePath);
}

void CTestLevel::CreateCelesteTestLevel()
{
	// Level ����
	CLevel* TestLevel = new CLevel;

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

	TestLevel->AddObject(LAYER::DEFAULT, CamObj);

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

	TestLevel->AddObject(LAYER::DEFAULT, pObject);



	//�÷��̾� ������Ʈ
	//CGameObject* pPlayer = new CGameObject;
	//pPlayer->SetName(L"Player");
	//pPlayer->AddComponent(new CTransform);
	//pPlayer->AddComponent(new CMeshRender);
	//pPlayer->AddComponent(new CCollider2D);
	//pPlayer->AddComponent(new CFlipBookComponent);
	//pPlayer->AddComponent(new CPlayerScript);
	//pPlayer->AddComponent(new CRigidBody);

	//pPlayer->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	//pPlayer->Transform()->SetRelativeScale(350, 350, 1.f);

	//pPlayer->Collider2D()->SetIndependentScale(true);
	//pPlayer->Collider2D()->SetOffset(Vec3(0.f, -64.f, 0.f));
	//pPlayer->Collider2D()->SetScale(Vec3(60.f, 96.f, 1.f));

	//pPlayer->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pPlayer->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl"));

	//pPlayer->FlipBookComponent()->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\Idle\\Idle.flip"));
	//pPlayer->FlipBookComponent()->Play(L"Idle", 10, true);




	//auto pPlayer = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\Player.prefab")->Instantiate();
	//pPlayer->FlipBookComponent()->Play(L"Idle");
	//
	//TestLevel->AddObject(LAYER::PLAYER, pPlayer);


	//// TileMap Object
	//CGameObject* pTileMapObj = new CGameObject;
	//pTileMapObj->SetName(L"Room");

	//pTileMapObj->AddComponent(new CTransform);
	//pTileMapObj->AddComponent(new CTileMap);

	//pTileMapObj->Transform()->SetRelativePos(Vec3(-500.f, 250.f, 500.f));

	//pTileMapObj->TileMap()->SetRowCol(200, 100);
	//pTileMapObj->TileMap()->SetTileSize(Vec2(58.f, 58.f)); // �ػ󵵰� �ƴ� ���ӻ� Scale

	//Ptr<CTexture> pTileAtlas = CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\TILE.bmp");
	//pTileMapObj->TileMap()->SetAtlasTexture(pTileAtlas, Vec2(64.f, 64.f));

	//TestLevel->AddObject(LAYER::TILEMAP, pTileMapObj);



	// ��ƼŬ ������Ʈ �׽�Ʈ (����)
	// Particle Object
	//CGameObject* pParticleObj = new CGameObject;
	//pParticleObj->SetName(L"Particle Snow");

	//pParticleObj->AddComponent(new CTransform);
	//pParticleObj->Transform()->SetRelativePos(Vec3(1000.f, 0.f, 0.f));
	//pParticleObj->Transform()->SetRelativeScale(1000.f, 1200.f, 1.f);

	//pParticleObj->AddComponent(new CMyParticleSystem);


	//TestLevel->AddObject(0, pParticleObj);



	// ��ƼŬ ������Ʈ �׽�Ʈ (�뽬)
	// Particle Object
	//CGameObject* pParticleObj2 = new CGameObject;
	//pParticleObj2->SetName(L"Particle Dash");

	//pParticleObj2->AddComponent(new CTransform);
	//pParticleObj2->Transform()->SetRelativePos(Vec3(0, 0.f, 0.f));
	//pParticleObj2->Transform()->SetRelativeScale(50, 50.f, 1.f);

	//pParticleObj2->AddComponent(new CMyParticleSystem(1));
	//pParticleObj2->MyParticleSystem()->SetReferenceDir(Vec3(1.f, 0.f, 0.f));

	//TestLevel->AddObject(LAYER::DEFAULT, pParticleObj2);


	// refill ������Ʈ
	CGameObject* pRefillObj = new CGameObject;
	pRefillObj->SetName(L"Refill");

	pRefillObj->AddComponent(new CTransform);
	pRefillObj->AddComponent(new CMeshRender);
	pRefillObj->AddComponent(new CCollider2D);
	pRefillObj->AddComponent(new CFlipBookComponent);
	//pRefillObj->AddComponent(new CRefillScript);
	

	pRefillObj->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	pRefillObj->Transform()->SetRelativeScale(50.f, 50.f, 1.f);

	pRefillObj->Collider2D()->SetScale(Vec3(1.f, 1.f, 1.f));

	pRefillObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pRefillObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl"));

	pRefillObj->FlipBookComponent()->AddFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(L"animation\\Object\\refill\\refill.flip"));
	pRefillObj->FlipBookComponent()->Play(L"refill");

	TestLevel->AddObject(LAYER::EXTRA_OBJ, pRefillObj);


	ChangeLevel(TestLevel, LEVEL_STATE::STOP);
}
