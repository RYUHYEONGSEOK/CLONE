#include "stdafx.h"
#include "LineBuffer.h"
#include "Device.h"
#include "NaviCell.h"


CLineBuffer::CLineBuffer(CNaviMgr::VECCELL* pNeviCell)
	: Vtx(NULL)
	, Idx(NULL)
{

}

CLineBuffer::~CLineBuffer()
{
}

HRESULT CLineBuffer::CreateBuffer(vector<CNaviCell*>* pNeviCell)
{
	m_iVtxCnt = pNeviCell->size() * 3;
	m_iVertexStrides = sizeof(VTXCOL);
	m_iVertexStrides = 0;

	m_iIndex = pNeviCell->size() + 2;
	return S_OK;
}

CLineBuffer * CLineBuffer::Create(CNaviMgr::VECCELL * pNaviCell)
{
	CLineBuffer*	pBuffer = new CLineBuffer(pNaviCell);

	if (FAILED(pBuffer->CreateBuffer(pNaviCell)))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResources * CLineBuffer::CloneResource(void)
{
	CResources* pResource = new CLineBuffer(*this);

	pResource->AddRef();

	return pResource;

}

//void CLineBuffer::Render()
//{
//	CVIBuffer::Render();
//}

int CLineBuffer::Update()
{
	return 0;
}

DWORD CLineBuffer::Release()
{
	Safe_Delete_Array(Vtx);
	Safe_Delete_Array(Idx);

	return 0;
}

void CLineBuffer::Render_Lines(void)
{
	if (NULL == m_VertexBuffer)
		return;

	ID3D11DeviceContext*	pDeviceContext = CDevice::GetInstance()->m_pDeviceContext;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_iVertexStrides, &m_iVertexOffsets);
	pDeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (m_pRasterizerState)
		pDeviceContext->RSSetState(m_pRasterizerState);

	pDeviceContext->DrawIndexed(m_dwRefCount, m_iStartIndex, m_iBaseVertex);
	pDeviceContext->Draw(m_iVtxCnt, m_iStartVertex);
}

void CLineBuffer::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	CDevice* pGrapicDevice = CDevice::GetInstance();
	pGrapicDevice->m_pDevice->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
}


