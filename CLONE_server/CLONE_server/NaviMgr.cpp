#include "stdafx.h"
#include "NaviMgr.h"

#include "NaviCell.h"
#include "Line2D.h"

IMPLEMENT_SINGLETON(CNaviMgr)

CNaviMgr::CNaviMgr(void)
{
	ZeroMemory(m_dwIdxCnt, sizeof(m_dwIdxCnt));
}
CNaviMgr::~CNaviMgr(void)
{
	Release();
}

void CNaviMgr::ReserveCellContainerSize(const DWORD& dwSize, eMapType _eMapType)
{
	m_vecCell[_eMapType].reserve(dwSize);
}

HRESULT CNaviMgr::AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, eMapType _eMapType)
{
	CNaviCell* pCell = CNaviCell::Create(pPointA, pPointB, pPointC, m_dwIdxCnt[_eMapType]);

	++m_dwIdxCnt[_eMapType];
	m_vecCell[_eMapType].push_back(pCell);

	return S_OK;
}

void CNaviMgr::LinkCell(void)
{
	for (int i = 0; i < MAP_END; ++i)
	{
		VECCELL::iterator iter = m_vecCell[i].begin();
		if (iter == m_vecCell[i].end())
			return;

		for (iter; iter != m_vecCell[i].end(); ++iter)
		{
			VECCELL::iterator	iter_Target = m_vecCell[i].begin();
			while (iter_Target != m_vecCell[i].end())
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
}

void CNaviMgr::Release(void)
{
	for (int i = 0; i < MAP_END; ++i)
	{
		for_each(m_vecCell[i].begin(), m_vecCell[i].end(), CDeleteObj());
		m_vecCell[i].clear();
	}
}

DWORD CNaviMgr::MoveOnNaviMesh(D3DXVECTOR3* OUT pPos, const D3DXVECTOR3* OUT pDir, const DWORD& OUT dwIndex, float _fFrameTime, eMapType _eMapType)
{
	CNaviCell::NEIGHBOR		eNeighbor;
	DWORD					dwNextIndex = dwIndex;

	//라인을 통과 했다.
	if (m_vecCell[_eMapType][dwIndex]->CheckPass(pPos, pDir, &eNeighbor))
	{
		const CNaviCell* pNeighbor = m_vecCell[_eMapType][dwIndex]->GetNeighbor(eNeighbor);
		if (pNeighbor == NULL)
		{
			//슬라이딩 벡터
			CLine2D* pPassLine = m_vecCell[_eMapType][dwIndex]->GetPassLine(pPos, pDir);
			D3DXVECTOR2 vLineNormal = pPassLine->GetLineNormal();
			D3DXVECTOR3 vNormal = D3DXVECTOR3(vLineNormal.x, 0.f, vLineNormal.y);
			D3DXVECTOR3 vSliding = *pDir - D3DXVec3Dot(pDir, &vNormal) * vNormal;
			D3DXVec3Normalize(&vSliding, &vSliding);

			//슬라이딩 벡터로 이동
			*pPos += vSliding * _fFrameTime;

			//밀어준다
			D3DXMATRIX matRot;
			D3DXMatrixIdentity(&matRot);

			//벡터 사이각 구하기
			float fAngle = D3DXVec3Dot(pDir, &vNormal);
			if (fAngle > 0.f) D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
			else D3DXMatrixRotationY(&matRot, D3DXToRadian(-180.f));
			D3DXVec3TransformCoord(&vNormal, &vNormal, &matRot);
			*pPos += vNormal * _fFrameTime;

			// 밖으로 나가는것 방지
			D3DXVECTOR3 vPosTemp = *pPos;

			if (vPosTemp.x < -HALF_PLAZA_MAPX + 1.f || vPosTemp.x > HALF_PLAZA_MAPX - 1.f
				|| vPosTemp.z < -HALF_PLAZA_MAPZ + 1.f || vPosTemp.z > HALF_PLAZA_MAPZ - 1.f)
				*pPos -= vSliding * _fFrameTime;

			GetNaviIndex(pPos, dwNextIndex, _eMapType);
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

bool CNaviMgr::GetNaviIndex(D3DXVECTOR3 * pPos, DWORD& Index, eMapType _eMapType)
{
	size_t iIndex = UNKNOWN_VALUE;

	CNaviCell::NEIGHBOR eNeighbor;
	size_t iSize = m_vecCell[_eMapType].size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecCell[_eMapType][i]->CheckPass(pPos, &D3DXVECTOR3(0.f, 0.f, 0.f), &eNeighbor) == false)
		{
			iIndex = i;
			break;
		}
	}

	if (iIndex == UNKNOWN_VALUE)
	{
		return false;
	}
	else
	{
		Index = (DWORD)iIndex;
		return true;
	}
}