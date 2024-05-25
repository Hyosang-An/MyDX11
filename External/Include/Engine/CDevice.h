#pragma once

// DirectX ���� ����� ���
// GPU ����

class CConstBuffer;

#include "CTexture.h"

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

	ComPtr<ID3D11Device>				m_Device;		// DX11 ��ü ����, GPU �޸� �Ҵ�
	ComPtr<ID3D11DeviceContext>			m_Context;	// Rendering ����

	ComPtr<IDXGISwapChain>				m_SwapChain;

	Ptr<CTexture>						m_RTTex;
	Ptr<CTexture>						m_DSTex;

	ComPtr<ID3D11RasterizerState>		m_RSState[(UINT)RS_TYPE::END];
	ComPtr<ID3D11SamplerState>			m_Sampler[2];

	ComPtr<ID3D11BlendState>			m_BSState;
	ComPtr <ID3D11DepthStencilState>	m_DSState;

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
	int CreateRasterizerState();
	int CreateSamplerState();

public:
	ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetContext() { return m_Context.Get(); }

	CConstBuffer* GetConstBuffer(CB_TYPE _type) { return m_arrCB[(UINT)_type]; }
	ID3D11RasterizerState* GetRSState(RS_TYPE _Type) { return m_RSState[(UINT)_Type].Get(); }
};

