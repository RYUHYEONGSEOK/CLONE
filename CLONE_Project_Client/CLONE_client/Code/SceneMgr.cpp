#include "stdafx.h"
#include "SceneMgr.h"
//Manager
#include "GraphicDevice.h"
#include "ResourceMgr.h"
#include "Renderer.h"
//Scene
#include "Logo.h"
#include "Custom.h"
#include "Stage.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
	: m_pDevice(NULL)
	, m_pRenderer(NULL)
	, m_pScene(NULL)
	, m_eCurScene(SCENE_ID_END)
	, m_bSceneLoadingEnd(false)

{
}

CSceneMgr::~CSceneMgr()
{
	Release();
}

HRESULT CSceneMgr::SetScene(SceneID eSceneID)
{
	if (m_pScene)
		Safe_Delete(m_pScene);

	CResourceMgr::GetInstance()->Release_Dynamic();
	m_bSceneLoadingEnd = false;

	m_eCurScene = eSceneID;
	m_pRenderer->SetScene(m_pScene);

	m_pScene = CLogo::Create(CDevice::GetInstance());
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	m_pRenderer->SetScene(m_pScene);

	return S_OK;
}

HRESULT CSceneMgr::SetSceneAftherLoading(SceneID eSceneID)
{
	if (m_pScene)
		Safe_Delete(m_pScene);

	m_eCurScene = eSceneID;

	switch (m_eCurScene)
	{
	case SCENE_ID_LOGO:		
		m_pScene = CLogo::Create(CDevice::GetInstance());
		m_pRenderer->m_bDeferredStage = false;
		break;
	case SCENE_ID_CUSTOM:
		m_pScene = CCustom::Create(CDevice::GetInstance());
		m_pRenderer->m_bDeferredStage = false;
		break;
	case SCENE_ID_STAGE:	
		m_pScene = CStage::Create(CDevice::GetInstance());
		m_pRenderer->m_bDeferredStage = true; 
		break;
	default:				
		m_pScene = NULL;						
		break;
	}

	NULL_CHECK_RETURN(m_pScene, E_FAIL);
	m_pRenderer->SetScene(m_pScene);

	return S_OK;
}

void CSceneMgr::Add_RenderGroup(RENDERGROUP eType, CObj * pObj, AddGroupType eGroupType /*= ADD_GROUP_BACK*/)
{
	if (eGroupType = ADD_GROUP_BACK)
		m_pRenderer->AddRenderGroup(eType, pObj);
	else if (eGroupType = ADD_GROUP_FRONT)
		m_pRenderer->AddRenderGroupFront(eType, pObj);
}

void CSceneMgr::Clear_RenderGroup(void)
{
	if (m_pRenderer != NULL)
		m_pRenderer->ClearRenderGroup();
}

HRESULT CSceneMgr::Init(CDevice * pDevice)
{
	m_pDevice = pDevice;
	m_pRenderer = CRenderer::Create(CDevice::GetInstance());

	return S_OK;
}

void CSceneMgr::Update()
{
	if (m_pScene != NULL)
		m_pScene->Update();
}

void CSceneMgr::Render(const float& fTime)
{
	if (m_pRenderer != NULL)
		m_pRenderer->Render(fTime);
}

void CSceneMgr::Release()
{
	Safe_Delete(m_pScene);
	Safe_Delete(m_pRenderer);
}

