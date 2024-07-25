#include "pch.h"
#include "SE_AtlasView.h"

#include "SE_Detail.h"


SE_AtlasView::SE_AtlasView()
{
}

SE_AtlasView::~SE_AtlasView()
{
}

void SE_AtlasView::Init()
{
}

void SE_AtlasView::Update()
{
	if (nullptr == m_AtlasTex)
		return;

	WheelCheck();

	// 이미지	
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);

	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

	// 실제 해상도 대비 출력 Image 의 비율
	m_Ratio = (m_WidthSize * m_WheelScale) / m_AtlasTex->Width();

	ImGui::Image(m_AtlasTex->GetSRV().Get(), ImVec2((m_WidthSize * m_WheelScale), m_AtlasTex->Height() * m_Ratio)
		, uv_min, uv_max, tint_col, border_col);

	// SelectCheck
	SelectCheck();

	// 선택한 영역에 사각형 그리기
	DrawSelectRect();
}

void SE_AtlasView::SetAtlasTex(Ptr<CTexture> _Tex)
{
	if (m_AtlasTex == _Tex)
		return;

	m_AtlasTex = _Tex;

	m_WidthSize = m_AtlasTex->Width();
}

void SE_AtlasView::WheelCheck()
{
	if (0 < ImGui::GetIO().MouseWheel)
	{
		m_WheelScale += 0.05f;
	}

	if (0 > ImGui::GetIO().MouseWheel)
	{
		m_WheelScale -= 0.05f;
	}

	if (3.f < m_WheelScale)
		m_WheelScale = 3.f;
	if (m_WheelScale < 0.1f)
		m_WheelScale = 0.1f;
}

void SE_AtlasView::SelectCheck()
{
	// Image 위젯 좌상단 좌표
	m_imageRectMin = ImGui::GetItemRectMin();
	auto imageRectMax = ImGui::GetItemRectMax();
	float ArrImageMin[] = { m_imageRectMin.x, m_imageRectMin.y };
	ImGui::InputFloat2("ImageMin", ArrImageMin);


	// 현재 마우스 위치
	m_MousePos = ImGui::GetMousePos();
	float arrMousePos[] = { m_MousePos.x, m_MousePos.y };
	ImGui::InputFloat2("MousePos", arrMousePos);

	ImVec2 vDiff = ImVec2(m_MousePos.x - m_imageRectMin.x, m_MousePos.y - m_imageRectMin.y);
	vDiff = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);

	// 마우스 위치의 아틀라스 픽셀좌표	
	float PixelPos[] = { vDiff.x, vDiff.y };
	ImGui::InputFloat2("PixelPos", PixelPos);

	if (m_imageRectMin.x <= ImGui::GetMousePos().x && ImGui::GetMousePos().x <= imageRectMax.x &&
		m_imageRectMin.y <= ImGui::GetMousePos().y && ImGui::GetMousePos().y <= imageRectMax.y)
	{


		// 마우스 왼쪽클릭 체크
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_MouseLTonOrignalTex = ImGui::GetMousePos();
			ImVec2 vDiff = ImVec2(m_MouseLTonOrignalTex.x - m_imageRectMin.x, m_MouseLTonOrignalTex.y - m_imageRectMin.y);
			m_MouseLTonOrignalTex = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_MouseRBonOriginalTex = ImGui::GetMousePos();
			ImVec2 vDiff = ImVec2(m_MouseRBonOriginalTex.x - m_imageRectMin.x, m_MouseRBonOriginalTex.y - m_imageRectMin.y);
			m_MouseRBonOriginalTex = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);
		}
	}
}

void SE_AtlasView::DrawSelectRect()
{
	ImVec2 MouseLTPos = ImVec2(m_MouseLTonOrignalTex.x * m_Ratio + m_imageRectMin.x, m_MouseLTonOrignalTex.y * m_Ratio + m_imageRectMin.y);
	ImVec2 MouseRBPos = ImVec2(m_MouseRBonOriginalTex.x * m_Ratio + m_imageRectMin.x, m_MouseRBonOriginalTex.y * m_Ratio + m_imageRectMin.y);

	ImGui::GetWindowDrawList()->AddRect(MouseLTPos, MouseRBPos
		, ImGui::GetColorU32(ImVec4(1.f, 1.f, 0.f, 1.f)), 0.f, 0.f, 1.f);
}