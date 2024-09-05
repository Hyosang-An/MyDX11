#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CDevice.h>

#include <Engine/CGameObject.h>
#include <Engine/CCollider2D.h>
#include <Engine/CCollisionMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>


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




	// �浹 üũ �ڽ�
	if (ImGui::CollapsingHeader("Check Collision layer", ImGuiTreeNodeFlags_None))
	{
		const UINT* iCollisionCheckArr = CCollisionMgr::GetInst()->GetCollisionArr();
		int maxLayer = 0;

		for (int i = 0; i < MAX_LAYER; ++i)
		{
			wstring wstrLayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(i)->GetName();
			string strLayer = string(wstrLayer.begin(), wstrLayer.end());
			maxLayer = i - 1;

			// �̸��� ���� ���̾�� ����
			if (wstrLayer == L"NO_NAME")
				break;
		}

		// ���ڸ� �ùٸ� ��ġ�� ����մϴ�.
		for (int i = maxLayer; i >= 0; --i)
		{
			// �� ���ڸ� ������ �������� ��ġ��ŵ�ϴ�. (üũ�ڽ� ������ 30)
			ImGui::SetCursorPosX(122.0f + (maxLayer - i) * 30); // ���ڰ� ������ X ��ġ�� ����
			ImGui::Text((std::to_string(i)).c_str());
			ImGui::SameLine();  // ���� ���ڸ� ���� �ٿ� ǥ��
		}


		for (size_t i = 0; i <= maxLayer; ++i)
		{
			wstring wstrLayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(i)->GetName();
			string strLayer = std::to_string(i) + " : " + string(wstrLayer.begin(), wstrLayer.end());

			// �ش� ���̾��� �̸��� ������ üũ �ڽ��� �������� �ʴ´�
			if (wstrLayer.empty())
				break;

			ImGui::NewLine();
			ImGui::Text(strLayer.c_str());
			ImGui::SameLine(120);

			for (int j = 0; j <= maxLayer - i; ++j)
			{
				// ����� (üũ�ڽ� �� ����Ȯ��)
				// auto cursorPos = ImGui::GetCursorPos();

				bool bCollision = iCollisionCheckArr[i] & (1 << maxLayer - j);

				const string name = "##layer_" + std::to_string(i) + "_" + std::to_string(maxLayer - j);

				if (ImGui::Checkbox(name.c_str(), &bCollision))
				{
					CCollisionMgr::GetInst()->CollisionCheck(i, maxLayer - j);
				}
				ImGui::SameLine();
			}
		}
	}




	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(ImVec2(0, content_size.y));
}