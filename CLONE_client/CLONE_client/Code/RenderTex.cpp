#include "stdafx.h"
#include "RenderTex.h"

CRenderTex::CRenderTex(CDevice * pDevice)
	: CBuffer(pDevice)
{

}

CRenderTex::CRenderTex(const CRenderTex & rhs)
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

CRenderTex::~CRenderTex(void)
{
	Release();
}

HRESULT CRenderTex::CreateBuffer(void)
{
	m_dwVtxCnt = 4;
	m_nVtxSize = sizeof(VertexTexture);
	m_nVtxOffset = 0;

	m_dwIdxCnt = 6;
	m_nIdxStart = 0;
	m_nIdxPlus = 0;

	VertexTexture pVertex[] =
	{
		{ D3DXVECTOR3(-1.f, 1.f, 0.f)
		, D3DXVECTOR3(0.f, 1.f, 0.f)
		, D3DXVECTOR2(0.f, 0.f) },

		{ D3DXVECTOR3(1.f, 1.f, 0.f)
		, D3DXVECTOR3(0.f, 1.f, 0.f)
		, D3DXVECTOR2(1.f, 0.f) },

		{ D3DXVECTOR3(1.f, -1.f, 0.f)
		, D3DXVECTOR3(0.f, 1.f, 0.f)
		, D3DXVECTOR2(1.f, 1.f) },

		{ D3DXVECTOR3(-1.f, -1.f, 0.f)
		, D3DXVECTOR3(0.f, 1.f, 0.f)
		, D3DXVECTOR2(0.f, 1.f) },
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
		{ INDEX16(0, 1, 2) },
		{ INDEX16(0, 2, 3) }
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

CRenderTex * CRenderTex::Create(CDevice * pDevice)
{
	CRenderTex*		pBuffer = new CRenderTex(pDevice);

	if (FAILED(pBuffer->CreateBuffer()))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResource * CRenderTex::Clone(void)
{
	++(*m_pRefCnt);
	return new CRenderTex(*this);
}

void CRenderTex::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CRenderTex::Update()
{

}

void CRenderTex::Release()
{

}

void CRenderTex::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

