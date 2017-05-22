#include "stdafx.h"
#include "NaviMgr.h"
#include "NaviCell.h"
#include "GraphicDevice.h"
#include "ResourceMgr.h"
#include "LineBuffer.h"
#include "LineShader.h"
#include "Camera.h"
#include "Line2D.h"
#include "TimeMgr.h"

IMPLEMENT_SINGLETON(CNaviMgr)

CNaviMgr::CNaviMgr(void)
	: m_dwIdxCnt(0)
	, m_pBuffer(NULL)
	, m_pShader(NULL)
	, m_pCam(NULL)
{

}

CNaviMgr::~CNaviMgr(void)
{
	Release();
}

void CNaviMgr::Reserve_CellContainerSize(CDevice* pDevice, const DWORD& dwSize)
{
	m_pDevice = pDevice;
	m_dwReserveSize = dwSize;
	m_vecCell.reserve(dwSize);	

	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Line"));

	m_matWorld = XMMatrixIdentity();
}

HRESULT CNaviMgr::AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC)
{
	CNaviCell*		pCell = CNaviCell::Create(m_pDevice, pPointA, pPointB, pPointC, m_dwIdxCnt);
	NULL_CHECK_RETURN(pCell, E_FAIL);

	++m_dwIdxCnt;

	m_vecCell.push_back(pCell);

	return S_OK;
}

void CNaviMgr::LinkCell(void)
{
	VECCELL::iterator		iter = m_vecCell.begin();
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

void CNaviMgr::Render_NaviMesh(void)
{
	if (m_pBuffer == NULL)
	{
		m_pBuffer = CLineBuffer::Create(m_pDevice, &m_vecCell);
		NULL_CHECK(m_pBuffer);
	}

	XMFLOAT4X4 matView, matProj, matWorld;
	memcpy(&matView, ((CCamera*)m_pCam)->GetViewMatrix(), sizeof(float) * 16);
	memcpy(&matProj, ((CCamera*)m_pCam)->GetProjMatrix(), sizeof(float) * 16);
	XMStoreFloat4x4(&matWorld, m_matWorld);

	((CLineShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer);

	m_pBuffer->Render_Lines();
}

void CNaviMgr::Release(void)
{
	Safe_Delete(m_pBuffer);

	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();
}

DWORD CNaviMgr::MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwIndex)
{
	CNaviCell::NEIGHBOR		eNeighbor;
	DWORD					dwNextIndex = dwIndex;

	//라인을 통과 했다.
	if (m_vecCell[dwIndex]->CheckPass(pPos, pDir, &eNeighbor))
	{
		const CNaviCell*		pNeighbor = m_vecCell[dwIndex]->GetNeighbor(eNeighbor);
		if (pNeighbor == NULL)
		{
			//슬라이딩 벡터
			CLine2D* pPassLine = m_vecCell[dwIndex]->GetPassLine(pPos, pDir);
			D3DXVECTOR2 vLineNormal = pPassLine->GetLineNormal();
			D3DXVECTOR3 vNormal = D3DXVECTOR3(vLineNormal.x, 0.f, vLineNormal.y);
			D3DXVECTOR3 vSliding = *pDir - D3DXVec3Dot(pDir, &vNormal) * vNormal;
			D3DXVec3Normalize(&vSliding, &vSliding);

			//슬라이딩 벡터로 이동
			*pPos += vSliding * CTimeMgr::GetInstance()->GetTime();

			// 밖으로 나가는것 방지
			D3DXVECTOR3 vPosTemp = *pPos;
			
			if (vPosTemp.x < -HALFMAPX + 1.f || vPosTemp.x > HALFMAPX - 1.f
				|| vPosTemp.z < -HALFMAPZ + 1.f || vPosTemp.z > HALFMAPZ - 1.f)
				*pPos -= vSliding * CTimeMgr::GetInstance()->GetTime();
			
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

DWORD CNaviMgr::MoveOnNaviMesh_Cam(D3DXVECTOR3* pEye, D3DXVECTOR3* pAt,
	const D3DXVECTOR3* pCamDir, float fCamDistance, float fCamY, const DWORD& dwIndex)
{
	CNaviCell::NEIGHBOR		eNeighbor;
	DWORD					dwNextIndex = dwIndex;
	
	// 우선 밖으로 나갔는지 체크한다.
	// Eye가 맵밖으로 나갔다면 Navi 체크를 할 필요가 없다.
	if (pEye->x < -HALFMAPX + 1.f || pEye->x > HALFMAPX - 1.f
		|| pEye->z < -HALFMAPZ + 1.f || pEye->z > HALFMAPZ - 1.f)
	{
		return 0;
	}

	//라인을 통과 했다.
	if (m_vecCell[dwIndex]->CheckPass(pEye, &D3DXVECTOR3(0.f, 0.f, 0.f), &eNeighbor))
	{
		const CNaviCell*		pNeighbor = m_vecCell[dwIndex]->GetNeighbor(eNeighbor);
		if (pNeighbor == NULL)	//라인을 통과했는데 없다.
		{
			CLine2D* pPassLine = m_vecCell[dwIndex]->GetPassLine(pEye, &D3DXVECTOR3(0.f, 0.f, 0.f));
			D3DXVECTOR2 vStartLinePos	= pPassLine->GetStartPos();
			D3DXVECTOR2 vEndLinePos		= pPassLine->GetEndPos();
			D3DXVECTOR3 vEyeTemp		= D3DXVECTOR3(pEye->x, pEye->y - fCamY - 2.f, pEye->z);

			// 기울기
			float fLineGradiant = (vEndLinePos.y - vStartLinePos.y) / (vEndLinePos.x - vStartLinePos.x);
			// Y절편
			float fYIntercept	= vStartLinePos.y - (fLineGradiant * vStartLinePos.x);

			// y = fLineGradiant * x  +  fYIntercept

			// 카메라 위치부터 마지막 네비라인까지의 거리
			// d = |ax + by + c| / sqrt(a * a + b * b)
			float fDistCamToPassLine = abs((fLineGradiant * pEye->x) + pEye->z + fYIntercept) / sqrt((fLineGradiant * fLineGradiant) + 1);

			if (fDistCamToPassLine > fCamDistance)
			{
				GetNaviIndex(pEye, dwNextIndex);
				return dwNextIndex;
			}

			// 카메라 거리만큼 다시 거리를 줄인다
			vEyeTemp	+= ((*pCamDir) * (-1)) * (fDistCamToPassLine + 2.f);
			vEyeTemp.y	+= /*fCamY*/ + 2.f;

			*pEye = vEyeTemp;

			GetNaviIndex(pEye, dwNextIndex);
		}
		else
		{
			dwNextIndex = pNeighbor->GetIndex();
		}
	}
	else //라인을 지나가지 않았다.
	{
		//처리할 필요가 없다.
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
		return FALSE;
	}
	else
	{
		Index = iIndex;
		return TRUE;
	}
}