#include "stdafx.h"
#include "WaterBuffer.h"

CWaterBuffer::CWaterBuffer(CDevice * pDevice)
	: CBuffer(pDevice)
{

}

CWaterBuffer::CWaterBuffer(const CWaterBuffer & rhs)
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

CWaterBuffer::~CWaterBuffer(void)
{
	Release();
}


HRESULT CWaterBuffer::CreateBuffer(const WORD& wCntX, const WORD& wCntZ)
{
	m_nVtxSize = sizeof(VertexTexture);
	m_nVtxOffset = 0;

	m_dwVtxCnt = wCntX * wCntZ;
	m_dwIdxCnt = (wCntX - 1) * (wCntZ - 1) * 6;
	m_dwTriCnt = (wCntX - 1) * (wCntZ - 1) * 2;
	
	vertices2 = new VertexTexture[m_dwVtxCnt];

	int		iIndex = 0;

	for (int z = 0; z < wCntZ; ++z)
	{
		for (int x = 0; x < wCntX; ++x)
		{
			iIndex = z * wCntX + x;

			vertices2[iIndex].vPos = D3DXVECTOR3(float(x) * 2.f, 0.f, float(z) * 2.f);
			vertices2[iIndex].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
			vertices2[iIndex].vTextureUV = D3DXVECTOR2(x / (wCntX - 1.f), z / (wCntZ - 1.f));
		}
	}

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

	//그리는 순서에 따른 인덱스 정보 입력
	INDEX32* Index = new INDEX32[m_dwIdxCnt];

	int	iTriCnt = 0;

	for (int z = 0; z < wCntZ - 1; ++z)
	{
		for (int x = 0; x < wCntX - 1; ++x)
		{
			iIndex = z * wCntX + x;

			// 오른쪽 위
			Index[iTriCnt]._1 = iIndex + wCntX;
			Index[iTriCnt]._2 = iIndex + wCntX + 1;
			Index[iTriCnt]._3 = iIndex + 1;

			++iTriCnt;

			// 왼쪽 아래
			Index[iTriCnt]._1 = iIndex + wCntX;
			Index[iTriCnt]._2 = iIndex + 1;
			Index[iTriCnt]._3 = iIndex;

			++iTriCnt;
		}
	}

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(UINT) * m_dwIdxCnt;
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

	//수정
	Safe_Delete(vertices2);
	Safe_Delete(Index);

	return S_OK;
}

CWaterBuffer * CWaterBuffer::Create(CDevice * pDevice, const WORD& wSizeX, const WORD& wSizeZ)
{
	CWaterBuffer*		pBuffer = new CWaterBuffer(pDevice);

	if (FAILED(pBuffer->CreateBuffer(wSizeX, wSizeZ)))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResource * CWaterBuffer::Clone(void)
{
	++(*m_pRefCnt);
	return new CWaterBuffer(*this);
}

void CWaterBuffer::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CWaterBuffer::Update()
{

}

void CWaterBuffer::Release()
{
	
}

void CWaterBuffer::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;
	tRasterizerDesc.AntialiasedLineEnable = TRUE;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

