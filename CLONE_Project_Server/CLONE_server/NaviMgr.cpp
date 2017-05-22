#include "stdafx.h"
#include "NaviMgr.h"

#include "NaviCell.h"
#include "Line2D.h"

IMPLEMENT_SINGLETON(CNaviMgr)

CNaviMgr::CNaviMgr(void)
	: m_dwIdxCnt(0)
{
}
CNaviMgr::~CNaviMgr(void)
{
	Release();
}

void CNaviMgr::ReserveCellContainerSize(const DWORD& dwSize)
{
	m_dwReserveSize = dwSize;
	m_vecCell.reserve(dwSize);
}

HRESULT CNaviMgr::AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC)
{
	CNaviCell*	pCell = CNaviCell::Create(pPointA, pPointB, pPointC, m_dwIdxCnt);

	++m_dwIdxCnt;
	m_vecCell.push_back(pCell);

	return S_OK;
}

void CNaviMgr::LinkCell(void)
{
	VECCELL::iterator iter = m_vecCell.begin();
	if (iter == m_vecCell.end())
		return;

	for (; iter != m_vecCell.end(); ++iter)
	{
		VECCELL::iterator	iter_Target = m_vecCell.begin();
		while (iter_Target != m_vecCell.end())
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

void CNaviMgr::Release(void)
{
	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();
}

DWORD CNaviMgr::MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwIndex, float _fFrameTime)
{
	CNaviCell::NEIGHBOR		eNeighbor;
	DWORD					dwNextIndex = dwIndex;

	//라인을 통과 했다.
	if (m_vecCell[dwIndex]->CheckPass(pPos, pDir, &eNeighbor))
	{
		const CNaviCell* pNeighbor = m_vecCell[dwIndex]->GetNeighbor(eNeighbor);
		if (pNeighbor == NULL)
		{
			//슬라이딩 벡터
			CLine2D* pPassLine = m_vecCell[dwIndex]->GetPassLine(pPos, pDir);
			D3DXVECTOR2 vLineNormal = pPassLine->GetLineNormal();
			D3DXVECTOR3 vNormal = D3DXVECTOR3(vLineNormal.x, 0.f, vLineNormal.y);
			D3DXVECTOR3 vSliding = *pDir - D3DXVec3Dot(pDir, &vNormal) * vNormal;
			D3DXVec3Normalize(&vSliding, &vSliding);

			//슬라이딩 벡터로 이동
			*pPos += vSliding * _fFrameTime;

			// 밖으로 나가는것 방지
			D3DXVECTOR3 vPosTemp = *pPos;

			if (vPosTemp.x < -HALFMAPX + 1.f || vPosTemp.x > HALFMAPX - 1.f
				|| vPosTemp.z < -HALFMAPZ + 1.f || vPosTemp.z > HALFMAPZ - 1.f)
				*pPos -= vSliding * _fFrameTime;

			GetNaviIndex(pPos, dwNextIndex);
		}
		else
		{
			dwNextIndex = pNeighbor->GetIndex();
			*pPos += *pDir;
		}
	}
	else //라인을 지나가지 않았다.
	{
		*pPos += *pDir;
	}

	return dwNextIndex;
}

bool CNaviMgr::GetNaviIndex(D3DXVECTOR3 * pPos, DWORD & Index)
{
	int iIndex = -1;

	CNaviCell::NEIGHBOR      eNeighbor;

	size_t iSize = m_vecCell.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecCell[i]->CheckPass(pPos, &D3DXVECTOR3(0.f, 0.f, 0.f), &eNeighbor) == false)
		{
			iIndex = i;
			break;
		}
	}

	if (iIndex == -1)
	{
		return false;
	}
	else
	{
		Index = iIndex;
		return true;
	}
}