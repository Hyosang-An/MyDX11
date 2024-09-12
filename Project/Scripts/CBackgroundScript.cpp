#include "pch.h"
#include "CBackgroundScript.h"

CBackgroundScript::CBackgroundScript() :
	CScript((UINT)SCRIPT_TYPE::BACKGROUNDSCRIPT)
{
	// 노출시킬 데이터
	AddScriptParam(SCRIPT_PARAM_TYPE::TEXTURE, "Background Tex", &m_BackgroundTex);
}

CBackgroundScript::CBackgroundScript(const CBackgroundScript& _Other) :
	CScript((UINT)SCRIPT_TYPE::BACKGROUNDSCRIPT)
{
	if (m_BackgroundTex != nullptr)
	{
		Ptr<CMaterial> mtrl = GetRenderComponent()->GetDynamicMaterial();
		mtrl->SetTexParam(TEX_0, m_BackgroundTex);
	}

	m_BackgroundTex = _Other.m_BackgroundTex;

	AddScriptParam(SCRIPT_PARAM_TYPE::TEXTURE, "Background Tex", &m_BackgroundTex);
}

CBackgroundScript::~CBackgroundScript()
{
}

void CBackgroundScript::TrackingCamera(Vec3 _cameraPos)
{

}

void CBackgroundScript::Init()
{
}

void CBackgroundScript::Begin()
{
	Ptr<CMaterial> mtrl = GetRenderComponent()->GetDynamicMaterial();
	mtrl->SetTexParam(TEX_0, m_BackgroundTex);
}

void CBackgroundScript::Tick()
{
}

void CBackgroundScript::SaveToFile(FILE* _File)
{
	SaveAssetRef(m_BackgroundTex, _File);
}

void CBackgroundScript::LoadFromFile(FILE* _File)
{
	LoadAssetRef(m_BackgroundTex, _File);


}
