#include "stdafx.h"
#include "NaviMgr.h"
#include "NaviCell.h"

IMPLEMENT_SINGLETON(CNaviMgr)

vector<CNaviCell*>  CNaviMgr::m_vecNaviMesh;

CNaviMgr::CNaviMgr()
	: m_pDevice(NULL)
	, m_dwReserveSize(0)
	, m_dwIdxCnt(0)
{
}


CNaviMgr::~CNaviMgr()
{
	Release();
}

void CNaviMgr::Reserve_cellContainerSize(const DWORD & dwSize)
{
	m_dwReserveSize = dwSize;
	m_vecNaviMesh.reserve(dwSize);
}

HRESULT CNaviMgr::Add_Cell(const D3DXVECTOR3 * pPointA, const D3DXVECTOR3 * pPointB, const D3DXVECTOR3 * pPointC)
{
	if (m_dwReserveSize == 0)
		return E_FAIL;

	CNaviCell*	pCell = CNaviCell::Create(pPointA, pPointB, pPointC, m_dwIdxCnt);
	if (pCell == NULL)
		return E_FAIL;

	++m_dwIdxCnt;

	m_vecNaviMesh.push_back(pCell);
	return S_OK;
}

void CNaviMgr::Link_Cell(void)
{
	VECCELL::iterator	iter = m_vecNaviMesh.begin();

	if (m_vecNaviMesh.end() == iter)
		return;

	for (; iter != m_vecNaviMesh.end(); ++iter)
	{
		VECCELL::iterator	iter_Target = m_vecNaviMesh.begin();

		while (iter_Target != m_vecNaviMesh.end())
		{
			if (iter == iter_Target)
			{
				++iter_Target;
				continue;
			}

			if ((*iter_Target)->ComparePoint((*iter)->GetPoint(CNaviCell::POINT_A)
				, (*iter)->GetPoint(CNaviCell::POINT_B), (*iter)))
			{
				(*iter)->SetNeighbor(CNaviCell::NEIGHBOR_AB, (*iter_Target));
			}
			else if ((*iter_Target)->ComparePoint((*iter)->GetPoint(CNaviCell::POINT_B)
				, (*iter)->GetPoint(CNaviCell::POINT_C), (*iter)))
			{
				(*iter)->SetNeighbor(CNaviCell::NEIGHBOR_BC, (*iter_Target));
			}
			else if ((*iter_Target)->ComparePoint((*iter)->GetPoint(CNaviCell::POINT_C)
				, (*iter)->GetPoint(CNaviCell::POINT_A), (*iter)))
			{
				(*iter)->SetNeighbor(CNaviCell::NEIGHBOR_CA, (*iter_Target));
			}

			++iter_Target;
		}
	}
}

void CNaviMgr::Render(void)
{
	UINT		iSize = m_vecNaviMesh.size();

	for (UINT i = 0; i < iSize; ++i)
	{
		m_vecNaviMesh[i]->Render();
	}
}

DWORD CNaviMgr::MoveOnNaviMesh(D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pDir, const DWORD & dwCurrentIdx)
{
	CNaviCell::NEIGHBOR	Neighbor;

	DWORD	dwNextIndex = dwCurrentIdx;

	if (m_vecNaviMesh[dwCurrentIdx]->CheckPass(pPos, pDir, &Neighbor))
	{
		CNaviCell*   pNeighbor = m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(Neighbor);
		if (pNeighbor == NULL)
		{

		}
		else
		{
			dwNextIndex = pNeighbor->GetIndex();
			*pPos += *pDir;
		}
	}
	else
		*pPos += *pDir;

	return dwNextIndex;
}

void CNaviMgr::GetNearPoint(D3DXVECTOR3 & vPoint, float fRange)
{
	if (m_vecNaviMesh.empty())
		return;

	D3DXVECTOR3 vMinPt = *(m_vecNaviMesh[0]->GetPoint(CNaviCell::POINT_A)) - vPoint;
	float fMinDist = D3DXVec3Length(&vMinPt);

	for (size_t i = 0; i < m_vecNaviMesh.size(); ++i)
	{
		for (int j = 0; j < CNaviCell::POINT_END; ++j)
		{
			D3DXVECTOR3 vTemp2 = *(m_vecNaviMesh[i]->GetPoint((CNaviCell::POINT)j)) - vPoint;
			float fTempDist = D3DXVec3Length(&vTemp2);

			vMinPt = (fMinDist < fTempDist ? vMinPt : vTemp2);
			fMinDist = min(fMinDist, fTempDist);
		}
	}

	if (fMinDist < fRange)
		vPoint += vMinPt;
	else
		return;
}

void CNaviMgr::Release(void)
{
	for_each(m_vecNaviMesh.begin(), m_vecNaviMesh.end(), CDeleteObj());
	m_vecNaviMesh.clear();
}
