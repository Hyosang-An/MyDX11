#include "pch.h"
#include "Content.h"

#include "TreeUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>

Content::Content()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("ContentTree");
	AddChild(m_Tree);

	// Ʈ�� �ɼ� ����
	m_Tree->ShowRoot(false); // ��Ʈ ������ �ʱ�
	m_Tree->AddClickedDelegate(this, (DELEGATE_1)&Content::AssetClicked);

	// Asset ���¸� Content �� TreeUI �� �ݿ�
	RenewContent();
}

Content::~Content()
{
}

void Content::Update()
{
	// AssetMgr ���� ���� Asset ������ ����ش�.
}

void Content::RenewContent()
{
	// �θ��带 �������� ���� == ��Ʈ��� �Է�
	TreeNode* pRoot = m_Tree->AddNode(nullptr, "Root");

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		TreeNode* pNode = m_Tree->AddNode(pRoot, ToString((ASSET_TYPE)i));
		pNode->SetFrame(true);

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// Asset�� �ּҰ��� Data�� �־��ش�. �̶�, pair�� const & �̹Ƿ�, pair.second.Get() �� const �Լ����� �Ѵ�.
			m_Tree->AddNode(pNode, string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second.Get());
		}
	}
}

void Content::AssetClicked(DWORD_PTR _Param)
{
	// _Param�� SelectedNode�̴�.
	TreeNode* pNode = (TreeNode*)_Param;
	Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
}