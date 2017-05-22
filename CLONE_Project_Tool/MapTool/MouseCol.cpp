#include "stdafx.h"
#include "MouseCol.h"
#include "Camera.h"
#include "Back.h"
#include "Info.h"


CMouseCol::CMouseCol()
	: m_DyCam(NULL)
	, m_iCntX(0)
	, m_iCntZ(0)
{
}


CMouseCol::~CMouseCol()
{
}


void CMouseCol::Translation_ViewSpace(void)
{
	POINT ptMouse = GetMousePos();

	D3DXVECTOR3		vTemp;
	D3DXMATRIX m_matProj = m_DyCam->GetProj();

	vTemp.x = ((float(ptMouse.x) / (WINCX >> 1)) - 1.f) / m_matProj._11;
	vTemp.y = ((float(-ptMouse.y) / (WINCY >> 1)) + 1.f) / m_matProj._22;
	vTemp.z = 1.f;

	m_vPivotPos = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_vRayDir = vTemp - m_vPivotPos;

	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
	
}

bool CMouseCol::Translation_ViewSpace2(void)
{
	POINT	ptMouse = GetMousePos();

	if (ptMouse.x < 0 || ptMouse.y > 600)
		return false;

	D3DXMATRIX	matProj;
	matProj = CCamera::GetInstance()->m_matProj;
	D3DXVECTOR3	vTemp;

	// 뷰 포트 영역에서 투영 역으로 마우스 변환
	vTemp.x = ((float(ptMouse.x) / (WINCX >> 1)) - 1.f) / matProj._11;
	vTemp.y = ((float(-ptMouse.y) / (WINCY >> 1)) + 1.f) / matProj._22;
	vTemp.z = 1.f;

	m_vPivotPos.x = 0.f;
	m_vPivotPos.y = 0.f;
	m_vPivotPos.z = 0.f;

	m_vRayDir = vTemp - m_vPivotPos;

	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);

	return true;
}

void CMouseCol::Translation_Local(const D3DXMATRIX * pmatWorld)
{
	D3DXMATRIX m_matView = CCamera::GetInstance()->m_matView;
	D3DXMatrixInverse(&m_matView, 0, &m_matView);
	D3DXVec3TransformCoord(&m_vPivotPos, &m_vPivotPos, &m_matView);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &m_matView);

	D3DXMATRIX		matWorld;
	D3DXMatrixInverse(&matWorld, 0, pmatWorld);
	D3DXVec3TransformCoord(&m_vPivotPos, &m_vPivotPos, &matWorld);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matWorld);
}

bool CMouseCol::PickObject(CInfo* m_pInfo, D3DXVECTOR3* pOut, vector<VTXTEX>* pVertex, int iCnt)
{
	if (Translation_ViewSpace2() == false)
	{
		return false;
	}

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	Translation_Local(&matIdentity);


	D3DXMATRIX	matObjScale, matObjRotX, matObjRotY, matObjRotZ, matObjTrans, matObjWorld, matSourcePos;

	//스자이공
	D3DXMatrixScaling(&matObjScale, m_pInfo->m_vScale.x, m_pInfo->m_vScale.y, m_pInfo->m_vScale.z);
	D3DXMatrixRotationX(&matObjRotX, m_pInfo->m_fAngle[ANGLE_X]);
	D3DXMatrixRotationY(&matObjRotY, m_pInfo->m_fAngle[ANGLE_Y]);
	D3DXMatrixRotationZ(&matObjRotZ, m_pInfo->m_fAngle[ANGLE_Z]);
	D3DXMatrixTranslation(&matObjTrans, m_pInfo->m_vPos.x, m_pInfo->m_vPos.y, m_pInfo->m_vPos.z);

	matObjWorld = matObjScale * matObjRotX * matObjRotY * matObjRotZ * matObjTrans;
	vector<D3DXVECTOR3>		vVertexPos;

	vector<VTXTEX>::iterator vec_iter = pVertex->begin();
	vector<VTXTEX>::iterator vec_iterend = pVertex->end();
	
	
	for (; vec_iter != vec_iterend; ++vec_iter)
	{
		/*D3DXVECTOR3		vResultPos;

		D3DXMatrixTranslation(&matSourcePos, (*vec_iter).vPos.x, (*vec_iter).vPos.y, (*vec_iter).vPos.z);
		D3DXMATRIX matResult = matSourcePos * matObjWorld;
		vResultPos.x = matResult._41;
		vResultPos.y = matResult._42;
		vResultPos.z = matResult._43;
		vVertexPos.push_back(vResultPos);*/

		D3DXVECTOR3		vResultPos((*vec_iter).vPos.x, (*vec_iter).vPos.y, (*vec_iter).vPos.z);

		D3DXVec3TransformCoord(&vResultPos, &vResultPos, &matObjWorld);
		vVertexPos.push_back(vResultPos);

		//cout << "충돌된 위치 x: " << vResultPos.x << " y: " << vResultPos.y << " z: " << vResultPos.z << endl;
	}

	//pOut->x = D3DXToRadian(-90);
	
	
	float	fU, fV, fDist;

	for (int z = 0; z < iCnt - 1; ++z)
	{
		for (int x = 0; x < iCnt - 1; ++x)
		{
			int		iIndex = z * iCnt + x;

			if (iIndex + iCnt + 1 >= vVertexPos.size() || iIndex + iCnt >= vVertexPos.size() || iIndex + 1 >= vVertexPos.size())
				return false;

			// 오른쪽 위 폴리곤
			if (D3DXIntersectTri(&vVertexPos.at(iIndex + iCnt + 1),
				&vVertexPos.at(iIndex + iCnt),
				&vVertexPos.at(iIndex + 1),
				&m_vPivotPos, &m_vRayDir,
				&fU, &fV, &fDist))
			{
				*pOut = vVertexPos.at(iIndex + iCnt + 1) +
					(vVertexPos.at(iIndex + iCnt) - vVertexPos.at(iIndex + iCnt + 1)) * fU +
					(vVertexPos.at(iIndex + 1) - vVertexPos.at(iIndex + iCnt + 1)) * fV;
				
				return true;
			}

			// 
			if (D3DXIntersectTri(&vVertexPos.at(iIndex),
				&vVertexPos.at(iIndex + iCnt),
				&vVertexPos.at(iIndex + 1),
				&m_vPivotPos, &m_vRayDir,
				&fU, &fV, &fDist))
			{
				*pOut = vVertexPos.at(iIndex) +
					(vVertexPos.at(iIndex + 1) - vVertexPos.at(iIndex)) * fV +
					(vVertexPos.at(iIndex + iCnt) - vVertexPos.at(iIndex)) * fU;

				return true;
			}

		}
	}
	return false;
}


POINT CMouseCol::GetMousePos(void)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	return ptMouse;
}

HRESULT CMouseCol::Initialize()
{
	
	return S_OK;
}

