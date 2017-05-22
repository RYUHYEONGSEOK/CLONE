#include "stdafx.h"
#include "NaviCell.h"
#include "Line2D.h"
#include "GraphicDevice.h"

CNaviCell::CNaviCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC)
	: m_pLine(NULL)
{
	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;
}

CNaviCell::~CNaviCell(void)
{
	Release();
}

HRESULT CNaviCell::InitCell(CDevice* pDevice, const DWORD& dwIdx)
{
	m_pDevice = pDevice;
	m_dwIndex = dwIdx;

	ZeroMemory(m_pNeighbor, sizeof(CNaviCell*) * NEIGHBOR_END);
	m_pLine2D[LINE_AB] = CLine2D::Create(&m_vPoint[POINT_A], &m_vPoint[POINT_B]);
	m_pLine2D[LINE_BC] = CLine2D::Create(&m_vPoint[POINT_B], &m_vPoint[POINT_C]);
	m_pLine2D[LINE_CA] = CLine2D::Create(&m_vPoint[POINT_C], &m_vPoint[POINT_A]);

	return S_OK;
}

bool CNaviCell::ComparePoint(const D3DXVECTOR3* pFirstPoint
	, const D3DXVECTOR3* pSecondPoint
	, CNaviCell* pNeighbor)
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

void CNaviCell::Render(void)
{
	
}

CNaviCell* CNaviCell::Create(CDevice* pDevice, const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, const DWORD& dwIdx)
{
	CNaviCell*		pCell = new CNaviCell(pPointA, pPointB, pPointC);

	if (FAILED(pCell->InitCell(pDevice, dwIdx)))
		Safe_Delete(pCell);

	return pCell;
}

void CNaviCell::Release(void)
{
	for (int i = 0; i < LINE_END; ++i)
		Safe_Delete(m_pLine2D[i]);

	Safe_Release(m_pLine);
}

bool CNaviCell::CheckPass(const D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, NEIGHBOR* pNeighbor)
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_pLine2D[i]->Check_LinePass(&D3DXVECTOR2(pPos->x + pDir->x, pPos->z + pDir->z)))
		{
			*pNeighbor = NEIGHBOR(i);
			return true;
		}
	}
	return false;
}

CLine2D * CNaviCell::GetPassLine(const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pDir)
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_pLine2D[i]->Check_LinePass(&D3DXVECTOR2(pPos->x + pDir->x, pPos->z + pDir->z)))
		{
			return m_pLine2D[i];
		}
	}
	return NULL;
}
