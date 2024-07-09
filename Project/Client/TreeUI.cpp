#include "pch.h"
#include "TreeUI.h"

// ========
// TreeNode
// ========
TreeNode::TreeNode(UINT _ID)
	: m_Owner(nullptr)
	, m_ParentNode(nullptr)
	, m_ID(_ID)
	, m_Data(0)
	, m_Frame(false)
	, m_Selected(false)
{
}

TreeNode::~TreeNode()
{
	Delete_Vec(m_vecChildNode);
}

void TreeNode::Update()
{
	UINT Flag = ImGuiTreeNodeFlags_OpenOnDoubleClick
			  | ImGuiTreeNodeFlags_OpenOnArrow
			  | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (m_Frame)
		Flag |= ImGuiTreeNodeFlags_Framed;

	if (m_Selected)
		Flag |= ImGuiTreeNodeFlags_Selected;

	//char Name[255] = {};
	string strName;

	if (m_vecChildNode.empty())
	{
		Flag |= ImGuiTreeNodeFlags_Leaf;
		//sprintf_s(Name, 255, "   %s##%d", m_Name.c_str(), m_ID);

		if (m_Frame)
			strName = "   " + m_Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
		else
			strName = m_Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
	}
	else
	{
		//sprintf_s(Name, 255, "%s##%d", m_Name.c_str(), m_ID);

		strName = m_Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
	}


	// NameOnly (폴더 경로 및 확장자를 뺀 파일 이름만 노출)
	if (m_Owner->IsShowNameOnly())
	{
		path Path = strName;
		strName = Path.stem().string();
	}

	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.4f, 1.0f)); // Gray color for normal state
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color on hover
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));  // Green color on active

	if (ImGui::TreeNodeEx(strName.c_str(), Flag))
	{
		// 해당 노드가 클릭된 경우 (왼쪽 버튼이 눌린 순간)
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_Owner->SetClickedNode(this);
		}

		// 해당 노드가 클릭되어있고 왼쪽 버튼이 떼진 순간
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			if (this == m_Owner->GetClickedNode())
				m_Owner->SetSelectedNode(this);
			else
				m_Owner->SetClickedNode(nullptr);
		}

		// Drag 체크	
		DragCheck();

		// Drop 체크
		DropCheck();

		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->Update();
		}

		ImGui::TreePop();
	}

	ImGui::PopStyleColor(3);
}

void TreeNode::DragCheck()
{
	if (m_Owner->IsDrag())
	{
		if (ImGui::BeginDragDropSource())
		{
			TreeNode* pThis = this;

			// 해당 트리 노드의 주소값을 전달
			ImGui::SetDragDropPayload(m_Owner->GetName().c_str(), &pThis, sizeof(TreeNode*));
			ImGui::Text(m_Name.c_str());
			ImGui::EndDragDropSource();

			m_Owner->SetDragedNode(this);
		}
	}
}


void TreeNode::DropCheck()
{
	if (!m_Owner->IsDrop())
		return;

	if (ImGui::BeginDragDropTarget())
	{
		m_Owner->SetDroppedNode(this);

		ImGui::EndDragDropTarget();
	}
}


// ======================================================================================================
// TreeUI
// ======================================================================================================
TreeUI::TreeUI()
{
}

TreeUI::~TreeUI()
{
	Clear();
}

void TreeUI::Update()
{
	if (nullptr == m_Root)
		return;

	if (m_ShowRoot)
		m_Root->Update();
	else
	{
		for (size_t i = 0; i < m_Root->m_vecChildNode.size(); ++i)
		{
			m_Root->m_vecChildNode[i]->Update();
		}
	}

	// 마우스 왼쪽 버튼을 뗏을 때, 선택된 m_DroppedNode, m_DragedNode 해제
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		m_DroppedNode = m_DragedNode = nullptr;
	}
}

TreeNode* TreeUI::AddNode(TreeNode* _ParentNode, const string& _Name, DWORD_PTR _Data)
{
	// _Data 기본값 0
	
	// 노드 생성 및 이름, ID 세팅
	TreeNode* pNode = new TreeNode(m_NodeID++);
	pNode->m_Owner = this;
	pNode->SetName(_Name);
	pNode->m_Data = _Data;

	// 부모가 지정되지 않으면 노드를 루트로 삼겠다.
	if (nullptr == _ParentNode)
	{
		assert(!m_Root);

		m_Root = pNode;
	}
	else
	{
		_ParentNode->AddChildNode(pNode);
	}

	return pNode;
}

void TreeUI::SetClickedNode(TreeNode* _node)
{
	m_ClickedNode = _node;
}

void TreeUI::SetSelectedNode(TreeNode* _Node)
{
	// 기존에 선택상태였던 노드를 선택상태 해제하고
	if (nullptr != m_SelectedNode)
		m_SelectedNode->m_Selected = false;

	// 새로운 노드를 선택된 노드로 갱신
	m_SelectedNode = _Node;

	// 새로운 노드를 선택된 상태로 만들어줌
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_Selected = true;

		if (m_SelectedUI && m_SelectedFunc)
		{
			(m_SelectedUI->*m_SelectedFunc)((DWORD_PTR)m_SelectedNode);
		}
	}
}

void TreeUI::SetDragedNode(TreeNode* _Node)
{
	m_DragedNode = _Node;
}

void TreeUI::SetDroppedNode(TreeNode* _Node)
{
	// 현재 트리에서 Drag 된 노드가 없는 경우 ( 외부 데이터가 트리로 드랍된 경우 )
	if (nullptr == m_DragedNode)
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_DropPayLoadName.c_str());
		if (payload)
		{
			m_DroppedNode = _Node;

			if (m_DropInst && m_DropFunc)
				(m_DropInst->*m_DropFunc)((DWORD_PTR)payload->Data, (DWORD_PTR)m_DroppedNode);
		}
	}

	// Self Drag Drop 된 상황
	else
	{
		assert(m_DragedNode->m_Owner == this);

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetName().c_str());
		if (payload)
		{
			m_DroppedNode = _Node;

			if (m_SelfDragDropInst && m_SelfDragDropFunc)
				(m_SelfDragDropInst->*m_SelfDragDropFunc)((DWORD_PTR)m_DragedNode, (DWORD_PTR)m_DroppedNode);
		}
	}
}

void TreeUI::Clear()
{
	if (nullptr != m_Root)
	{
		delete m_Root;
		m_Root = nullptr;
	}
}
