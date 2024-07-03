#pragma once
#include "EditorUI.h"

class ComponentUI :
    public EditorUI
{
// protected�� ���� ����
private:
    class CGameObject*      m_TargetObject = nullptr;
    const COMPONENT_TYPE    m_Type;

public:
    void SetTargetObject(CGameObject* _Object);
    CGameObject* GetTargetObject() { return m_TargetObject; }

protected:
    void Title();

public:
    ComponentUI(COMPONENT_TYPE _Type);
    ~ComponentUI();
};
