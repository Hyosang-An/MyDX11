#include "pch.h"
#include "MaterialUI.h"

MaterialUI::MaterialUI()
	: AssetUI(ASSET_TYPE::MATERIAL)
{
}

MaterialUI::~MaterialUI()
{
}


void MaterialUI::Update()
{
	Title();

	// 재질 이름
	OutputAssetName();

	Ptr<CMaterial> pMtrl = (CMaterial*)GetAsset().Get();

	// 참조하는 쉐이더 이름
	Ptr<CGraphicShader> pShader = pMtrl->GetShader();

	string Name;
	if (nullptr == pShader)
		Name = "None";
	else
		Name = string(pShader->GetKey().begin(), pShader->GetKey().end());

	// 쉐이더 이름
	ImGui::Text("Name");
	ImGui::SameLine(100);
	ImGui::InputText("##ShaderName", (char*)Name.c_str(), Name.length(), ImGuiInputTextFlags_ReadOnly);


	// Shader Parameter


}