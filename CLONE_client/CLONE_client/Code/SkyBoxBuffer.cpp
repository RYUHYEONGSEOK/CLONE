#include "stdafx.h"
#include "SkyBoxBuffer.h"

CSkyBoxBuffer::CSkyBoxBuffer(CDevice * pDevice)
	: CBuffer(pDevice)
{

}

CSkyBoxBuffer::CSkyBoxBuffer(const CSkyBoxBuffer & rhs)
	: CBuffer(rhs.m_pDevice)
{
	m_pRasterizerState = rhs.m_pRasterizerState;

	m_pVtxBuffer = rhs.m_pVtxBuffer;
	m_dwVtxCnt = rhs.m_dwVtxCnt;
	m_nVtxSize = rhs.m_nVtxSize;
	m_nVtxOffset = rhs.m_nVtxOffset;
	m_nVtxStart = rhs.m_nVtxStart;

	m_pIdxBuffer = rhs.m_pIdxBuffer;
	m_dwIdxCnt = rhs.m_dwIdxCnt;
	m_nIdxStart = rhs.m_nIdxStart;
	m_nIdxPlus = rhs.m_nIdxPlus;

	m_dwTriCnt = rhs.m_dwTriCnt;

	m_ConstantBuffer = rhs.m_ConstantBuffer;

	vertices = rhs.vertices;
	indices = rhs.indices;

	m_pRefCnt = rhs.m_pRefCnt;
}

CSkyBoxBuffer::~CSkyBoxBuffer(void)
{
	Release();
}

HRESULT CSkyBoxBuffer::CreateBuffer(void)
{
	m_dwVtxCnt = 8;
	m_nVtxSize = sizeof(VertexSkyBox);
	m_nVtxOffset = 0;
	m_nVtxStart = 1;

	m_dwIdxCnt = 36;
	m_nIdxStart = 0;
	m_nIdxPlus = 0;

	VertexSkyBox pVertex[] =
	{
		{ D3DXVECTOR3(-1.f, 1.f, -1.f)
		, D3DXVECTOR2(0.f, 0.f) },

		{ D3DXVECTOR3(1.f, 1.f, -1.f)
		, D3DXVECTOR2(1.f, 0.f) },

		{ D3DXVECTOR3(1.f, -1.f, -1.f)
		, D3DXVECTOR2(1.f, 1.f) },

		{ D3DXVECTOR3(-1.f, -1.f, -1.f)
		, D3DXVECTOR2(0.f, 1.f) },

		{ D3DXVECTOR3(-1.f, 1.f, 1.f)
		, D3DXVECTOR2(0.f, 0.f) },

		{ D3DXVECTOR3(1.f, 1.f, 1.f)
		, D3DXVECTOR2(1.f, 0.f) },

		{ D3DXVECTOR3(1.f, -1.f, 1.f)
		, D3DXVECTOR2(1.f, 1.f) },

		{ D3DXVECTOR3(-1.f, -1.f, 1.f)
		, D3DXVECTOR2(0.f, 1.f) }
	};


	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = m_nVtxSize * m_dwVtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pVertex;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&bd, &InitData, &m_pVtxBuffer), E_FAIL);

	INDEX16 pIdx[] =
	{
		// Front
		{ INDEX16(0, 1, 2) },
		{ INDEX16(0, 2, 3) },

		// Right
		{ INDEX16(1, 5, 6) },
		{ INDEX16(1, 6, 2) },

		// Bottom
		{ INDEX16(3, 2, 6) },
		{ INDEX16(3, 6, 7) },

		// Left
		{ INDEX16(4, 0, 3) },
		{ INDEX16(4, 3, 7) },

		// Top
		{ INDEX16(4, 5, 1) },
		{ INDEX16(4, 1, 0) },

		// Back
		{ INDEX16(5, 4, 7) },
		{ INDEX16(5, 7, 6) }
	};

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(DWORD) * m_dwIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = pIdx;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&tBufferDesc, &tData, &m_pIdxBuffer), E_FAIL);

	//ConstantBuffer
	D3D11_BUFFER_DESC cb;
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.ByteWidth = sizeof(ConstantBuffer);
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&cb, NULL, &m_ConstantBuffer), E_FAIL);

	CreateRasterizerState();

	return S_OK;
}

CSkyBoxBuffer * CSkyBoxBuffer::Create(CDevice * pDevice)
{
	CSkyBoxBuffer*		pBuffer = new CSkyBoxBuffer(pDevice);

	if (FAILED(pBuffer->CreateBuffer()))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResource * CSkyBoxBuffer::Clone(void)
{
	++(*m_pRefCnt);
	return new CSkyBoxBuffer(*this);
}

void CSkyBoxBuffer::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CSkyBoxBuffer::Update()
{

}

void CSkyBoxBuffer::Release()
{

}

void CSkyBoxBuffer::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	tRasterizerDesc.DepthClipEnable = true;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

