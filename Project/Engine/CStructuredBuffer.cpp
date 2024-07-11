#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_Desc{}
	, m_ElementCount(0)
	, m_ElementSize(0)
{
}

CStructuredBuffer::~CStructuredBuffer()
{
}


int CStructuredBuffer::Create(UINT _ElementSize, UINT _ElementCount, void* _InitData)
{
	// _InitData 기본값: nullptr
	
	// CreateBuffer 또는 CreateShaderResourceView를 할 때, 이미 m_SB나 m_SRV가 존재한다면 이를 nullptr로 가리키게 하여 참조 카운트를 줄여줘야한다.
	m_SB = nullptr;
	m_SRV = nullptr;

	m_ElementSize = _ElementSize;
	m_ElementCount = _ElementCount;

	// ID3D11Buffer 를 StructuredBuffer 용도로 생성하기
	m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	m_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	m_Desc.ByteWidth = m_ElementSize * m_ElementCount;

	m_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_Desc.StructureByteStride = m_ElementSize;


	HRESULT hr = S_OK;
	if (nullptr == _InitData)
	{
		hr = DEVICE->CreateBuffer(&m_Desc, nullptr, m_SB.GetAddressOf());
	}

	else
	{
		D3D11_SUBRESOURCE_DATA sub = {};
		sub.pSysMem = _InitData;
		hr = DEVICE->CreateBuffer(&m_Desc, &sub, m_SB.GetAddressOf());
	}

	if (FAILED(hr))
		return E_FAIL;


	// ShaderResourceView 생성하기
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
	SRVDesc.BufferEx.NumElements = m_ElementCount;

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &SRVDesc, m_SRV.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}


void CStructuredBuffer::SetData(void* _pData, UINT _DataSize)
{
	// 입력데이터가 구조화버퍼 크기보다 작아야한다.
	assert(_DataSize <= m_Desc.ByteWidth);

	if (0 == _DataSize)
	{
		_DataSize = m_Desc.ByteWidth;
	}

	D3D11_MAPPED_SUBRESOURCE tMapSub = {};
	CONTEXT->Map(m_SB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
	memcpy(tMapSub.pData, _pData, _DataSize);
	CONTEXT->Unmap(m_SB.Get(), 0);
}

void CStructuredBuffer::Binding(UINT _RegisterNum)
{
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}