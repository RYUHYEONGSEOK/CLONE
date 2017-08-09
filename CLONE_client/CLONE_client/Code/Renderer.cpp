#include "stdafx.h"
#include "Renderer.h"

#include "GraphicDevice.h"
#include "TimeMgr.h"
#include "RenderTargetMgr.h"
#include "ResourceMgr.h"
#include "LightMgr.h"

#include "Obj.h"
#include "Camera.h"
#include "Scene.h"
#include "VIBuffer.h"
#include "RenderTex.h"
//#include "BlendTex.h"
#include "BlendShader.h"

float CRenderer::m_fTimer = 0.f;
DWORD CRenderer::m_dwFPSCnt = 0;

CRenderer::CRenderer(CDevice * pDevice)
	: m_pDevice(pDevice)
	, m_pScene(NULL)
	, m_pScreenBuffer(NULL)
	, m_bDeferredStage(false)
{
	ZeroMemory(m_szFps, sizeof(TCHAR) * 128);
}

CRenderer::~CRenderer(void)
{
	Release();
}

bool Compare_ViewZ(CObj * pSour, CObj * pDest)
{
	return pSour->GetViewZ() > pDest->GetViewZ();
}

CRenderer * CRenderer::Create(CDevice * pDevice)
{
	CRenderer* pRenderer = new CRenderer(pDevice);

	if (FAILED(pRenderer->Init()))
		Safe_Delete(pRenderer);

	return pRenderer;
}

void CRenderer::AddRenderGroup(RENDERGROUP eType, CObj * pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, );

	m_RenderGroup[eType].push_back(pGameObject);
}

void CRenderer::AddRenderGroupFront(RENDERGROUP eType, CObj * pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, );

	m_RenderGroup[eType].push_front(pGameObject);
}

void CRenderer::RemoveRenderGroup(RENDERGROUP eType, CObj * pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, );

	auto iter = m_RenderGroup[eType].begin();
	auto iter_end = m_RenderGroup[eType].end();

	for (iter; iter != iter_end; )
	{
		if ((*iter) == pGameObject)
		{
			m_RenderGroup[eType].erase(iter);
			break;
		}
		else
			++iter;
	}
}

void CRenderer::ClearRenderGroup(void)
{
	for (int i = 0; i < RENDERTYPE_END; ++i)
		m_RenderGroup[i].clear();
}

HRESULT CRenderer::Init()
{
	HRESULT hr = NULL;

	/*D3D11_VIEWPORT ViewPort;
	ZeroMemory(&ViewPort, sizeof(ViewPort));
	UINT numViewport = 1;
	m_pDevice->GetDeviceContext()->RSGetViewports(&numViewport, &ViewPort);*/

	m_pScreenBuffer = CRenderTex::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pScreenBuffer, E_FAIL);

	// 물을 위한 클리핑 평면
	m_vClipPlane = D3DXVECTOR4(0.f, -1.f, 0.f, WATER_HEIGHT + 0.1f);

	return S_OK;
}

void CRenderer::Render(const float& fTime)
{
	if (g_bDraw == TRUE)
	{
		m_pDevice->Render_Begin();

		if (m_bDeferredStage == TRUE)
		{
			// 물반사를 위한 선행 그리기
			Render_For_Refraction();
			Render_For_Reflection();

			// 그리기
			Render_Priority();
			//Render_NoneAlpha();		//논알파를 끄고 디퍼드와 블랜드로 그린다...
			//여기서 쉐도우맵을 한번 더 그린다.
			Render_Shadow();
			Render_Deferred();
			Render_Blend();
			Render_Alpha();
			Render_UI();

			if (m_pScene != NULL)
				m_pScene->Render();

			Render_DebugBuffer();
		}
		else
		{
			if (m_pScene != NULL)
				m_pScene->Render();
		}
	}

	Render_FPS(fTime);

	m_pDevice->Render_End();
}

void CRenderer::Render_Priority(void)
{
	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_PRIORITY].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_PRIORITY].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderer::Render_Shadow(void)
{
	CRenderTargetMgr::GetInstance()->Begin_MRT_Shadow(L"MRT_ALL");

	LightMatrixStruct tLightMatrix = CLightMgr::GetInstance()->GetLightMatrix();

	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_NONEALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_NONEALPHA].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->RenderForShadow(tLightMatrix);
	}

	CRenderTargetMgr::GetInstance()->End_MRT_Shadow(L"MRT_ALL");
}

void CRenderer::Render_Water(void)
{
	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_WATER].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_WATER].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderer::Render_For_Refraction(void) // 물 아래서 그려지는 물체들
{
	CRenderTargetMgr::GetInstance()->Begin_MRT_Refraction(L"MRT_ALL");

	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_NONEALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_NONEALPHA].end();

	CObj::ObjCullToRef eCheckType;

	for (; iter != iter_end; ++iter)
	{
		// 그릴 Obj를 추린다
		eCheckType = (*iter)->GetObjRenderRefType();

		if (eCheckType != CObj::OBJ_RENDER_REF_END)
			int i = 0;

		if (eCheckType == CObj::OBJ_RENDER_REF_UNDER || eCheckType == CObj::OBJ_RENDER_REF_BOTH)
		{
			// 반경 50.f 인지 검사로 연산 줄여놈
			if ((*iter)->CheckRenderForRef())
				(*iter)->RenderForWaterEffect(m_vClipPlane, CObj::REFTYPE_REFRACT);
		}
	}

	CRenderTargetMgr::GetInstance()->End_MRT_Refraction(L"MRT_ALL");
}

void CRenderer::Render_For_Reflection(void)	// 물 위에서 반사될 Obj들
{
	CRenderTargetMgr::GetInstance()->Begin_MRT_Reflection(L"MRT_ALL");

	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_NONEALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_NONEALPHA].end();

	CObj::ObjCullToRef eCheckType;

	for (; iter != iter_end; ++iter)
	{
		// 그릴 Obj를 추린다
		eCheckType = (*iter)->GetObjRenderRefType();
		if (eCheckType == CObj::OBJ_RENDER_REF_UPPER || eCheckType == CObj::OBJ_RENDER_REF_BOTH)
		{
			// 반경 40.f 인지 검사로 연산 줄여놈
			if ((*iter)->CheckRenderForRef())
				(*iter)->RenderForWaterEffect(m_vClipPlane, CObj::REFTYPE_REFLECT);
		}
	}

	CRenderTargetMgr::GetInstance()->End_MRT_Reflection(L"MRT_ALL");
}

void CRenderer::Render_NoneAlpha(void)
{
	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_NONEALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_NONEALPHA].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderer::Render_Alpha(void)
{
	m_RenderGroup[RENDERTYPE_ALPHA].sort(Compare_ViewZ);

	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_ALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_ALPHA].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderer::Render_UI(void)
{
	m_pDevice->Blend_Begin();
	m_pDevice->TurnZBufferOff();

	RENDERLIST::iterator	iter = m_RenderGroup[RENDERTYPE_UI].begin();
	RENDERLIST::iterator	iter_end = m_RenderGroup[RENDERTYPE_UI].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}

	m_pDevice->TurnZBufferOn();
	m_pDevice->Blend_End();
}

void CRenderer::Render_FPS(const float & fTime)
{
	m_fTimer += fTime;
	++m_dwFPSCnt;

	if (m_fTimer >= 1.f)
	{
		wsprintf(m_szFps, L"FPS : %d", m_dwFPSCnt);
		m_fTimer = 0.f;
		m_dwFPSCnt = 0;
	}
	::SetWindowText(g_hWnd, m_szFps);
}

void CRenderer::Render_Deferred(void)
{
	CRenderTargetMgr::GetInstance()->Begin_MRT_Defferd(L"MRT_ALL");

	Render_NoneAlpha();
	Render_Water();

	CRenderTargetMgr::GetInstance()->End_MRT_Defferd(L"MRT_ALL");

	Render_Light();
}

void CRenderer::Render_Light(void)
{
	CRenderTargetMgr::GetInstance()->Begin_MRT_Light(L"MRT_ALL");

	m_pDevice->Blend_Begin();

	CRenderTargetMgr::GetInstance()->Render_LightTarget(L"Target_Deferred");

	m_pDevice->Blend_End();

	CRenderTargetMgr::GetInstance()->End_MRT_Light(L"MRT_ALL");
}

void CRenderer::Render_Blend(void)
{
	m_pDevice->Blend_Begin();

	CRenderTargetMgr::GetInstance()->Set_ConstantTable(m_pScreenBuffer, L"Target_Deferred");

	m_pScreenBuffer->Render(DXGI_16);

	m_pDevice->Blend_End();
}

void CRenderer::Render_DebugBuffer(void)
{
#ifdef _DEBUG
	m_pDevice->TurnZBufferOff();

	CRenderTargetMgr::GetInstance()->Render_DebugBuffer(L"Target_Deferred");

	m_pDevice->TurnZBufferOn();
#endif
}

void CRenderer::Release(void)
{
	for (int i = 0; i < RENDERTYPE_END; ++i)
		m_RenderGroup[i].clear();

	Safe_Delete(m_pScreenBuffer);
	CRenderTargetMgr::GetInstance()->DestroyInstance();
}