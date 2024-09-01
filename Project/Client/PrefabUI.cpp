#include "pch.h"
#include "PrefabUI.h"
#include "CEditorMgr.h"

#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>



PrefabUI::PrefabUI()
	: AssetUI(ASSET_TYPE::PREFAB)
{
}

PrefabUI::~PrefabUI()
{
}

void PrefabUI::Activate()
{
	Ptr<CPrefab> pPrefab = (CPrefab*)GetAsset().Get();
	int originalLayerIdx = pPrefab->GetOriginalLayerIdx();
	if (originalLayerIdx != -1)
		m_spawnLayer = (LAYER)originalLayerIdx;
}

void PrefabUI::Update()
{
	Title();

	OutputAssetName();

	Ptr<CPrefab> pPrefab = (CPrefab*)GetAsset().Get();

	// layer index combo box
    
	if (ImGui::BeginCombo("##LayerCombo", LAYER_STRING[(int)m_spawnLayer]))
	{
		for (int i = 0; i < (int)LAYER::END; ++i)
		{
			if (ImGui::Selectable(LAYER_STRING[i]))
			{
				m_spawnLayer = (LAYER)i;
			}
		}

		ImGui::EndCombo();
	}

	// Instantiate 버튼
	if (ImGui::Button("Instantiate"))
	{
		CGameObject* pObj = pPrefab->Instantiate();
		// 에디터 카메라 좌표에 생성
		Vec3 vPos = CEditorMgr::GetInst()->GetEditorCamera()->Transform()->GetWorldPos();
		pObj->Transform()->SetRelativePos(vPos);

		// 현재 레벨의 모든 오브젝트 벡터
		CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
		
		if (!pObj->GetName().empty())
		{
			while (true)
			{
				bool bNameExist = false;
				for (int i = 0; i < MAX_LAYER; ++i)
				{
					CLayer* pLayer = pCurLevel->GetLayer(i);
					// 레이어 내에 같은 이름의 오브젝트가 있다면
					if (pLayer->FindObject(pObj->GetName()) != nullptr)
					{
						// 이름 뒤에 숫자를 붙여준다.
						pObj->SetName(incrementTrailingNumber(pObj->GetName()));

						bNameExist = true;
					}
				}

				if (!bNameExist)
				{
					break;
				}
			}
		}
		else
			pObj->SetName(L"NO_NAME");

		SpawnObject(pObj, (int)m_spawnLayer);
	}
}