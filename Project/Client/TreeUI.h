#pragma once
#include "EditorUI.h"

class TreeNode
{
private:
    class TreeUI*       m_Owner;

    string              m_Name;
    TreeNode*           m_ParentNode;
    vector<TreeNode*>   m_vecChildNode;
    const UINT          m_ID;
    DWORD_PTR           m_Data;

    bool                m_Frame;
    bool                m_Selected;


public:
    void SetName(const string& _Name) { m_Name = _Name; }
    const string& GetName() { return m_Name; }

    void SetFrame(bool _Frame) { m_Frame = _Frame; }
    bool IsFrame() { return m_Frame; }

    DWORD_PTR GetData() { return m_Data; }

    void AddChildNode(TreeNode* _Node) { m_vecChildNode.push_back(_Node); }

    void DragCheck();
    void DropCheck();

public:
    void Update();

public:
    TreeNode(UINT _ID);
    ~TreeNode();

    friend class TreeUI;
};


class TreeUI :
    public EditorUI
{
private:
    TreeNode*   m_Root = nullptr;
    TreeNode*   m_ClickedNode = nullptr;
    TreeNode*   m_SelectedNode = nullptr;
    TreeNode*   m_DragedNode = nullptr;
    TreeNode*   m_DroppedNode = nullptr;

    UINT        m_NodeID = 0;
    bool        m_ShowRoot = false;
    bool        m_ShowNameOnly = false;
    bool        m_UseDrag = false;
    bool        m_UseDrop = false;

    EditorUI*   m_SelectedUI = nullptr;
    DELEGATE_1  m_SelectedFunc = nullptr;

    EditorUI*   m_SelfDragDropInst = nullptr;
    DELEGATE_2  m_SelfDragDropFunc = nullptr;

    EditorUI*   m_DropInst = nullptr;
    DELEGATE_2  m_DropFunc = nullptr;
    string      m_DropPayLoadName;


public:
    TreeNode* AddNode(TreeNode* _Parent, const string& _Name, DWORD_PTR _Data = 0);
    void ShowRoot(bool _Show) { m_ShowRoot = _Show; }
    bool IsShowRoot() { return m_ShowRoot; }
    bool IsShowNameOnly() { return m_ShowNameOnly; }
    void ShowNameOnly(bool _NameOnly) { m_ShowNameOnly = _NameOnly; }

    TreeNode* GetClickedNode() { return m_ClickedNode; }

    void SetClickedNode(TreeNode* _node);
    void SetSelectedNode(TreeNode* _Node);
    void SetDragedNode(TreeNode* _Node);
    void SetDroppedNode(TreeNode* _Node);

    void UseDrag(bool _Drag) { m_UseDrag = _Drag; }
    void UseDrop(bool _Drop) { m_UseDrop = _Drop; }

    bool IsDrag() { return m_UseDrag; }
    bool IsDrop() { return m_UseDrop; }

    void SetDropPayLoadName(const string& _Name) { m_DropPayLoadName = _Name; }
    const string GetDropPayLoadName() { return m_DropPayLoadName; }

    void AddSelectedDelegate(EditorUI* _Inst, DELEGATE_1 _Func) { m_SelectedUI = _Inst; m_SelectedFunc = _Func; }
    void AddDragDropDelegate(EditorUI* _Inst, DELEGATE_2 _Func) { m_SelfDragDropInst = _Inst; m_SelfDragDropFunc = _Func; }
    void AddDropDelegate(EditorUI* _Inst, DELEGATE_2 _Func) { m_DropInst = _Inst; m_DropFunc = _Func; }

    void Clear();

public:
    virtual void Update() override;

public:
    TreeUI();
    ~TreeUI();
};

