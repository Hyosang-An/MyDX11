#include "pch.h"
#include "TransformUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>

TransformUI::TransformUI()
	: ComponentUI(COMPONENT_TYPE::TRANSFORM)
{
}

TransformUI::~TransformUI()
{
}

void TransformUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();

	Title();

	CTransform* pTrans = GetTargetObject()->Transform();

	if (pTrans == nullptr)
		return;

	Vec3 vPos = pTrans->GetRelativePos();
	Vec3 vScale = pTrans->GetRelativeScale();
	Vec3 vPrevScale = vScale;
	Vec3 vRot = pTrans->GetRelativeRoatation();
	vRot = (vRot / XM_PI) * 180.f;
	ImGui::Text("Position");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##Pos", vPos);

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##Scale", vScale);

	// scale의 절대값이 0이 되지 않도록
	float fMin = 0.001f;
	for (int i = 0; i < 3; ++i)
	{
		if (abs(vScale[i]) <= fMin)
		{
			if (vPrevScale[i] > 0)
				vScale[i] = fMin;
			else if (vPrevScale[i] < 0)
				vScale[i] = -fMin;
		}

	}

	ImGui::Text("Rotation");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##Rot", vRot, 0.1f);

	pTrans->SetRelativePos(vPos);
	pTrans->SetRelativeScale(vScale);

	vRot = (vRot / 180.f) * XM_PI;
	pTrans->SetRelativeRotation(vRot);


	// Independent Scale
	bool IS = pTrans->IsIndependentScale();

	ImGui::Text("Ignore Parent");
	ImGui::SameLine(100);
	if (ImGui::Checkbox("##TransIS", &IS))
	{
		pTrans->SetIndependentScale(IS);
	}


	// WorldPos
	Vec3 vWorldPos = pTrans->GetWorldPos();
	ImGui::Text("World Position");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##WorldPos", vWorldPos, 1);
	pTrans->SetWorldPos(vWorldPos);

	// WorldScale
	Vec3 vWorldScale = pTrans->GetWorldScale();
	ImGui::Text("World Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat3("##WorldScale", vWorldScale, 1);
	pTrans->SetWorldScale(vWorldScale);



	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}