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


int CStructuredBuffer::Create(UINT _ElementSize, UINT _ElementCount,
	SB_TYPE _Type, bool _SysMemMove, void* _InitData)
{
	// _Type �⺻��: SRV_ONLY
	// _SysMemMove �⺻��: false
	// _InitData �⺻��: nullptr

	assert(0 == (_ElementSize % 16));	// ����ȭ ������ ������ 16����Ʈ�̾�� �Ѵ�.
	
	// CreateBuffer �Ǵ� CreateShaderResourceView�� �� ��, �̹� m_SB�� m_SRV�� �����Ѵٸ� �̸� nullptr�� ����Ű�� �Ͽ� ���� ī��Ʈ�� �ٿ�����Ѵ�.
	m_SB = nullptr;
	m_SB_Write = nullptr;
	m_SB_Read = nullptr;

	m_SRV = nullptr;

	m_ElementSize = _ElementSize;
	m_ElementCount = _ElementCount;

	m_SysMemMove = _SysMemMove;

	// ID3D11Buffer m_SB�� ���� StructuredBuffer �뵵�� �����ϱ�
	if (SB_TYPE::SRV_UAV == _Type)
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	else
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	m_Desc.ByteWidth = m_ElementSize * m_ElementCount;
	m_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_Desc.StructureByteStride = m_ElementSize;


	if (false == m_SysMemMove)
	{
		m_Desc.Usage = D3D11_USAGE_DYNAMIC;
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		m_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Desc.CPUAccessFlags = 0;
	}



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


	// �߰����� ����
	if (m_SysMemMove)
	{
		m_Desc.Usage = D3D11_USAGE_DYNAMIC;
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//// Test
		//D3D11_BUFFER_DESC bufferDesc = {};
		//bufferDesc.Usage = D3D11_USAGE_DYNAMIC;          // ���۸� �������� ����ϵ��� ����
		//bufferDesc.ByteWidth = _ElementSize * _ElementCount; // ������ �� ũ�� (����Ʈ ����)
		//bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // ���̴����� ���ҽ��� �����ϵ��� ����
		//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU�� ���۸� �� �� �ֵ��� ����
		//bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // ����ȭ�� ���۷� ����
		//bufferDesc.StructureByteStride = _ElementSize;   // �� ����� ũ��

		if (nullptr == _InitData)
		{
			hr = DEVICE->CreateBuffer(&m_Desc, nullptr, m_SB_Write.GetAddressOf());
		}

		else
		{
			D3D11_SUBRESOURCE_DATA sub = {};
			sub.pSysMem = _InitData;
			hr = DEVICE->CreateBuffer(&m_Desc, &sub, m_SB_Write.GetAddressOf());
		}

		m_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if (nullptr == _InitData)
		{
			hr = DEVICE->CreateBuffer(&m_Desc, nullptr, m_SB_Read.GetAddressOf());
		}

		else
		{
			D3D11_SUBRESOURCE_DATA sub = {};
			sub.pSysMem = _InitData;
			hr = DEVICE->CreateBuffer(&m_Desc, &sub, m_SB_Read.GetAddressOf());
		}
	}

	// ShaderResourceView �����ϱ�
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
	SRVDesc.BufferEx.NumElements = m_ElementCount;

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &SRVDesc, m_SRV.GetAddressOf())))
	{
		return E_FAIL;
	}

	// UAV ����
	if (m_Type == SB_TYPE::SRV_UAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC Desc = {};

		Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		Desc.Buffer.NumElements = m_ElementCount;
		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &Desc, m_UAV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}


void CStructuredBuffer::SetData(void* _pData, UINT _DataSize)
{
	// �Էµ����Ͱ� ����ȭ���� ũ�⺸�� �۾ƾ��Ѵ�.
	assert(_DataSize <= m_Desc.ByteWidth);

	if (0 == _DataSize)
	{
		_DataSize = m_Desc.ByteWidth;
	}

	// ���� ���۸� ����ϴ� ���
	if (false == m_SysMemMove)
	{
		D3D11_MAPPED_SUBRESOURCE tMapSub = {};
		CONTEXT->Map(m_SB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
		memcpy(tMapSub.pData, _pData, _DataSize);
		CONTEXT->Unmap(m_SB.Get(), 0);
	}

	// �б�, ���� ���۸� ���� ���� ���
	else
	{
		D3D11_MAPPED_SUBRESOURCE tMapSub = {};
		auto test = m_SB_Write;
		CONTEXT->Map(m_SB_Write.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
		memcpy(tMapSub.pData, _pData, _DataSize);
		CONTEXT->Unmap(m_SB_Write.Get(), 0);

		CONTEXT->CopyResource(m_SB.Get(), m_SB_Write.Get());
	}
}

void CStructuredBuffer::GetData(void* _pData, UINT _DataSize)
{
	// ��û�� ������ ũ�Ⱑ ����ȭ���� ũ�⺸�� �۾ƾ��Ѵ�.
	assert(_DataSize <= m_Desc.ByteWidth);
	assert(m_SysMemMove);

	if (0 == _DataSize)
	{
		_DataSize = m_Desc.ByteWidth;
	}

	CONTEXT->CopyResource(m_SB_Read.Get(), m_SB.Get());

	D3D11_MAPPED_SUBRESOURCE tMapSub = {};
	CONTEXT->Map(m_SB_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tMapSub);
	memcpy(_pData, tMapSub.pData, _DataSize);
	CONTEXT->Unmap(m_SB_Read.Get(), 0);
}

void CStructuredBuffer::Binding(UINT _RegisterNum)
{
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}