#pragma once

// DirectX 관련 기능을 담당
// GPU 제어

class CConstBuffer;

class CDevice :
	public CSingleton<CDevice>
{
private:
	friend class CSingleton<CDevice>;
	CDevice();
	~CDevice();

private:
	HWND								m_hWnd;
	Vec2								m_vResolution;

	ComPtr<ID3D11Device>				m_Device;		// DX11 객체 생성, GPU 메모리 할당
	ComPtr<ID3D11DeviceContext>			m_Context;	// Rendering 관련

	ComPtr<IDXGISwapChain>				m_SwapChain;

	ComPtr<ID3D11Texture2D>				m_RTTex;
	ComPtr<ID3D11Texture2D>				m_DSTex;

	ComPtr<ID3D11RenderTargetView>		m_RTView;
	ComPtr<ID3D11DepthStencilView>		m_DSView;

	ComPtr<ID3D11BlendState>			m_BSState;
	ComPtr <ID3D11DepthStencilState>	m_DSState;
	ComPtr<ID3D11SamplerState>			m_Sampler;
	ComPtr<ID3D11RasterizerState>		m_RSState;

	CConstBuffer*						m_arrCB[(UINT)CB_TYPE::END];

public:
	int Init(HWND _hWnd, UINT _width, UINT _height);
	void Clear();
	void Present() { m_SwapChain->Present(0, 0); }
	Vec2 GetResolution() { return m_vResolution; }

private:
	int CreateSwapChain();
	int CreateView();
	int CreateConstBuffer();

public:
	ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetContext() { return m_Context.Get(); }

	CConstBuffer* GetConstBuffer(CB_TYPE _type) { return m_arrCB[(UINT)_type]; }
};

