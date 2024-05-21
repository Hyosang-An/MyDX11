#include "pch.h"
#include "CComponent.h"

CComponent::CComponent(COMPONENT_TYPE _type) :
	m_Type(_type),
	m_Owner{}
{
}

CComponent::~CComponent()
{
}

void CComponent::Begin()
{
}

void CComponent::Tick()
{
}
