#include "stdafx.h"
#include "FountainBuffer.h"

CFountainBuffer::CFountainBuffer(CDevice * pDevice)
	: CBuffer(pDevice)
{

}

CFountainBuffer::CFountainBuffer(const CFountainBuffer & rhs)
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

CFountainBuffer::~CFountainBuffer(void)
{
	Release();
}


HRESULT CFountainBuffer::CreateBuffer(void)
{
	m_nVtxSize = sizeof(VertexTexture);
	m_nVtxOffset = 0;

	m_dwVtxCnt = 7;
	m_dwIdxCnt = 18;
	m_dwTriCnt = 6;
	m_nIdxStart = 0;
	m_nIdxPlus = 0;

	vertices2 = new VertexTexture[m_dwVtxCnt];

	vertices2[0].vPos = D3DXVECTOR3(-0.5f, 0.f, -1.f);
	vertices2[0].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	vertices2[0].vTextureUV = D3DXVECTOR2(0.25f, 0.f);

	vertices2[1].vPos = D3DXVECTOR3(0.5f, 0.f, -1.f);
	vertices2[1].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	vertices2[1].vTextureUV = D3DXVECTOR2(0.75f, 0.f);

	vertices2[2].vPos = D3DXVECTOR3(1.f, 0.f, 0.f);
	vertices2[2].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	vertices2[2].vTextureUV = D3DXVECTOR2(1.f, 0.5f);

	vertices2[3].vPos = D3DXVECTOR3(0.5f, 0.f, 1.f);
	vertices2[3].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	vertices2[3].vTextureUV = D3DXVECTOR2(0.75f, 1.f);

	vertices2[4].vPos = D3DXVECTOR3(-0.5f, 0.f, 1.f);
	vertices2[4].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	vertices2[4].vTextureUV = D3DXVECTOR2(0.25f, 1.f);

	vertices2[5].vPos = D3DXVECTOR3(-1.f, 0.f, 0.f);
	vertices2[5].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	vertices2[5].vTextureUV = D3DXVECTOR2(0.f, 0.5f);

	vertices2[6].vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	vertices2[6].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
	vertices2[6].vTextureUV = D3DXVECTOR2(0.5f, 0.5f);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = m_nVtxSize * m_dwVtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices2;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&bd, &InitData, &m_pVtxBuffer), E_FAIL);

	INDEX32* Index = new INDEX32[m_dwIdxCnt];

	Index[0] = INDEX32(6, 0, 1);
	Index[1] = INDEX32(6, 1, 2);
	Index[2] = INDEX32(6, 2, 3);
	Index[3] = INDEX32(6, 3, 4);
	Index[4] = INDEX32(6, 4, 5);
	Index[5] = INDEX32(6, 5, 0);

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(DWORD) * m_dwIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = Index;
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

	//¼öÁ¤
	Safe_Delete(vertices2);
	Safe_Delete(Index);

	return S_OK;
}

CFountainBuffer * CFountainBuffer::Create(CDevice * pDevice)
{
	CFountainBuffer*		pBuffer = new CFountainBuffer(pDevice);

	if (FAILED(pBuffer->CreateBuffer()))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResource * CFountainBuffer::Clone(void)
{
	++(*m_pRefCnt);
	return new CFountainBuffer(*this);
}

void CFountainBuffer::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CFountainBuffer::Update()
{

}

void CFountainBuffer::Release()
{

}

void CFountainBuffer::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	tRasterizerDesc.AntialiasedLineEnable = TRUE;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

