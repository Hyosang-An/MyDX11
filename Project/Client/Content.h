#pragma once
#include "EditorUI.h"
class Content :
    public EditorUI
{
private:
    class TreeUI* m_Tree;

public:
    virtual void Update() override;

private:
    void RenewContent();

    void AssetClicked(DWORD_PTR _Param);

public:
    Content();
    ~Content();
};

