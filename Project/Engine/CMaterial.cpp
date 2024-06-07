#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"


CMaterial::CMaterial()
	: CAsset(ASSET_TYPE::MATERIAL)
{

}

CMaterial::~CMaterial()
{

}

void CMaterial::Binding()
{
	if (!m_Shader)
		return;

	for (int i = 0; i < TEX_PARAM::END; ++i)
	{
		if (m_arrTex[i] == nullptr)
		{
			m_Const.btex[i] = 0;
			CTexture::Clear(i);
		}
		else
		{
			m_Const.btex[i] = 1;
			m_arrTex[i]->Binding(i);
		}
	}

	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding();

	m_Shader->Binding();

}