#include "pch.h"
#include "MenuUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>

MenuUI::MenuUI()
{
}

MenuUI::~MenuUI()
{
}

void MenuUI::Tick()
{
	if (!IsActive())
		return;

	if (ImGui::BeginMainMenuBar())
	{

		Update();

		ImGui::EndMainMenuBar();
	}
}


void MenuUI::Update()
{
	File();

	Level();

	GameObject();

	Assets();
}

void MenuUI::File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Level Save"))
		{

		}

		if (ImGui::MenuItem("Level Load"))
		{

		}

		ImGui::EndMenu();
	}
}

void MenuUI::Level()
{
	if (ImGui::BeginMenu("Level"))
	{
		if (ImGui::MenuItem("Play"))
		{

		}


		if (ImGui::MenuItem("Pause"))
		{

		}

		if (ImGui::MenuItem("Stop"))
		{

		}

		ImGui::EndMenu();
	}
}

void MenuUI::GameObject()
{
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Create Empty Object"))
		{

		}

		if (ImGui::BeginMenu("Add Component"))
		{
			ImGui::MenuItem("MeshRender");
			ImGui::MenuItem("Collider2D");
			ImGui::MenuItem("Camera");

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void MenuUI::Assets()
{
	if (ImGui::BeginMenu("Assets"))
	{
		if (ImGui::MenuItem("Create Empty Material"))
		{
			Ptr<CMaterial> pMtrl = new CMaterial;

			// Key는 저장 상대경로
			wstring Key = GetAssetKey(ASSET_TYPE::MATERIAL, L"Default Material");
			CAssetMgr::GetInst()->AddAsset<CMaterial>(Key, pMtrl);
			pMtrl->Save(Key);
		}

		ImGui::EndMenu();
	}
}

wstring MenuUI::GetAssetKey(ASSET_TYPE _Type, const wstring& _KeyFormat)
{
	wstring Key;

	switch (_Type)
	{
		case ASSET_TYPE::MATERIAL:
		{
			Key = wstring(L"material\\") + _KeyFormat + L" %d.mtrl";
		}
		break;
		case ASSET_TYPE::PREFAB:
		{
			Key = wstring(L"prefab\\") + _KeyFormat + L" %d.pref";
		}
		break;
		case ASSET_TYPE::SPRITE:
		{
			Key = wstring(L"sprite\\") + _KeyFormat + L" %d.sprite";
		}
		break;
		case ASSET_TYPE::FLIPBOOK:
		{
			Key = wstring(L"flipbook\\") + _KeyFormat + L" %d.flip";
		}
		break;
		default:
			assert(nullptr);
			break;
	}

	wchar_t szKey[255] = {};
	wstring FilePath = CPathMgr::GetInst()->GetContentsPath();

	for (UINT i = 0; i < 0xffffffff; ++i)
	{
		swprintf_s(szKey, 255, Key.c_str(), i);

		if (false == std::filesystem::exists(FilePath + szKey))
		{
			break;
		}
	}

	return szKey;
}
