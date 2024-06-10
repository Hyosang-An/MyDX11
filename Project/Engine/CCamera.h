#pragma once
#include "CComponent.h"

class CGameObject;

enum PROJ_TYPE
{
    ORTHOGRAPHIC,
    PERSPECTIVE,
};

class CCamera :
    public CComponent
{
public:
    CCamera();
    ~CCamera();
    virtual CCamera* Clone() { return new CCamera(*this); }

private:
    int         m_Priority;
    UINT        m_LayerCheck;   // ���ϴ� ���̸� ī�޶� �������� ����

    PROJ_TYPE   m_ProjType;

    float       m_Width;
    float       m_Height;
    float       m_AspectRatio;  // ��Ⱦ��
    float       m_Far;          // ī�޶� �� �� �ִ� �þ� �Ÿ�

    float       m_FOV; // Field Of View (�þ� ����, �þ� ��)

    Matrix      m_matView;
    Matrix      m_matProj;

    vector<CGameObject*>    m_vecOpaque;        // ������
    vector<CGameObject*>    m_vecMasked;        // ������, ����
    vector<CGameObject*>    m_vecTransparent;   // ����, ������
    vector<CGameObject*>    m_vecParticles;     // ����, ������, ���� Ÿ��

public:
    void SetPriority(int _Priority) { m_Priority = _Priority; }
    void SetLayer(UINT _LayerIdx, bool _bCheck)
    {
        if (_bCheck)
            m_LayerCheck |= (1 << _LayerIdx);
        else
            m_LayerCheck &= ~(1 << _LayerIdx);
    }
    void SetLayerAll() { m_LayerCheck = 0xffffffff; }

    void SetProjType(PROJ_TYPE _type) { m_ProjType = _type; }
    PROJ_TYPE GetProjType() { return m_ProjType; }

    void SetFar(float _Far) { m_Far = _Far; }
    float GetFar() { return m_Far; }

private:
    void SortGameObject();

public:
    virtual void Begin() override;
    virtual void FinalTick() override;
    void Render();

};

