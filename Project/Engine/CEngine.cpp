#include "pch.h"
#include "CEngine.h"

#include "CDevice.h"


CEngine::CEngine() :
	m_hWnd{},
	m_ptResolution{}
{
}

CEngine::~CEngine()
{
}

int CEngine::Init(HWND _wnd, POINT _ptResolution)
{
	m_hWnd = _wnd;
	m_ptResolution = _ptResolution;
	ChangeWindowScale(_ptResolution.x, _ptResolution.y);

	if (FAILED(CDevice::GetInst()->Init(m_hWnd, m_ptResolution.x, m_ptResolution.y)))
	{
		MessageBox(nullptr, L"장치 초기화 실패", L"CDevice 초기화 실패", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void CEngine::Progress()
{
}

void CEngine::ChangeWindowScale(UINT _width, UINT _height)
{
	bool bMenu = false;
	if (GetMenu(m_hWnd))
		bMenu = true;

	RECT rt{ 0,0, (int)_width, (int)_height };
	AdjustWindowRect(&rt, WS_OVERLAPPED, bMenu);
	SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
}
