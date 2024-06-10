#pragma once
#include "CComponent.h"

class CCollider2D :
    public CComponent
{
public:
    CCollider2D();
    ~CCollider2D();

private:
    Vec3    m_Offset;
    Vec3    m_Scale;
    Vec3    m_FinalPos;
    int     m_OverlapCount;

public:
    void SetOffset(Vec3 _Offset) { m_Offset = _Offset; }
    void SetScale(Vec3 _Scale) { m_Scale = _Scale; }

    Vec3 GetOffset() { return m_Offset; }
    Vec3 GetScale() { return m_Scale; }
    Vec3 GetFinalPos() { return m_FinalPos; }
    int GetOverlapCount() { return m_OverlapCount; }

public:
    virtual void BeginOverlap(CCollider2D* _Other);
    virtual void Overlap(CCollider2D* _Other);
    virtual void EndOverlap(CCollider2D* _Other);

public:
    virtual void FinalTick() override;

    virtual CCollider2D* Clone() { return new CCollider2D(*this); }
};

