#include "stdafx.h"
#include "CollisionMgr.h"
//Manager
#include "SceneMgr.h"
//Object
#include "Obj.h"
#include "Mesh.h"
#include "Transform.h"
#include "Scene.h"
#include "Stage.h"
#include "StageUIFrame.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr(void)
	: m_pmapObjList(NULL)
	, m_pTerrainVtx(NULL)
{

}

CCollisionMgr::~CCollisionMgr(void)
{
	Release();
}

bool CCollisionMgr::Collision_OBB(const wstring& wTargetTag, const D3DXVECTOR3* pDestMin, const D3DXVECTOR3* pDestMax
	, const D3DXMATRIX* pDestWorld, const D3DXVECTOR3* pSourMin, const D3DXVECTOR3* pSourMax, const D3DXMATRIX* pSourWorld)
{
	ZeroMemory(m_OBB, sizeof(OBB) * 2);

	InitPoint(&m_OBB[0], pDestMin, pDestMax);
	InitPoint(&m_OBB[1], pSourMin, pSourMax);

	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_OBB[0].vPoint[i], &m_OBB[0].vPoint[i], pDestWorld);
		D3DXVec3TransformCoord(&m_OBB[1].vPoint[i], &m_OBB[1].vPoint[i], pSourWorld);
	}
	D3DXVec3TransformCoord(&m_OBB[0].vCenter, &m_OBB[0].vCenter, pDestWorld);
	D3DXVec3TransformCoord(&m_OBB[1].vCenter, &m_OBB[1].vCenter, pSourWorld);

	InitAxis(&m_OBB[0]);
	InitAxis(&m_OBB[1]);

	float		fDistance[3];
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&m_OBB[0].vProjAxis[0], &m_OBB[i].vAxis[j]))
				+ fabs(D3DXVec3Dot(&m_OBB[0].vProjAxis[1], &m_OBB[i].vAxis[j]))
				+ fabs(D3DXVec3Dot(&m_OBB[0].vProjAxis[2], &m_OBB[i].vAxis[j]));

			fDistance[1] = fabs(D3DXVec3Dot(&m_OBB[1].vProjAxis[0], &m_OBB[i].vAxis[j]))
				+ fabs(D3DXVec3Dot(&m_OBB[1].vProjAxis[1], &m_OBB[i].vAxis[j]))
				+ fabs(D3DXVec3Dot(&m_OBB[1].vProjAxis[2], &m_OBB[i].vAxis[j]));

			D3DXVECTOR3		vTemp = m_OBB[1].vCenter - m_OBB[0].vCenter;
			fDistance[2] = fabs(D3DXVec3Dot(&vTemp, &m_OBB[i].vAxis[j]));

			if (fDistance[2] > fDistance[1] + fDistance[0])
				return FALSE;
		}
	}
	return TRUE;
}

bool CCollisionMgr::Collision_AABB(const wstring& wTargetTag, CObj* pObj)
{
	unordered_map<wstring, list<CObj*>>::iterator	iterMap = m_pmapObjList->find(wTargetTag);

	if (iterMap == m_pmapObjList->end())
		return FALSE;
	
	D3DXVECTOR3		vSourMin, vSourMax;
	CMesh*	pTargetMesh			= NULL;
	CTransform*	pTargetInfo		= NULL;

	list<CObj*>::iterator iter = iterMap->second.begin();
	list<CObj*>::iterator iter_end = iterMap->second.end();

	for (iter; iter != iter_end; ++iter)
	{
		// 우선 타겟의 상태가 ATT가 아닐 시 return 시킨다.
		if ((*iter)->GetObjState() != CObj::OBJ_STATE_ATT)
			return FALSE;

		pTargetMesh = dynamic_cast<CMesh*>((*iter)->GetComponent(L"Mesh"));
		pTargetInfo = dynamic_cast<CTransform*>((*iter)->GetComponent(L"Transform"));
	}
	
	CMesh* pObjMesh		 = dynamic_cast<CMesh*>(pObj->GetComponent(L"Mesh"));
	CTransform* pObjInfo = dynamic_cast<CTransform*>(pObj->GetComponent(L"Transform"));

	// ATT 애니메이션에서도 일정 부분만 체크하도록 한다.
	// 60.f 가 임의의 애니메이션 플레이타임
	if (pTargetMesh->GetCurrentAniIdx() != CObj::OBJ_STATE_ATT || pTargetMesh->GetAniPlayTime() < 60.f)
		return FALSE;

	pObjMesh->GetMinMax(&vSourMin, &vSourMax);

	iter = iterMap->second.begin();
	iter_end = iterMap->second.end();

	for (iter; iter != iter_end; ++iter)
	{
		//Target Calc
		D3DXVECTOR3		vDestMin, vDestMax;

		pTargetMesh->GetMinMax(&vDestMin, &vDestMax);

		D3DXMATRIX matAABB;
		D3DXMatrixIdentity(&matAABB);
		memcpy(&matAABB.m[3][0], &pTargetInfo->m_matWorld.m[3][0], sizeof(D3DXVECTOR3));
		
		D3DXVec3TransformCoord(&vDestMin, &vDestMin, &matAABB);
		D3DXVec3TransformCoord(&vDestMax, &vDestMax, &matAABB);

		//This Obj Calc
		D3DXMatrixIdentity(&matAABB);
		memcpy(&matAABB.m[3][0], &pObjInfo->m_matWorld.m[3][0], sizeof(D3DXVECTOR3));

		//Translation
		matAABB.m[3][0] -= pObjInfo->m_vDir.x * 0.9f;
		matAABB.m[3][2] -= pObjInfo->m_vDir.z * 0.9f;
		//Scaling
		matAABB.m[2][2] = 1.5f;
		matAABB.m[1][1] = 1.5f;
		
		D3DXVec3TransformCoord(&vSourMin, &vSourMin, &matAABB);
		D3DXVec3TransformCoord(&vSourMax, &vSourMax, &matAABB);

		//Collision Checking ------------------------------------------------------------
		float		fMin = 0.f;
		float		fMax = 0.f;

		//x
		fMin = max(vDestMin.x, vSourMin.x);
		fMax = min(vDestMax.x, vSourMax.x);
		if (fMin > fMax)
			return FALSE;

		fMin = max(vDestMin.y, vSourMin.y);
		fMax = min(vDestMax.y, vSourMax.y);
		if (fMin > fMax)
			return FALSE;

		fMin = max(vDestMin.z, vSourMin.z);
		fMax = min(vDestMax.z, vSourMax.z);
		if (fMin > fMax)
			return FALSE;

		//여기부턴 충돌이 된것임
		//킬로그 넣기
		//CObj* pStageUI = ((CStage*)CSceneMgr::GetInstance()->GetScene())->GetStageUI();
		//((CStageUIFrame*)pStageUI)->AddKillLog((*iter)->GetObjID(), pObj->GetObjID());

		return TRUE;
	}

	return false;
}

void CCollisionMgr::InitPoint(OBB* pOBB, const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax)
{
	pOBB->vPoint[0] = D3DXVECTOR3(pMin->x, pMax->y, pMin->z);
	pOBB->vPoint[1] = D3DXVECTOR3(pMax->x, pMax->y, pMin->z);
	pOBB->vPoint[2] = D3DXVECTOR3(pMax->x, pMin->y, pMin->z);
	pOBB->vPoint[3] = D3DXVECTOR3(pMin->x, pMin->y, pMin->z);

	pOBB->vPoint[4] = D3DXVECTOR3(pMin->x, pMax->y, pMax->z);
	pOBB->vPoint[5] = D3DXVECTOR3(pMax->x, pMax->y, pMax->z);
	pOBB->vPoint[6] = D3DXVECTOR3(pMax->x, pMin->y, pMax->z);
	pOBB->vPoint[7] = D3DXVECTOR3(pMin->x, pMin->y, pMax->z);

	for (int i = 0; i < 8; ++i)
		pOBB->vCenter += pOBB->vPoint[i];

	pOBB->vCenter *= 0.125f;
}

void CCollisionMgr::InitAxis(OBB* pOBB)
{
	/*
	4---5
	0/--1/|
	| 7	| 6
	3---2/
	*/
	//x
	pOBB->vProjAxis[0] = (pOBB->vPoint[1] + pOBB->vPoint[2] + pOBB->vPoint[5] + pOBB->vPoint[6]) * 0.25f - pOBB->vCenter;
	pOBB->vAxis[0] = pOBB->vPoint[1] - pOBB->vPoint[0];

	//y
	pOBB->vProjAxis[1] = (pOBB->vPoint[0] + pOBB->vPoint[1] + pOBB->vPoint[4] + pOBB->vPoint[5]) * 0.25f - pOBB->vCenter;
	pOBB->vAxis[1] = pOBB->vPoint[0] - pOBB->vPoint[3];

	//z
	pOBB->vProjAxis[2] = (pOBB->vPoint[4] + pOBB->vPoint[5] + pOBB->vPoint[6] + pOBB->vPoint[7]) * 0.25f - pOBB->vCenter;
	pOBB->vAxis[2] = pOBB->vPoint[7] - pOBB->vPoint[3];

	for (int i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pOBB->vAxis[i], &pOBB->vAxis[i]);
}

void CCollisionMgr::Collision_Terrain(CObj * pObj)
{
	CTransform* pObjInfo = dynamic_cast<CTransform*>(pObj->GetComponent(L"Transform"));
	D3DXVECTOR3	vPos = pObjInfo->m_vPos;

	int		iIndex = (int(vPos.z) / VTXITV) * VTXCNTX + (int(vPos.x) / VTXITV);

	float	fRatioX = (vPos.x - m_pTerrainVtx[iIndex + VTXCNTX].Pos.x) / VTXITV;
	float	fRatioZ = (m_pTerrainVtx[iIndex + VTXCNTX].Pos.z - vPos.z) / VTXITV;

	XMVECTOR	Plane;
	XMFLOAT4	Float4Plane;

	if (fRatioX > fRatioZ)	// 오른쪽 위
	{
		Plane = XMPlaneFromPoints(XMLoadFloat3(&m_pTerrainVtx[iIndex + VTXCNTX].Pos)
			, XMLoadFloat3(&m_pTerrainVtx[iIndex + VTXCNTX + 1].Pos)
			, XMLoadFloat3(&m_pTerrainVtx[iIndex + 1].Pos));

		XMStoreFloat4(&Float4Plane, Plane);
	}
	else // 왼쪽 아래
	{
		Plane = XMPlaneFromPoints(XMLoadFloat3(&m_pTerrainVtx[iIndex + VTXCNTX].Pos)
			, XMLoadFloat3(&m_pTerrainVtx[iIndex + 1].Pos)
			, XMLoadFloat3(&m_pTerrainVtx[iIndex].Pos));

		XMStoreFloat4(&Float4Plane, Plane);
	}
	pObjInfo->m_vPos.y = (-Float4Plane.x * vPos.x - Float4Plane.z * vPos.z - Float4Plane.w) / Float4Plane.y;
	pObjInfo->m_vPos.y += 1.f;
}


void CCollisionMgr::Release(void)
{

}

bool CCollisionMgr::Collision_AABB_Bead(const wstring & wTargetTag, CObj * pObj)
{
	unordered_map<wstring, list<CObj*>>::iterator	iterMap = m_pmapObjList->find(wTargetTag);

	if (iterMap == m_pmapObjList->end())
		return FALSE;

	D3DXVECTOR3		vSourMin, vSourMax;
	CMesh* pObjMesh = dynamic_cast<CMesh*>(pObj->GetComponent(L"Mesh"));
	CTransform* pObjInfo = dynamic_cast<CTransform*>(pObj->GetComponent(L"Transform"));

	pObjMesh->GetMinMax(&vSourMin, &vSourMax);

	list<CObj*>::iterator iter = iterMap->second.begin();
	list<CObj*>::iterator iter_end = iterMap->second.end();

	for (iter; iter != iter_end; ++iter)
	{
		//Target Calc
		D3DXVECTOR3		vDestMin, vDestMax;

		CMesh*	pTargetMesh = dynamic_cast<CMesh*>((*iter)->GetComponent(L"Mesh"));
		CTransform*	pTargetInfo = dynamic_cast<CTransform*>((*iter)->GetComponent(L"Transform"));

		pTargetMesh->GetMinMax(&vDestMin, &vDestMax);

		D3DXMATRIX matAABB;
		D3DXMatrixIdentity(&matAABB);
		memcpy(&matAABB.m[3][0], &pTargetInfo->m_matWorld.m[3][0], sizeof(D3DXVECTOR3));

		D3DXVec3TransformCoord(&vDestMin, &vDestMin, &matAABB);
		D3DXVec3TransformCoord(&vDestMax, &vDestMax, &matAABB);

		//This Obj Calc
		D3DXMatrixIdentity(&matAABB);
		memcpy(&matAABB.m[3][0], &pObjInfo->m_matWorld.m[3][0], sizeof(D3DXVECTOR3));

		D3DXVec3TransformCoord(&vSourMin, &vSourMin, &matAABB);
		D3DXVec3TransformCoord(&vSourMax, &vSourMax, &matAABB);

		//Collision Checking ------------------------------------------------------------
		float		fMin = 0.f;
		float		fMax = 0.f;

		//x
		fMin = max(vDestMin.x, vSourMin.x);
		fMax = min(vDestMax.x, vSourMax.x);
		if (fMin > fMax)
			return FALSE;

		fMin = max(vDestMin.y, vSourMin.y);
		fMax = min(vDestMax.y, vSourMax.y);
		if (fMin > fMax)
			return FALSE;

		fMin = max(vDestMin.z, vSourMin.z);
		fMax = min(vDestMax.z, vSourMax.z);
		if (fMin > fMax)
			return FALSE;

		//여기부턴 충돌이 된것임
		//킬로그에 넣는 부분
		//CObj* pStageUI = ((CStage*)CSceneMgr::GetInstance()->GetScene())->GetStageUI();
		//((CStageUIFrame*)pStageUI)->AddKillLog((*iter)->GetObjID(), pObj->GetObjID());

		return TRUE;
	}

	return false;
}
