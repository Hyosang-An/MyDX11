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

	// 트리 옵션 세팅
	m_Tree->ShowRoot(false); // 루트 보이지 않기
	m_Tree->AddClickedDelegate(this, (DELEGATE_1)&Content::AssetClicked);

	// Asset 상태를 Content 의 TreeUI 에 반영
	RenewContent();
}

Content::~Content()
{
}

void Content::Update()
{
	// AssetMgr 에서 받은 Asset 정보를 띄워준다.
}

void Content::RenewContent()
{
	// 부모노드를 지정하지 않음 == 루트노드 입력
	TreeNode* pRoot = m_Tree->AddNode(nullptr, "Root");

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		TreeNode* pNode = m_Tree->AddNode(pRoot, ToString((ASSET_TYPE)i));
		pNode->SetFrame(true);

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// Asset의 주소값을 Data로 넣어준다. 이때, pair가 const & 이므로, pair.second.Get() 도 const 함수여야 한다.
			m_Tree->AddNode(pNode, string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second.Get());
		}
	}
}

void Content::AssetClicked(DWORD_PTR _Param)
{
	// _Param은 SelectedNode이다.
	TreeNode* pNode = (TreeNode*)_Param;
	Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
}