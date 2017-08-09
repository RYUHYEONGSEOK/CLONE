#include "stdafx.h"
#include "Status.h"
//Manager
#include "ResourceMgr.h"
#include "SceneMgr.h"
#include "LightMgr.h"
//Object
#include "Font.h"
#include "TextureShader.h"

CStatus::CStatus(CDevice * pDevice)
	: CUI(pDevice)
	, m_pFont(NULL)
	, m_pGrapichBrandFont(NULL)
	, m_pGrapichMemoryFont(NULL)
{

}

CStatus::~CStatus()
{
	Release();
}

HRESULT CStatus::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_UI;

	//Font Setting
	m_pFont->m_eType = CFont::FONT_TYPE_OUTLINE;
	m_pFont->m_wstrText = L"Clone_Client";
	m_pFont->m_fSize = 17.f;
	m_pFont->m_nColor = 0xFFFFFFFF;
	m_pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_vPos = D3DXVECTOR2(20.f, 20.f);
	m_pFont->m_fOutlineSize = 1.f;
	m_pFont->m_nOutlineColor = 0xFF000000;
	
	//Graphic Brand Font Setting
	m_pGrapichBrandFont->m_eType = CFont::FONT_TYPE_BASIC;
	wstring wName(&((CDevice::GetInstance()->GetGraphicBrandName())[0]));
	m_pGrapichBrandFont->m_wstrText = wName;
	m_pGrapichBrandFont->m_fSize = 17.f;
	m_pGrapichBrandFont->m_nColor = 0xFF41FF3A;
	m_pGrapichBrandFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pGrapichBrandFont->m_vPos = D3DXVECTOR2(20.f, 50.f);
	//m_pGrapichBrandFont->m_fOutlineSize = 1.f;
	//m_pGrapichBrandFont->m_nOutlineColor = 0xFF000000;

	//Graphic Memory Font Setting
	m_pGrapichMemoryFont->m_eType = CFont::FONT_TYPE_BASIC;
	wstring wMemory = to_wstring(m_pDevice->GetGraphicMemory());
	wstring wMegaByte(L" MB");
	wMemory += wMegaByte;
	m_pGrapichMemoryFont->m_wstrText = wMemory;
	m_pGrapichMemoryFont->m_fSize = 17.f;
	m_pGrapichMemoryFont->m_nColor = 0xFF0000FF;
	m_pGrapichMemoryFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pGrapichMemoryFont->m_vPos = D3DXVECTOR2(20.f, 70.f);
	//m_pGrapichMemoryFont->m_fOutlineSize = 1.f;
	//m_pGrapichMemoryFont->m_nOutlineColor = 0xFF000000;

	m_fSizeX = 84;
	m_fSizeY = 140;

	m_fX = 200.f;
	m_fY = 200.f;

	return S_OK;
}

int CStatus::Update()
{
	D3DXMatrixOrthoLH(&m_matUIProj, WINCX, WINCY, 0.f, 1.f);
	m_matUIView._11 = m_fSizeX;
	m_matUIView._22 = m_fSizeY;
	m_matUIView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matUIView._41 = m_fX - (WINCX >> 1);
	m_matUIView._42 = -m_fY + (WINCY >> 1);

	// 	RECT	rcUI = {long(m_fX - m_fSizeX * 0.5)
	// 		, long(m_fY - m_fSizeY * 0.5)
	// 		, long(m_fX + m_fSizeX * 0.5)
	// 		, long(m_fY + m_fSizeY * 0.5)};

	// 	POINT		ptMouse;
	// 	GetCursorPos(&ptMouse);
	// 	ScreenToClient(g_hWnd, &ptMouse);
	// 
	// 	if(PtInRect(&rcUI, ptMouse))
	// 	{
	// 		m_fSizeX = 200.f;
	// 		m_fSizeY = 200.f;
	// 	}
	// 	else
	// 	{
	// 		m_fSizeX = 100.f;
	// 		m_fSizeY = 100.f;
	// 	}

	CObj::Update_Component();

	return 0;
}

void CStatus::Render()
{
	//카메라에서 행렬 뽑아옴
	//SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matView;
	XMFLOAT4X4 matProj;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	memcpy(&matView, m_matUIView, sizeof(float) * 16);
	memcpy(&matProj, m_matUIProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer, NULL, NULL);

	m_pBuffer->Render(DXGI_16);

	//Font
	m_pFont->Render();
	m_pGrapichBrandFont->Render();
	m_pGrapichMemoryFont->Render();
}

HRESULT CStatus::AddComponent(void)
{
	CComponent*		pComponent = NULL;

	//Transform
	pComponent = m_pInfo = CTransform::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Transform", pComponent));

	//Buffer
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_BUFFER,
		L"Buffer_UI");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Buffer", pComponent));

	//Font
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_STATIC,
		CResourceMgr::RESOURCE_TYPE_FONT,
		L"Font_koverwatch1");
	m_pFont = dynamic_cast<CFont*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Font", pComponent));

	//GraphicBrandFont
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_STATIC,
		CResourceMgr::RESOURCE_TYPE_FONT,
		L"Font_koverwatch1");
	m_pGrapichBrandFont = dynamic_cast<CFont*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Font", pComponent));

	//GraphicMemoryFont
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_STATIC,
		CResourceMgr::RESOURCE_TYPE_FONT,
		L"Font_koverwatch1");
	m_pGrapichMemoryFont = dynamic_cast<CFont*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Font", pComponent));

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Texture"));

	return S_OK;
}

CStatus * CStatus::Create(CDevice * pDevice)
{
	CStatus*	pUI = new CStatus(pDevice);

	if (FAILED(pUI->Init()))
		Safe_Delete(pUI);

	return pUI;
}

void CStatus::Release()
{
	Safe_Release(m_pShader);
}

void CStatus::RenderForShadow(LightMatrixStruct tLight)
{
}

void CStatus::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}



