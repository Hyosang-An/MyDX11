#pragma once
#include "EditorUI.h"

class ComponentUI :
    public EditorUI
{
// protected�� ���� ����
private:
    class CGameObject* m_TargetObject = nullptr;

public:
    void SetTargetObject(CGameObject* _Object) { m_TargetObject = _Object; }
    CGameObject* GetTargetObject() { return m_TargetObject; }

public:
    ComponentUI();
    ~ComponentUI();
};

