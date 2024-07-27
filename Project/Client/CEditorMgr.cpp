#include "pch.h"
#include "CEditorMgr.h"

#include "CGameObjectEx.h"
#include "CEditorCameraScript.h"
#include "EditorUI.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CRenderMgr.h>

#include <Engine/components.h>
#include <Engine/CEngine.h>
#include <Engine/CDevice.h>
#include <Engine/CKeyMgr.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

CEditorMgr::CEditorMgr()
{

}

CEditorMgr::~CEditorMgr()
{
	Delete_Vec(m_vecEditorObject);
	Delete_Map(m_mapUI);

	// ImGui Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CEditorMgr::Init()
{
	CreateEditorObject();

	InitImGui();
}


void CEditorMgr::Progress()
{
	ShortCut();

	EditorObjectProgress();

	ImGuiProgress();
}


void CEditorMgr::ShortCut()
{
	if (KEY_JUST_PRESSED(KEY::I))
	{
		EditorUI* pUI = FindEditorUI("Inspector");

		if (pUI->IsActive())
			pUI->SetActive(false);
		else
			pUI->SetActive(true);
	}
}


void CEditorMgr::EditorObjectProgress()
{
	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->FinalTick();
	}
}

void CEditorMgr::ImGuiTick()
{
	ImGui::ShowDemoWindow();

	for (const auto& pair : m_mapUI)
	{
		pair.second->Tick();
	}
}