#include "stdafx.h"
#include "RenderTargetMgr.h"
//Manager
#include "GraphicDevice.h"
#include "ResourceMgr.h"
#include "SceneMgr.h"
//Object
#include "RenderTarget.h"
#include "Scene.h"
#include "TextureShader.h"
#include "Obj.h"

IMPLEMENT_SINGLETON(CRenderTargetMgr)

CRenderTargetMgr::CRenderTargetMgr(void)
{

}

CRenderTargetMgr::~CRenderTargetMgr(void)
{
	Release();
}

HRESULT CRenderTargetMgr::Add_RenderTarget(CDevice* pDevice, CObj* pCam, const TCHAR* pTargetTag, const UINT& iSizeX, const UINT& iSizeY, DXGI_FORMAT Format, D3DXCOLOR Color)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pTargetTag);
	NULL_CHECK_RETURN(!pRenderTarget, E_FAIL);

	pRenderTarget = CRenderTarget::Create(pDevice, pCam, iSizeX, iSizeY, Format, Color);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	m_mapTarget.insert(make_pair(pTargetTag, pRenderTarget));
	return S_OK;
}

HRESULT CRenderTargetMgr::Add_MRT(const TCHAR* pMRTTag, const TCHAR* pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	MAPMRTLIST::iterator	iter = find_if(m_mapMRTlist.begin(), m_mapMRTlist.end(), CTagFinder(pMRTTag));
	if (iter == m_mapMRTlist.end())
	{
		m_mapMRTlist[pMRTTag] = MRTLIST();
	}
	m_mapMRTlist[pMRTTag].push_back(pRenderTarget);
	return S_OK;
}

HRESULT CRenderTargetMgr::Add_DebugBuffer(const TCHAR* pTargetTag, const float& fSizeX, const float& fSizeY)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	return pRenderTarget->Create_DebugBuffer(fSizeX, fSizeY);
}

void CRenderTargetMgr::Render_DebugBuffer(const TCHAR* pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, );

	pRenderTarget->Draw_RenderTarget();
}

void CRenderTargetMgr::Render_LightTarget(const TCHAR * pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, );

	pRenderTarget->Draw_LightTarget();
}

ID3D11ShaderResourceView * CRenderTargetMgr::GetReflectionView(void)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(L"Target_Deferred");
	if (pRenderTarget == NULL)
		return NULL;

	return pRenderTarget->GetReflectionView();
}

ID3D11ShaderResourceView * CRenderTargetMgr::GetRefactionView(void)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(L"Target_Deferred");
	if (pRenderTarget == NULL)
		return NULL;

	return pRenderTarget->GetRefactionView();
}

void CRenderTargetMgr::Begin_MRT_Refraction(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);
	NULL_CHECK_RETURN(pMRTList, );

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Set_Target_Refraction();
	}
}

void CRenderTargetMgr::End_MRT_Refraction(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Relese_Target();
	}
}

void CRenderTargetMgr::Begin_MRT_Reflection(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);
	NULL_CHECK_RETURN(pMRTList, );

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Set_Target_Reflaction();
	}
}

void CRenderTargetMgr::End_MRT_Reflection(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Relese_Target();
	}
}

void CRenderTargetMgr::Begin_MRT_Defferd(const TCHAR* pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);
	NULL_CHECK_RETURN(pMRTList, );

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Clear_Target();
	}

	iter = pMRTList->begin();
	for (; iter != iter_end; ++iter)
	{
		(*iter)->Set_Target_Deferred();
	}
}

void CRenderTargetMgr::End_MRT_Defferd(const TCHAR* pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Relese_Target();
	}
}

void CRenderTargetMgr::Begin_MRT_Shadow(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);
	NULL_CHECK_RETURN(pMRTList, );

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Set_Target_Shadow();
	}
}

void CRenderTargetMgr::End_MRT_Shadow(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Relese_Target();
	}
}

void CRenderTargetMgr::Begin_MRT_Light(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);
	NULL_CHECK_RETURN(pMRTList, );

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	//Clear는 Deferred 에서 이미 해주었으니 할 필요가 없다.
	for (; iter != iter_end; ++iter)
	{
		(*iter)->Set_Target_Light();
	}
}

void CRenderTargetMgr::End_MRT_Light(const TCHAR * pMRTTag)
{
	MRTLIST*	pMRTList = Find_MRT(pMRTTag);

	MRTLIST::iterator iter = pMRTList->begin();
	MRTLIST::iterator iter_end = pMRTList->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Relese_Target();
	}
}

CRenderTarget* CRenderTargetMgr::Find_RenderTarget(const TCHAR* pTargetTag)
{
	MAPTARGET::iterator iter = find_if(m_mapTarget.begin(), m_mapTarget.end(), CTagFinder(pTargetTag));
	if (iter == m_mapTarget.end())
		return NULL;

	return iter->second;
}

list<CRenderTarget*>* CRenderTargetMgr::Find_MRT(const TCHAR* pMRTTag)
{
	MAPMRTLIST::iterator	iter = find_if(m_mapMRTlist.begin(), m_mapMRTlist.end(), CTagFinder(pMRTTag));
	if (iter == m_mapMRTlist.end())
		return NULL;

	return &iter->second;
}

void CRenderTargetMgr::Release(void)
{
	for_each(m_mapTarget.begin(), m_mapTarget.end(), CDeleteMap());
	m_mapTarget.clear();
}

void CRenderTargetMgr::Set_ConstantTable(CBuffer* pBuffer
	, const TCHAR* pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, );

	pRenderTarget->Set_ConstantTable(pBuffer);
}

void CRenderTargetMgr::Set_Cam(const TCHAR * pTargetTag, CObj * pCam)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, );

	pRenderTarget->Set_Cam(pCam);
}