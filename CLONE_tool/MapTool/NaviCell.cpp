#include "stdafx.h"
#include "NaviCell.h"
#include "Device.h"
#include "Camera.h"

CNaviCell::CNaviCell(const D3DXVECTOR3 * pPointA, const D3DXVECTOR3 * pPointB, const D3DXVECTOR3 * pPointC)
{
	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;
}

CNaviCell::~CNaviCell()
{
	Release();
}

const D3DXVECTOR3 * CNaviCell::GetPoint(POINT ePointID)
{
	return &m_vPoint[ePointID];
}

CNaviCell * CNaviCell::GetNeighbor(NEIGHBOR eNeighbor)
{
	return m_pNeighbor[eNeighbor];
}

DWORD CNaviCell::GetIndex(void)
{
	return m_dwIndex;
}

HRESULT CNaviCell::InitCell(const DWORD & dwIdx)
{
	m_dwIndex = dwIdx;
	ZeroMemory(m_pNeighbor, sizeof(CNaviCell*) * NEIGHBOR_END);

	HRESULT hr = E_FAIL;

	m_iVtxCnt = 3;
	m_iVertexStrides = sizeof(VTXCOL);
	m_iVertexOffsets = 0;

	m_iIndex = 3;
	Vtx = new VTXCOL[m_iVtxCnt];

	int iIndex = 0;

	for (int i = 0; i < 3; ++i)
	{
		Vtx[i].vPos = m_vPoint[i];
		Vtx[i].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 0.f);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = m_iVertexStrides * m_iVtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vtx;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&bd, &InitData, &m_VertexBuffer);

	Idx = new INDEX16[m_iIndex];

	iIndex = 0;

	for (int i = 0; i < 1; ++i)
	{
		Idx[iIndex]._1 = (i * 3);
		Idx[iIndex]._2 = (i * 3) + 1;
		Idx[iIndex]._3 = (i * 3) + 2;

		iIndex++;
	}

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(DWORD) * m_iIndex;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = Idx;

	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&tBufferDesc, &tData, &m_IndexBuffer);

	D3D11_BUFFER_DESC cb;
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.ByteWidth = sizeof(ConstantBuffer);
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cb, NULL, &m_ConstantBuffer);

	D3D11_RASTERIZER_DESC tRasterizerDesc;
	CDevice* pGrapicDevice = CDevice::GetInstance();

	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

	pGrapicDevice->m_pDevice->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);

	return S_OK;
}

bool CNaviCell::ComparePoint(const D3DXVECTOR3 * pFirstPoint, const D3DXVECTOR3 * pSecondPoint, CNaviCell * pNeighbor)
{
	if (*pFirstPoint == m_vPoint[POINT_A])
	{
		if (*pSecondPoint == m_vPoint[POINT_B])
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if (*pSecondPoint == m_vPoint[POINT_C])
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
	}

	if (*pFirstPoint == m_vPoint[POINT_B])
	{
		if (*pSecondPoint == m_vPoint[POINT_A])
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if (*pSecondPoint == m_vPoint[POINT_C])
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}

	if (*pFirstPoint == m_vPoint[POINT_C])
	{
		if (*pSecondPoint == m_vPoint[POINT_A])
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
		else if (*pSecondPoint == m_vPoint[POINT_B])
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}

	return false;
}

bool CNaviCell::CheckPass(const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pDir, NEIGHBOR * pNeighbor)
{
	return false;
}

void CNaviCell::Render(void)
{
	if (NULL == m_VertexBuffer)
		return;

	ID3D11DeviceContext*	pDeviceContext = CDevice::GetInstance()->m_pDeviceContext;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_iVertexStrides, &m_iVertexOffsets);
	pDeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (m_pRasterizerState)
		pDeviceContext->RSSetState(m_pRasterizerState);

	SelectBuffer cb1;
	cb1.vSelect.x = 1.f;
	cb1.vSelect.y = 0.f;
	cb1.vSelect.z = 0.f;
	cb1.vSelect.w = 0.f;
	CDevice::GetInstance()->m_pDeviceContext->UpdateSubresource(CCamera::GetInstance()->m_cbSelectBuffer, 0, NULL, &cb1, 0, 0);
	CDevice::GetInstance()->m_pDeviceContext->PSSetConstantBuffers(3, 1, &CCamera::GetInstance()->m_cbSelectBuffer);

	pDeviceContext->DrawIndexed(0, 0, 0);
	pDeviceContext->Draw(m_iVtxCnt, 0);
}

CNaviCell * CNaviCell::Create(const D3DXVECTOR3 * pPointA, const D3DXVECTOR3 * pPointB, const D3DXVECTOR3 * pPointC, const DWORD & dwIdx)
{
	CNaviCell*		pCell = new CNaviCell(pPointA, pPointB, pPointC);
	if (FAILED(pCell->InitCell(dwIdx)))
		Safe_Delete(pCell);

	return pCell;
}

DWORD CNaviCell::Release(void)
{
	Safe_Delete_Array(Vtx);
	Safe_Delete_Array(Idx);

	Safe_Release(m_VertexBuffer);
	Safe_Release(m_IndexBuffer);
	Safe_Release(m_ConstantBuffer);

	return 0;
}

CResources * CNaviCell::CloneResource(void)
{
	return nullptr;
}
