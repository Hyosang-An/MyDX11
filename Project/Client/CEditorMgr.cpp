#include "pch.h"
#include "CEditorMgr.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CRenderMgr.h>

#include "CGameObjectEx.h"
#include <Engine/components.h>
#include "CEditorCameraScript.h"

#include <Engine/CEngine.h>
#include <Engine/CDevice.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

CEditorMgr::CEditorMgr()
{

}

CEditorMgr::~CEditorMgr()
{
	Delete_Vec(m_vecEditorObject);

	// ImGui Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CEditorMgr::Init()
{
	InitImGui();

	CreateEditorObject();
}

void CEditorMgr::Tick()
{
	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->FinalTick();
	}

	ImGuiProgress();
}

void CEditorMgr::ImGuiProgress()
{
	// Start ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui Tick
	ImGuiTick();

	// ImGui Render    
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CEditorMgr::ImGuiTick()
{
	//ImGui::ShowDemoWindow();

	static bool b = false;
	if (KEY_JUST_PRESSED(KEY::B))
	{
		b = !b;
	}

	if (b)
	{
		ImGui::Begin("MyTestWindow1");
		ImGui::Button("TestBtn");
		ImGui::End();

		ImGui::Begin("MyTestWindow2");
		ImGui::Button("TestBtn1");
		ImGui::End();

		ImGui::Begin("MyTestWindow3");
		ImGui::Button("TestBtn2");
		ImGui::End();
	}
}

void CEditorMgr::InitImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(CEngine::GetInst()->GetMainWnd());
	ImGui_ImplDX11_Init(DEVICE, CONTEXT);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);
}


void CEditorMgr::CreateEditorObject()
{
	CGameObject* pEditorCamera = new CGameObjectEx;
	pEditorCamera->SetName(L"EditorCamera");
	pEditorCamera->AddComponent(new CTransform);
	pEditorCamera->AddComponent(new CCamera);
	pEditorCamera->AddComponent(new CEditorCameraScript);

	pEditorCamera->Camera()->SetLayerAll();
	pEditorCamera->Camera()->SetLayer(31, false);
	pEditorCamera->Camera()->SetFar(100000.f);
	pEditorCamera->Camera()->SetProjType(ORTHOGRAPHIC);

	m_vecEditorObject.push_back(pEditorCamera);

	CRenderMgr::GetInst()->RegisterEditorCamera(pEditorCamera->Camera());
}
