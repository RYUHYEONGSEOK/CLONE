#include "stdafx.h"
#include "VIBuffer.h"

CBuffer::CBuffer(CDevice* pDevice)
	: CResource(pDevice)
	, m_pRasterizerState(NULL)
	, m_pCommandList(NULL)

	, m_pVtxBuffer(NULL)
	, m_dwVtxCnt(0)
	, m_nVtxSize(0)
	, m_nVtxOffset(0)
	, m_nVtxStart(0)

	, m_pIdxBuffer(NULL)
	, m_dwIdxCnt(0)
	, m_nIdxStart(0)
	, m_nIdxPlus(0)

	, m_dwTriCnt(0)

	, m_ConstantBuffer(NULL)
	, m_Float4Buffer(NULL)

	, vertices(NULL)
	, vertices2(NULL)
	, indices(NULL)
{
	
}

CBuffer::CBuffer(const CBuffer & rhs)
	: CResource(rhs.m_pDevice)
{

}

CBuffer::~CBuffer()
{
	Release();
}

void CBuffer::Render(DXGIFORMATTYPE eFormatType /*= DXGI_END*/, DRAWTYPE eDrawType /*= DRAW_INDEX*/)
{
	if (NULL == m_pVtxBuffer)
		return;

	ID3D11DeviceContext* pDeviceContext = CDevice::GetInstance()->GetDeviceContext();
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVtxBuffer, &m_nVtxSize, &m_nVtxOffset);
	if(eFormatType == DXGI_16)
		pDeviceContext->IASetIndexBuffer(m_pIdxBuffer, DXGI_FORMAT_R16_UINT, 0);
	else if(eFormatType == DXGI_32)
		pDeviceContext->IASetIndexBuffer(m_pIdxBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (m_pRasterizerState)
		pDeviceContext->RSSetState(m_pRasterizerState);

	if(eDrawType == DRAW_INDEX)
		pDeviceContext->DrawIndexed(m_dwIdxCnt, m_nIdxStart, m_nIdxPlus);
	else if(eDrawType == DRAW_NORMAL)
		pDeviceContext->Draw(m_dwVtxCnt, m_nVtxStart);
}

void CBuffer::Release()
{
	if ((*m_pRefCnt) == 0)
	{
		Safe_Release(m_pVtxBuffer);
		Safe_Release(m_pIdxBuffer);
		Safe_Delete_Array(vertices);
		Safe_Delete_Array(indices);
		Safe_Delete_Array(vertices2);
		Safe_Release(m_ConstantBuffer);
		Safe_Release(m_Float4Buffer);

		CResource::Release_RefCnt();
	}
	else 
		--(*m_pRefCnt);
}

void CBuffer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_BACK;
	tRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	CDevice::GetInstance()->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

// Vertex Buffer Struct ---------------
void VertexAni::AddBone(int iIdx, float fWeight)
{
	for (int i = 0; i < BONE_NUM; ++i)
	{
		if (fBoneWeight[i] <= 0.0f)
		{
			iBoneIdx[i] = iIdx;
			fBoneWeight[i] = fWeight;
			return;
		}
	}

	fBoneWeight[0] += fWeight;
}