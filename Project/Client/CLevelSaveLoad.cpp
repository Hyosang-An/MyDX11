#include "pch.h"
#include "CLevelSaveLoad.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>

#include <Scripts/CScriptMgr.h>

CLevel* CLevelSaveLoad::pLoadingLevel = nullptr;

void CLevelSaveLoad::SaveLevel(const wstring& _FilePath, CLevel* _Level)
{
	//assert(_Level && _Level->GetState() == LEVEL_STATE::STOP);
	assert(_Level);

	std::filesystem::path dir_path = _FilePath;
	if (!std::filesystem::exists(dir_path.parent_path()))
		std::filesystem::create_directories(dir_path.parent_path());

	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"wb");

	assert(File);

	// Level �̸� ����
	SaveWString(_Level->GetName(), File);

	// Level �ȿ� �ִ� Layer �� ����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = _Level->GetLayer(i);

		// Layer �̸� ����
		SaveWString(pLayer->GetName(), File);

		// Layer �ȿ� �ִ� GameObject �� ����
		const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();

		// Layer �� ������ ParentObject ���� ����
		size_t count = vecParents.size();
		fwrite(&count, sizeof(size_t), 1, File);

		// GameObject ����
		for (size_t i = 0; i < vecParents.size(); ++i)
		{
			SaveGameObject(File, vecParents[i]);
		}
	}

	fclose(File);
}

void CLevelSaveLoad::SaveGameObject(FILE* _File, CGameObject* _Object)
{
	// GameObject �� �̸� ����
	SaveWString(_Object->GetName(), _File);

	// layerIdx ����
	int LayerIdx = _Object->GetLayerIdx();
	fwrite(&LayerIdx, sizeof(int), 1, _File);

	// Component ���� ����
	UINT i = 0;
	for (; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pComponent = _Object->GetComponent((COMPONENT_TYPE)i);

		if (nullptr == pComponent)
			continue;

		// ComponentType �� ����
		COMPONENT_TYPE Type = pComponent->GetComponentType();
		fwrite(&Type, sizeof(COMPONENT_TYPE), 1, _File);

		pComponent->SaveToFile(_File);
	}

	// COMPONENT_TYPE::END ����
	fwrite(&i, sizeof(COMPONENT_TYPE), 1, _File);

	// Script ���� ����
	// Script ���� ����
	const vector<CScript*> vecScripts = _Object->GetScriptVec();
	size_t ScriptCount = vecScripts.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ScriptCount; ++i)
	{
		// Script �� �̸� ����
		wstring ScriptName = CScriptMgr::GetScriptName(vecScripts[i]);
		SaveWString(ScriptName, _File);

		// Script ���� ����
		vecScripts[i]->SaveToFile(_File);
	}

	// Child ���� ����
	const vector<CGameObject*>& vecChild = _Object->GetChildren();
	size_t ChildCount = vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		SaveGameObject(_File, vecChild[i]);
	}
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& _FilePath)
{
	FILE* File = nullptr;

	errno_t err = _wfopen_s(&File, _FilePath.c_str(), L"rb");

	if (err != 0 || File == nullptr)
	{
		// ������ ���� ���� ��쿡 ���� ���� ó��
		MessageBox(nullptr, L"Level���� �ε� ����", L"Level���� �ε� ����", MB_OK);
		assert(false);
		return nullptr;
	}

	// Level ����
	pLoadingLevel = new CLevel;

	// Level �̸� �ҷ�����
	wstring LevelName;
	LoadWString(LevelName, File);
	pLoadingLevel->SetName(LevelName);

	// Level �ȿ� �ִ� Layer ���� �ҷ�����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pLoadingLevel->GetLayer(i);

		// Layer �̸� �ҷ�����
		wstring LayerName;
		LoadWString(LayerName, File);
		pLayer->SetName(LayerName);

		// Layer �� ������ ParentObject ���� �ҷ�����
		size_t count = 0;
		fread(&count, sizeof(size_t), 1, File);

		// GameObject �ҷ��ͼ� Layer �� ����ֱ�
		for (size_t i = 0; i < count; ++i)
		{
			int layerIdx = -1;
			CGameObject* pLoadedObject = LoadGameObject(File, layerIdx);
			pLayer->AddObject(pLoadedObject, false);
		}
	}

	fclose(File);

	return pLoadingLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _File, int& _objIdx)
{
	CGameObject* pObject = new CGameObject;

	// GameObject �� �̸� �ε�
	wstring Name;
	LoadWString(Name, _File);
	pObject->SetName(Name);

	// LayerIdx �ε�
	int layerIdx = -1;
	fread(&layerIdx, sizeof(int), 1, _File);
	_objIdx = layerIdx;

	// Component ���� �ε�
	COMPONENT_TYPE Type = COMPONENT_TYPE::END;
	while (true)
	{
		// ����Ǿ��ִ� ������ � ������Ʈ���� Ȯ��
		fread(&Type, sizeof(COMPONENT_TYPE), 1, _File);

		// ���� Ÿ�� ������ END �� ���, ����� ������Ʈ ������ ���̴�.
		if (COMPONENT_TYPE::END == Type)
			break;

		// ����� Ÿ�Կ� �´� ������Ʈ�� ���� ��Ű��, �����Ҷ��� ������ ������ �����͸� �д´�.
		CComponent* pComponent = GetComponent(Type);

		// ������Ʈ�� LoadFromFile�� �� ��, Onwer ������ �ʿ��� ��찡 �ִ�.
		pComponent->SetOwner(pObject);
		// �������� ������ �о�ͼ� �����Ѵ�.
		pComponent->LoadFromFile(_File);

		// ������ ������Ʈ�� ������Ʈ�� �־��ش�. �־��ָ鼭 component init ����
		pObject->AddComponent(pComponent);
	}

	// Script ���� �ε�	
	size_t ScriptCount = 0;
	fread(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ScriptCount; ++i)
	{
		// Script �� �̸� �б�
		wstring ScriptName;
		LoadWString(ScriptName, _File);

		// ���� Script �̸����� �̸��� �ش��ϴ� Script ����
		CScript* pScript = CScriptMgr::GetScript(ScriptName);
		pScript->LoadFromFile(_File);

		pObject->AddComponent(pScript);
	}

	// Child ���� �ε�
	size_t ChildCount = 0;
	fread(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ChildCount; ++i)
	{
		int layerIdx = -1;
		CGameObject* pChildObject = LoadGameObject(_File, layerIdx);
		
		pObject->AddChild(pChildObject, layerIdx);
	}

	return pObject;
}

CComponent* CLevelSaveLoad::GetComponent(COMPONENT_TYPE _Type)
{
	switch (_Type)
	{
		case COMPONENT_TYPE::TRANSFORM:
			return new CTransform;

		case COMPONENT_TYPE::COLLIDER2D:
			return new CCollider2D;

		case COMPONENT_TYPE::COLLIDER3D:

			break;
		case COMPONENT_TYPE::LIGHT2D:
			return  new CLight2D;

		case COMPONENT_TYPE::LIGHT3D:
			break;

		case COMPONENT_TYPE::FLIPBOOK_COMPONENT:
			return new CFlipBookComponent;

		case COMPONENT_TYPE::ANIMATOR3D:
			break;

		case COMPONENT_TYPE::STATE_MACHINE:
			break;

		case COMPONENT_TYPE::RIGIDBODY:
			break;

		case COMPONENT_TYPE::CAMERA:
			return  new CCamera;

		case COMPONENT_TYPE::MESHRENDER:
			return  new CMeshRender;

		case COMPONENT_TYPE::TILEMAP:
			return  new CTileMap;

		case COMPONENT_TYPE::PARTICLE_SYSTEM:
			return new CMyParticleSystem;

		case COMPONENT_TYPE::DECAl:
			break;

		case COMPONENT_TYPE::SKYBOX:
			break;

		case COMPONENT_TYPE::LANDSCAPE:
			break;
	}

	return nullptr;
}

void CLevelSaveLoad::LoadInitialGameLevel()
{
	// Load Initial Game Level
	wstring _levelFilePath = L"";
	CLevel* pLoadedLevel = LoadLevel(_levelFilePath);
	ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);
}
