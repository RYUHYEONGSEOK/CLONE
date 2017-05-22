#include "stdafx.h"
#include "LightMgr.h"
//Manager
#include "ResourceMgr.h"
//Object
#include "Light.h"

IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr(void)
{

}

CLightMgr::~CLightMgr(void)
{
	Release();
}

LightInfo* CLightMgr::GetLightInfo(const UINT& iIndex)
{
	LIGHTLIST::iterator		iter = m_Lightlist.begin();
	for (size_t i = 0; i < iIndex; ++i)
	{
		++iter;
	}
	return (*iter)->GetLightInfo();
}

HRESULT CLightMgr::AddLight(CDevice* pDevice, const LightInfo* pLightInfo, const WORD& wLightIdx, CObj* pCam)
{
	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	CLight*			pLight = CLight::Create(pDevice, pLightInfo, wLightIdx);
	NULL_CHECK_RETURN(pLight, E_FAIL);
	pLight->SetCam(pCam);	//Light_Render를 위해 Cam정보 필요

	m_Lightlist.push_back(pLight);
	return S_OK;
}

void CLightMgr::Render_Light(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj, 
	XMFLOAT4X4 matViewInv, XMFLOAT4X4 matProjInv, 
	ID3D11ShaderResourceView* pNormalResource, 
	ID3D11ShaderResourceView* pDepthResource , 
	ID3D11ShaderResourceView* pShadowResource)
{
	LightMatrixStruct tStruct;

	LIGHTLIST::iterator iter = m_Lightlist.begin();
	LIGHTLIST::iterator iter_end = m_Lightlist.end();

	// LightMatrix 찾기
	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetLightInfo()->eLightType == LIGHT_SPOT)
			tStruct = (*iter)->GetLightMatrix();
	}

	// iter 초기화
	iter = m_Lightlist.begin();
	iter_end = m_Lightlist.end();
	
	for (; iter != iter_end; ++iter)
	{
		if (!((*iter)->GetLightInfo()->eLightType == LIGHT_SPOT))
		{
			(*iter)->Render_Light(matWorld, matView, matProj, matViewInv, matProjInv, tStruct,
				pNormalResource, pDepthResource, pShadowResource);
		}
	}
}

LightMatrixStruct CLightMgr::GetLightMatrix(void)
{
	LightMatrixStruct tStruct;

	LIGHTLIST::iterator iter = m_Lightlist.begin();
	LIGHTLIST::iterator iter_end = m_Lightlist.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetLightInfo()->eLightType == LIGHT_SPOT)
			return (*iter)->GetLightMatrix();
	}

	return tStruct;
}

void CLightMgr::Release(void)
{
	for_each(m_Lightlist.begin(), m_Lightlist.end(), CDeleteObj());
	m_Lightlist.clear();
}