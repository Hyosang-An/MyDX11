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
    UINT        m_LayerCheck;   // 원하는 레이만 카메라에 찍히도록 설정

    PROJ_TYPE   m_ProjType;

    float       m_Width;
    float       m_Height;
    float       m_AspectRatio;  // 종횡비
    float       m_Far;          // 카메라가 볼 수 있는 시야 거리

    float       m_FOV; // Field Of View (시야 범위, 시야 각)

    Matrix      m_matView;
    Matrix      m_matProj;

    vector<CGameObject*>    m_vecOpaque;        // 불투명
    vector<CGameObject*>    m_vecMasked;        // 불투명, 투명
    vector<CGameObject*>    m_vecTransparent;   // 투명, 반투명
    vector<CGameObject*>    m_vecParticles;     // 투명, 반투명, 입자 타입

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

