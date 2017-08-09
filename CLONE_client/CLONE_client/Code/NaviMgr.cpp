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
	: m_pShader(NULL)
	, m_pCam(NULL)
{
	for (int i = 0; i < MAP_END; ++i)
	{
		m_pBuffer[i] = NULL;
		m_dwIdxCnt[i] = 0;
	}
}

CNaviMgr::~CNaviMgr(void)
{
	Release();
}

void CNaviMgr::Reserve_CellContainerSize(CDevice* pDevice, const DWORD& dwSize, eMapType eMapType)
{
	if (!m_vecCell[eMapType].empty())
	{
		for (auto i = m_vecCell[eMapType].begin(); i != m_vecCell[eMapType].end(); ++i)
			Safe_Delete(*i);
		m_vecCell[eMapType].clear();
	}

	m_pDevice = pDevice;
	m_vecCell[eMapType].reserve(dwSize);

	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Line"));

	m_matWorld = XMMatrixIdentity();
}

HRESULT CNaviMgr::AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, eMapType eMapType)
{
	CNaviCell*		pCell = CNaviCell::Create(m_pDevice, pPointA, pPointB, pPointC, m_dwIdxCnt[eMapType]);
	NULL_CHECK_RETURN(pCell, E_FAIL);

	++m_dwIdxCnt[eMapType];

	m_vecCell[eMapType].push_back(pCell);

	return S_OK;
}

void CNaviMgr::LinkCell(void)
{
	for (int i = 0; i < MAP_END; ++i)
	{
		VECCELL::iterator		iter = m_vecCell[i].begin();
		if (iter == m_vecCell[i].end())
			return;

		for (; iter != m_vecCell[i].end(); ++iter)
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

void CNaviMgr::Render_NaviMesh(eMapType eMapType)
{
	if (m_pBuffer[eMapType] == NULL)
	{
		m_pBuffer[eMapType] = CLineBuffer::Create(m_pDevice, &m_vecCell[eMapType]);
		NULL_CHECK(m_pBuffer[eMapType]);
	}

	XMFLOAT4X4 matView, matProj, matWorld;
	memcpy(&matView, ((CCamera*)m_pCam)->GetViewMatrix(), sizeof(float) * 16);
	memcpy(&matProj, ((CCamera*)m_pCam)->GetProjMatrix(), sizeof(float) * 16);
	XMStoreFloat4x4(&matWorld, m_matWorld);

	((CLineShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer[eMapType]);

	m_pBuffer[eMapType]->Render_Lines();
}

void CNaviMgr::Release(void)
{
	for (int i = 0; i < MAP_END; ++i)
	{
		Safe_Delete(m_pBuffer[i]);

		for_each(m_vecCell[i].begin(), m_vecCell[i].end(), CDeleteObj());
		m_vecCell[i].clear();
	}
}

DWORD CNaviMgr::MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwIndex, eMapType eMapType)
{
	CNaviCell::NEIGHBOR		eNeighbor;
	DWORD					dwNextIndex = dwIndex;

	//라인을 통과 했다.
	if (m_vecCell[eMapType][dwIndex]->CheckPass(pPos, pDir, &eNeighbor))
	{
		const CNaviCell*		pNeighbor = m_vecCell[eMapType][dwIndex]->GetNeighbor(eNeighbor);
		if (pNeighbor == NULL)
		{
			//슬라이딩 벡터
			CLine2D* pPassLine = m_vecCell[eMapType][dwIndex]->GetPassLine(pPos, pDir);
			D3DXVECTOR2 vLineNormal = pPassLine->GetLineNormal();
			D3DXVECTOR3 vNormal = D3DXVECTOR3(vLineNormal.x, 0.f, vLineNormal.y);
			D3DXVECTOR3 vSliding = *pDir - D3DXVec3Dot(pDir, &vNormal) * vNormal;
			D3DXVec3Normalize(&vSliding, &vSliding);

			//슬라이딩 벡터로 이동
			*pPos += vSliding * CTimeMgr::GetInstance()->GetTime();
			//밀어준다
			D3DXMATRIX matRot;
			D3DXMatrixIdentity(&matRot);
			//벡터 사이각 구하기
			float fAngle = D3DXVec3Dot(pDir, &vNormal);
			if (fAngle > 0.f)
				D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
			else
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-180.f));
			D3DXVec3TransformCoord(&vNormal, &vNormal, &matRot);
			*pPos += vNormal * CTimeMgr::GetInstance()->GetTime();

			// 밖으로 나가는것 방지
			D3DXVECTOR3 vPosTemp = *pPos;
			
			if (vPosTemp.x < -HALFMAPX + 1.f || vPosTemp.x > HALFMAPX - 1.f
				|| vPosTemp.z < -HALFMAPZ + 1.f || vPosTemp.z > HALFMAPZ - 1.f)
				*pPos -= vSliding * CTimeMgr::GetInstance()->GetTime();
			
			GetNaviIndex(pPos, dwNextIndex, eMapType);
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
	const D3DXVECTOR3* pCamDir, float fCamDistance, float fCamY, const DWORD& dwIndex, eMapType eMapType)
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
	if (m_vecCell[eMapType][dwIndex]->CheckPass(pEye, &D3DXVECTOR3(0.f, 0.f, 0.f), &eNeighbor))
	{
		const CNaviCell*		pNeighbor = m_vecCell[eMapType][dwIndex]->GetNeighbor(eNeighbor);
		if (pNeighbor == NULL)	//라인을 통과했는데 없다.
		{
			CLine2D* pPassLine = m_vecCell[eMapType][dwIndex]->GetPassLine(pEye, &D3DXVECTOR3(0.f, 0.f, 0.f));
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
				GetNaviIndex(pEye, dwNextIndex, eMapType);
				return dwNextIndex;
			}

			// 카메라 거리만큼 다시 거리를 줄인다
			vEyeTemp	+= ((*pCamDir) * (-1)) * (fDistCamToPassLine + 2.f);
			vEyeTemp.y	+= /*fCamY*/ + 2.f;

			*pEye = vEyeTemp;

			GetNaviIndex(pEye, dwNextIndex, eMapType);
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

bool CNaviMgr::GetNaviIndex(D3DXVECTOR3 * pPos, DWORD & Index, eMapType eMapType)
{
	int iIndex = -1;

	CNaviCell::NEIGHBOR      eNeighbor;

	size_t iSize = m_vecCell[eMapType].size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecCell[eMapType][i]->CheckPass(pPos, &D3DXVECTOR3(0.f, 0.f, 0.f), &eNeighbor) == false)
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