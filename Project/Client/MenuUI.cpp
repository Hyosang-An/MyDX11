#include "pch.h"
#include "MenuUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/assets.h>
#include <Engine/CGameObject.h>
#include <Engine/CScript.h>
#include <Scripts/CScriptMgr.h>

#include "CLevelSaveLoad.h"
#include "Inspector.h"
#include "CEditorMgr.h"

MenuUI::MenuUI()
{
}

MenuUI::~MenuUI()
{
}

void MenuUI::Init()
{
	m_lastSaveLoadDirectory = CPathMgr::GetInst()->GetContentsPath() + L"level\\";
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
		if (ImGui::MenuItem("Save Level"))
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				IFileSaveDialog* pFileSave;

				// CLSID_FileSaveDialog 클래스의 인스턴스를 생성하고 IFileSaveDialog 인터페이스 포인터를 가져옵니다.
				hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

				if (SUCCEEDED(hr)) {
					// 파일 형식 필터를 설정합니다.
					COMDLG_FILTERSPEC rgSpec[] = {
						{ L"Level Files", L"*.level" },
						{ L"All Files", L"*.*" }
					};
					pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec); // 필터를 대화상자에 설정합니다.
					pFileSave->SetFileTypeIndex(1); // 기본 파일 형식을 설정합니다. rgSpec중 어떤 것을 기본값으로 할지. Index는 1부터.
					pFileSave->SetDefaultExtension(L"level"); // 기본 확장자를 설정합니다.
					pFileSave->SetTitle(L"Save Level File"); // 대화상자의 제목을 설정합니다.

					// 마지막 경로를 초기 폴더로 설정
					IShellItem* psiFolder = nullptr;
					hr = SHCreateItemFromParsingName(m_lastSaveLoadDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
					if (SUCCEEDED(hr) && psiFolder) {
						pFileSave->SetFolder(psiFolder);
						psiFolder->Release();
					}

					// 파일 저장 대화상자를 표시합니다.
					hr = pFileSave->Show(NULL);

					if (SUCCEEDED(hr)) {
						IShellItem* pItem;

						// 사용자가 선택한 파일의 IShellItem을 가져옵니다.
						hr = pFileSave->GetResult(&pItem);
						if (SUCCEEDED(hr)) {
							PWSTR pszFilePath;

							// IShellItem에서 파일 경로를 가져옵니다.
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
							if (SUCCEEDED(hr)) {
								// 여기에서 pszFilePath 경로를 사용하여 파일을 저장합니다.
								CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
								CLevelSaveLoad::SaveLevel(pszFilePath, pCurLevel);

								// 마지막 디렉토리 업데이트
								path filePath = pszFilePath;
								m_lastSaveLoadDirectory = filePath.parent_path().wstring();

								// 파일 경로 사용 후 메모리를 해제합니다.
								CoTaskMemFree(pszFilePath);
							}
							pItem->Release(); // IShellItem 포인터를 해제합니다.
						}
					}
					pFileSave->Release(); // IFileSaveDialog 포인터를 해제합니다.
				}
				CoUninitialize(); // COM 라이브러리를 종료합니다.
			}
		}

		if (ImGui::MenuItem("Load Level"))
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				IFileOpenDialog* pFileOpen;

				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

				if (SUCCEEDED(hr)) {
					COMDLG_FILTERSPEC rgSpec[] = {
						{ L"Level Files", L"*.level" },
						{ L"All Files", L"*.*" }
					};
					pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
					pFileOpen->SetFileTypeIndex(1);
					pFileOpen->SetTitle(L"Load Level File");

					IShellItem* psiFolder = nullptr;
					hr = SHCreateItemFromParsingName(m_lastSaveLoadDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
					if (SUCCEEDED(hr) && psiFolder) {
						pFileOpen->SetFolder(psiFolder);
						psiFolder->Release();
					}

					hr = pFileOpen->Show(NULL);

					if (SUCCEEDED(hr)) {
						IShellItem* pItem;
						hr = pFileOpen->GetResult(&pItem);
						if (SUCCEEDED(hr)) {
							PWSTR pszFilePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
							if (SUCCEEDED(hr)) {

								if (path(pszFilePath).extension().wstring() == L".level")
								{
									CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(pszFilePath);
									ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);

									// Inspector Clear 하기 (이전 오브젝트 정보를 보여주고 있을 수가 있기 때문에)				
									Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
									pInspector->SetTargetObject(nullptr);
									pInspector->SetTargetAsset(nullptr);

									path filePath = pszFilePath;
									m_lastSaveLoadDirectory = filePath.parent_path().wstring();
								}

								CoTaskMemFree(pszFilePath);
							}
							pItem->Release();
						}
					}
					pFileOpen->Release();
				}
				CoUninitialize();
			}
		}

		ImGui::EndMenu();
	}
}

void MenuUI::Level()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	ImGui::BeginDisabled(!pCurLevel);

	if (ImGui::BeginMenu("Level"))
	{
		auto curState = pCurLevel->GetState();

		ImGui::BeginDisabled(LEVEL_STATE::PLAY == curState);
		if (ImGui::MenuItem("Play"))
		{
			if (LEVEL_STATE::STOP == curState)
			{
				wstring strLevelPath = CPathMgr::GetInst()->GetContentsPath();
				strLevelPath += L"level\\Temp.level";
				CLevelSaveLoad::SaveLevel(strLevelPath, pCurLevel);
			}

			ChangeLevelState(LEVEL_STATE::PLAY);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(LEVEL_STATE::PLAY != curState);
		if (ImGui::MenuItem("Pause"))
		{
			ChangeLevelState(LEVEL_STATE::PAUSE);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(LEVEL_STATE::STOP == curState);
		if (ImGui::MenuItem("Stop"))
		{
			wstring StrLevelLoadPath = CPathMgr::GetInst()->GetContentsPath();
			StrLevelLoadPath += L"level\\Temp.level";
			CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(StrLevelLoadPath);
			ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);

			// Inspector Clear 하기 (이전 오브젝트 정보를 보여주고 있을 수가 있기 때문에)				
			Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
			pInspector->SetTargetObject(nullptr);
			pInspector->SetTargetAsset(nullptr);
		}
		ImGui::EndDisabled();

		ImGui::EndMenu();
	}
	ImGui::EndDisabled();
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

		AddScript();

		ImGui::EndMenu();
	}
}

void MenuUI::AddScript()
{
	if (ImGui::BeginMenu("Add Script"))
	{
		vector<wstring> vecScriptsName;
		CScriptMgr::GetScriptInfo(vecScriptsName);

		for (size_t i = 0; i < vecScriptsName.size(); ++i)
		{
			if (ImGui::MenuItem(string(vecScriptsName[i].begin(), vecScriptsName[i].end()).c_str()))
			{
				// 인스펙터
				Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");

				// 타겟 오브젝트 알아냄
				CGameObject* pObject = pInspector->GetTargetObject();

				// 오브젝트에, 선택한 스크립트를 추가해줌
				if (nullptr != pObject)
				{
					// 오브젝트에, 선택한 스크립트를 추가해줌
					CScript* pScript = CScriptMgr::GetScript(vecScriptsName[i]);
					pObject->AddComponent(pScript);
				}
			}
		}

		ImGui::EndMenu();
	}

}

void MenuUI::Assets()
{
	if (ImGui::BeginMenu("Assets"))
	{
		// Create Empty Material
		if (ImGui::MenuItem("Create Empty Material"))
		{
			Ptr<CMaterial> pMtrl = new CMaterial;

			// Key는 저장 상대경로
			wstring relativePathKey = CreateRelativePathAssetKey(ASSET_TYPE::MATERIAL, L"Default Material");
			CAssetMgr::GetInst()->AddAsset<CMaterial>(relativePathKey, pMtrl);
			pMtrl->Save(CPathMgr::GetInst()->GetContentsPath() + relativePathKey);
		}

		// SpriteCreator
		EditorUI* pSpriteEditor = CEditorMgr::GetInst()->FindEditorUI("SpriteCreator");
		bool IsActive = pSpriteEditor->IsActive();

		if (ImGui::MenuItem("Create Sprite", nullptr, &IsActive))
		{
			CEditorMgr::GetInst()->FindEditorUI("SpriteCreator")->SetActive(IsActive);
		}

		// FlipBook Editor
		EditorUI* pFlipBookEditor = CEditorMgr::GetInst()->FindEditorUI("FlipBookEditor");
		IsActive = pFlipBookEditor->IsActive();

		if (ImGui::MenuItem("Edit FlipBook", nullptr, &IsActive))
		{
			CEditorMgr::GetInst()->FindEditorUI("FlipBookEditor")->SetActive(IsActive);
		}

		ImGui::EndMenu();
	}
}

wstring MenuUI::CreateRelativePathAssetKey(ASSET_TYPE _Type, const wstring& _KeyFormat)
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
