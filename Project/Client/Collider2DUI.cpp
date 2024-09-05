#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CDevice.h>

#include <Engine/CGameObject.h>
#include <Engine/CCollider2D.h>


Collider2DUI::Collider2DUI()
	: ComponentUI(COMPONENT_TYPE::COLLIDER2D)
{
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();



	Title();

	CCollider2D* pCollider2D = GetTargetObject()->Collider2D();

	if (pCollider2D == nullptr)
		return;

	// Offset Pos
	Vec3 vOffset = pCollider2D->GetOffset();
	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::InputFloat3("##Offset", vOffset);
	pCollider2D->SetOffset(vOffset);

	
	// Offset Scale
	Vec3 vScale = pCollider2D->GetScale();
	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##Scale", vScale);
	pCollider2D->SetScale(vScale);



	// Independent Scale CheckBox
	bool bIndependentScale = pCollider2D->GetIndependentScale();
	ImGui::Text("Independent Scale");
	ImGui::SameLine(100);
	ImGui::Checkbox("##IndependentScale", &bIndependentScale);
	pCollider2D->SetIndependentScale(bIndependentScale);




	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}