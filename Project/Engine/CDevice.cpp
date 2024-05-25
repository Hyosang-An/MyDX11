#include "pch.h"
#include "CDevice.h"

#include "CConstBuffer.h"
#include "CAssetMgr.h"

CDevice::CDevice() :
	m_hWnd{},
	m_arrCB{},
	m_Sampler{}
{
}

CDevice::~CDevice()
{
	for (size_t i = 0; i < (size_t)CB_TYPE::END; i++)
	{
		if (m_arrCB[i] != nullptr)
			delete m_arrCB[i];
	}
}

int CDevice::Init(HWND _hWnd, UINT _width, UINT _height)
{
	// 윈도우 핸들 및 해상도 값 저장
	m_hWnd = _hWnd;

	m_vResolution.x = (float)_width;
	m_vResolution.y = (float)_height;

	// Device, Context 생성
	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		Flag, nullptr, 0, D3D11_SDK_VERSION, m_Device.GetAddressOf(), nullptr, m_Context.GetAddressOf())))
	{
		MessageBox(nullptr, L"Device, Context 생성 실패", L"장치초기화 실패", MB_OK);
		return E_FAIL;
	}

	// 스왑체인
	if (FAILED(CreateSwapChain()))
	{
		MessageBox(nullptr, L"SwapChain 생성 실패", L"장치초기화 실패", MB_OK);
		return E_FAIL;
	}

	// RenderTargetView, DepthStencilView
	if (FAILED(CreateView()))
	{
		MessageBox(nullptr, L"View 생성 실패", L"장치초기화 실패", MB_OK);
		return E_FAIL;
	}

	// Output Merge State (출력 병합 단계)
	m_Context->OMSetRenderTargets(1, m_RTTex->GetRTV().GetAddressOf(), m_DSTex->GetDSV().Get());

	// ViewPort 설정
   // 출력시킬 화면 윈도우 영역을 설정
	D3D11_VIEWPORT viewport = {};

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_vResolution.x;
	viewport.Height = m_vResolution.y;

	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.f;

	m_Context->RSSetViewports(1, &viewport);

	if (FAILED(CreateConstBuffer()))
	{
		MessageBox(nullptr, L"상수버퍼 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateRasterizerState()))
	{
		MessageBox(nullptr, L"레스터라이저 스테이트 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateSamplerState()))
	{
		MessageBox(nullptr, L"샘플러 스테이트 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void CDevice::Clear()
{
	float color[4] = { 0.4f, 0.4f, 0.4f, 1.f };
	m_Context->ClearRenderTargetView(m_RTTex->GetRTV().Get(), color);
	m_Context->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

int CDevice::CreateSwapChain()
{
	// SwapChain : 화면에 그림을 그리기위한 버퍼를 관리하고,
	//             버퍼에 그려진 이미지를 화면 출력을 담당하는 객체
	DXGI_SWAP_CHAIN_DESC Desc = {};

	Desc.BufferCount = 1; // 백버퍼 개수
	Desc.BufferDesc.Width = (UINT)m_vResolution.x; // 백버퍼 해상도 
	Desc.BufferDesc.Height = (UINT)m_vResolution.y;// 백버퍼 해상도
	Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 픽셀 포맷
	Desc.BufferDesc.RefreshRate.Denominator = 60;
	Desc.BufferDesc.RefreshRate.Numerator = 1;
	Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.Windowed = true;  // 창모드 설정
	Desc.OutputWindow = m_hWnd;// SwapChain 버퍼의 이미지를 출력시킬 윈도우 핸들
	Desc.Flags = 0;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	ComPtr<IDXGIDevice>     Device = nullptr;
	ComPtr<IDXGIAdapter>    Adapter = nullptr;
	ComPtr<IDXGIFactory>    Factory = nullptr;

	if (FAILED(m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)Device.GetAddressOf())))
		return E_FAIL;

	if (FAILED(Device->GetParent(__uuidof(IDXGIAdapter), (void**)Adapter.GetAddressOf())))
		return E_FAIL;

	if (FAILED(Adapter->GetParent(__uuidof(IDXGIFactory), (void**)Factory.GetAddressOf())))
		return E_FAIL;

	if (FAILED(Factory->CreateSwapChain(m_Device.Get(), &Desc, m_SwapChain.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateView()
{
	// =======================================================
	// RenderTarget Texture, DepthStencil Texture 를 생성시킨다
	// =======================================================
	// 스왚체인의 백버퍼의 주소를 받아온다.
	ComPtr<ID3D11Texture2D> RenderTargetTex;

	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)RenderTargetTex.GetAddressOf()); 
	// (void**)로 강제 형변환을 하도록 하여 실수로 다른 이중 포인터 변수가 들어가는것을 방지

	m_RTTex = CAssetMgr::GetInst()->CreateTexture(L"RenderTargetTex", RenderTargetTex);

	// DepthStencil 텍스쳐 생성
	m_DSTex = CAssetMgr::GetInst()->CreateTexture(L"DepthStencilTex"
		, (UINT)m_vResolution.x, (UINT)m_vResolution.y
		, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	return S_OK;
}

int CDevice::CreateConstBuffer()
{
	CConstBuffer* pCB = nullptr;

	// 상수버퍼 생성
	pCB = new CConstBuffer;
	if (FAILED(pCB->Create(CB_TYPE::TRANSFORM, sizeof(tTransform))))
	{
		MessageBox(nullptr, L"상수버퍼 생성 실패", L"초기화 실패", MB_OK);
		return E_FAIL;
	}
	m_arrCB[(UINT)CB_TYPE::TRANSFORM] = pCB;

	return S_OK;
}

int CDevice::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC Desc = {};

	// Cull Back (기본값)
	m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	// Cull Front 
	Desc.CullMode = D3D11_CULL_FRONT;
	Desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

	// Cull None
	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	// Wire Frame
	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_WIREFRAME;
	DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

	return S_OK;
}

int CDevice::CreateSamplerState()
{
	D3D11_SAMPLER_DESC Desc = {};

	Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.Filter = D3D11_FILTER_ANISOTROPIC; // 이방성 필터링

	if (FAILED(DEVICE->CreateSamplerState(&Desc, m_Sampler[0].GetAddressOf())))
	{
		return E_FAIL;
	}

	Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 포인트 필터링

	if (FAILED(DEVICE->CreateSamplerState(&Desc, m_Sampler[1].GetAddressOf())))
	{
		return E_FAIL;
	}

	CONTEXT->VSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->HSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->DSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->GSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->PSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->CSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());

	CONTEXT->VSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->HSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->DSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->GSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->PSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->CSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());

	return S_OK;
}