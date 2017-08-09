#include "stdafx.h"
#include "Frustum.h"
//Objact
#include "Camera.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum(void)
{

}

CFrustum::~CFrustum(void)
{

}

HRESULT CFrustum::UpdateFrustum(CObj* pCam)
{
	D3DXMATRIX viewProjection;
	D3DXMatrixMultiply(&viewProjection, &(*((CCamera*)pCam)->GetViewMatrix()), &(*((CCamera*)pCam)->GetProjMatrix()));

	// Left plane
	m_Plane[0].a = viewProjection._14 + viewProjection._11;
	m_Plane[0].b = viewProjection._24 + viewProjection._21;
	m_Plane[0].c = viewProjection._34 + viewProjection._31;
	m_Plane[0].d = viewProjection._44 + viewProjection._41;

	// Right plane
	m_Plane[1].a = viewProjection._14 - viewProjection._11;
	m_Plane[1].b = viewProjection._24 - viewProjection._21;
	m_Plane[1].c = viewProjection._34 - viewProjection._31;
	m_Plane[1].d = viewProjection._44 - viewProjection._41;

	// Top plane
	m_Plane[2].a = viewProjection._14 - viewProjection._12;
	m_Plane[2].b = viewProjection._24 - viewProjection._22;
	m_Plane[2].c = viewProjection._34 - viewProjection._32;
	m_Plane[2].d = viewProjection._44 - viewProjection._42;

	// Bottom plane
	m_Plane[3].a = viewProjection._14 + viewProjection._12;
	m_Plane[3].b = viewProjection._24 + viewProjection._22;
	m_Plane[3].c = viewProjection._34 + viewProjection._32;
	m_Plane[3].d = viewProjection._44 + viewProjection._42;

	// Near plane
	m_Plane[4].a = viewProjection._13;
	m_Plane[4].b = viewProjection._23;
	m_Plane[4].c = viewProjection._33;
	m_Plane[4].d = viewProjection._43;

	// Far plane
	m_Plane[5].a = viewProjection._14 - viewProjection._13;
	m_Plane[5].b = viewProjection._24 - viewProjection._23;
	m_Plane[5].c = viewProjection._34 - viewProjection._33;
	m_Plane[5].d = viewProjection._44 - viewProjection._43;

	// Normalize planes
	for (int i = 0; i < 6; i++)
	{
		D3DXPlaneNormalize(&m_Plane[i], &m_Plane[i]);
	}

	return S_OK;
}

bool CFrustum::SphereInFrustum(const D3DXVECTOR3* pCenter, const float& fRadius)
{
	for (int i = 0; i < 6; ++i)
	{
		if (D3DXPlaneDotCoord(&m_Plane[i], pCenter) + fRadius < 0)
			return false;
	}
	return true;
}

