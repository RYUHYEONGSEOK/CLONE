#include "stdafx.h"
#include "LineBuffer.h"
//Manager
#include "GraphicDevice.h"
//Object
#include "NaviCell.h"

CLineBuffer::CLineBuffer(CDevice * pDevice, CNaviMgr::VECCELL* pNeviCell)
	: CBuffer(pDevice)
	, vtx(NULL)
	, Idx(NULL)
{

}

CLineBuffer::CLineBuffer(const CLineBuffer & rhs)
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

	vtx = rhs.vtx;
	Idx = rhs.Idx;

	m_pRefCnt = rhs.m_pRefCnt;
}

CLineBuffer::~CLineBuffer(void)
{
	Release();
}

HRESULT CLineBuffer::CreateBuffer(vector<CNaviCell*>* pNeviCell)
{
	m_dwVtxCnt = pNeviCell->size() * 3;
	m_nVtxSize = sizeof(VertexColor);
	m_nVtxOffset = 0;

	m_dwIdxCnt = pNeviCell->size();
	m_nIdxStart = 0;
	m_nIdxPlus = 0;

	vtx = new VertexColor[m_dwVtxCnt];

	int iIndex = 0;

	for (int i = 0; i < (int)pNeviCell->size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			iIndex = i * 3 + j;

			vtx[iIndex].vPos = (((*pNeviCell)[i])->GetPoint())[j];
			vtx[iIndex].vColor = D3DXVECTOR4(1.f, 0.f, 0.f, 1.f);
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
	InitData.pSysMem = vtx;
	FAILED_CHECK_RETURN(CDevice::GetInstance()->GetDevice()->CreateBuffer(&bd, &InitData, &m_pVtxBuffer), E_FAIL);

	Idx = new INDEX16[m_dwIdxCnt];

	iIndex = 0;

	for (int i = 0; i < (int)pNeviCell->size(); ++i)
	{
		Idx[iIndex]._1 = (i * 3);
		Idx[iIndex]._2 = (i * 3) + 1;
		Idx[iIndex]._3 = (i * 3) + 2;

		iIndex++;
	}
	
	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(DWORD) * m_dwIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = Idx;
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

CLineBuffer * CLineBuffer::Create(CDevice * pDevice, CNaviMgr::VECCELL* pNeviCell)
{
	CLineBuffer*		pBuffer = new CLineBuffer(pDevice, pNeviCell);

	if (FAILED(pBuffer->CreateBuffer(pNeviCell)))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResource * CLineBuffer::Clone(void)
{
	++(*m_pRefCnt);
	return new CLineBuffer(*this);
}

void CLineBuffer::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CLineBuffer::Update()
{

}

void CLineBuffer::Release()
{
	Safe_Delete_Array(vtx);
	Safe_Delete_Array(Idx);
}

void CLineBuffer::Render_Lines(void)
{
	if (NULL == m_pVtxBuffer)
		return;

	ID3D11DeviceContext* pDeviceContext = CDevice::GetInstance()->GetDeviceContext();
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVtxBuffer, &m_nVtxSize, &m_nVtxOffset);
	pDeviceContext->IASetIndexBuffer(m_pIdxBuffer, DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (m_pRasterizerState)
		pDeviceContext->RSSetState(m_pRasterizerState);

	pDeviceContext->DrawIndexed(m_dwIdxCnt, m_nIdxStart, m_nIdxPlus);
	pDeviceContext->Draw(m_dwVtxCnt, m_nVtxStart);
}

void CLineBuffer::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

