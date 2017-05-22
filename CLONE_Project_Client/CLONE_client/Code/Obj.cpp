#include "stdafx.h"
#include "Obj.h"

#include "Component.h"
#include "Camera.h"

CObj::CObj(CDevice * pDevice)
	: m_pDevice(pDevice)
	, m_eObjState(OBJ_STATE_END)
	, m_eObjType(OBJ_END)
	, m_eTeamType(TEAM_END)
	, m_pInfo(NULL)
	, m_pBuffer(NULL)
	, m_pCam(NULL)
	, m_pShader(NULL)
	, m_pBoundingBoxShader(NULL)
	, m_pDepthShader(NULL)
	, m_pTexture(NULL)
	, m_pLightInfo(NULL)
	, m_fViewZ(0.f)
	, m_bCollision(false)
	, m_eRenderType(RENDERTYPE_END)
{
	
}

CObj::~CObj()
{
	Release_Component();
}

CComponent * CObj::GetComponent(const wstring & _wstrComponentKey)
{
	unordered_map<wstring, CComponent*>::iterator iter = m_mapComponent.find(_wstrComponentKey);

	if (iter == m_mapComponent.end())
		return NULL;

	return iter->second;
}

void CObj::Update_Component()
{
	unordered_map<wstring, CComponent*>::iterator iter = m_mapComponent.begin();
	unordered_map<wstring, CComponent*>::iterator iter_end = m_mapComponent.end();

	for (; iter != iter_end; ++iter)
		iter->second->Update();
}

void CObj::SetViewProjTransform()
{
	if (m_pCam != NULL)
	{
		memcpy(&m_matView, ((CCamera*)m_pCam)->GetViewMatrix(), sizeof(float) * 16);
		memcpy(&m_matProj, ((CCamera*)m_pCam)->GetProjMatrix(), sizeof(float) * 16);
	}
}

const float & CObj::GetViewZ(void) const
{
	return m_fViewZ;
}

void CObj::Compute_ViewZ(const D3DXVECTOR3 * pPos)	//알파값이 필요한 녀석들만 계산한다!
{
	XMMATRIX		matView;
	matView = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_matView));

	D3DXVECTOR3		vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(D3DXVECTOR3));

	float		fDistance = D3DXVec3Length(&(vCamPos - *pPos));

	m_fViewZ = fDistance;
}

void CObj::Release_Component()
{
	for_each(m_mapComponent.begin(), m_mapComponent.end(), CDeleteMap());
	m_mapComponent.clear();
}
