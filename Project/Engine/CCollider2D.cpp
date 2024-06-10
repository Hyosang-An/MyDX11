#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"


CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_OverlapCount(0)
{
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::FinalTick()
{
	m_FinalPos = Transform()->GetRelativePos() + m_Offset;


}

void CCollider2D::BeginOverlap(CCollider2D* _Other)
{
}

void CCollider2D::Overlap(CCollider2D* _Other)
{
}

void CCollider2D::EndOverlap(CCollider2D* _Other)
{
}