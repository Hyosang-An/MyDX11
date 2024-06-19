#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"

CLevel::CLevel() :
	m_Layer{}
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i] = new CLayer(i);
	}
}

CLevel::~CLevel()
{
	Delete_Array(m_Layer);
}

void CLevel::Begin()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->Begin();
	}
}

void CLevel::Tick()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->Tick();
	}
}

void CLevel::FinalTick()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->FinalTick();
	}
}

void CLevel::AddObject(int LayerIdx, CGameObject* _Object, bool _bMoveChild)
{
	m_Layer[LayerIdx]->AddObject(_Object, _bMoveChild);
}

void CLevel::ClearObject()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->ClearObject();
	}
}

void CLevel::ChangeState(LEVEL_STATE _NextState)
{
	if (m_State == _NextState)
		return;

	// Stop -> Pause (X)
	if (STOP == m_State && PAUSE == _NextState)
		return;

	// Stop -> Play (정지 상태의 레벨이 시작되면, 레벨에 있던 물체들은 Begin 이 호출되어야 한다.)
	if (STOP == m_State && PLAY == _NextState)
	{
		Begin();
	}

	m_State = _NextState;

	// Play -> Stop (최초 레벨이 시작되던 시점으로 복구가 가능해야 한다.)
}