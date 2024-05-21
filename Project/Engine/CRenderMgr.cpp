#include "pch.h"
#include "CRenderMgr.h"

#include "CCamera.h"

CRenderMgr::CRenderMgr()
{
}

CRenderMgr::~CRenderMgr()
{
}



void CRenderMgr::Init()
{
}

void CRenderMgr::Tick()
{
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		if (nullptr == m_vecCam[i])
			continue;

		m_vecCam[i]->Render();
	}
}

void CRenderMgr::RegisterCamera(CCamera* _cam, int _camPriority)
{
	// 카메라 우선순위에 따라서 벡터의 공간의 마련되어야 한다.
	if (m_vecCam.size() <= _camPriority + 1)
		m_vecCam.resize(_camPriority + 1);

	// 카메라 우선순위에 맞는 위치에 넣는다
	m_vecCam[_camPriority] = _cam;
}