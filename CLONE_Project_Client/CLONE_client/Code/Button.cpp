#include "stdafx.h"
#include "Button.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
//Object
#include "TextureShader.h"

CButton::CButton(CDevice * pDevice)
	: CUI(pDevice)
{

}

CButton::~CButton()
{
	Release();
}

HRESULT CButton::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_UI;

	m_fSizeX = 84;
	m_fSizeY = 140;

	m_fX = 100.f;
	m_fY = WINCY - 100.f;

	return S_OK;
}

int CButton::Update()
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

void CButton::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matView;
	XMFLOAT4X4 matProj;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	memcpy(&matView, m_matUIView, sizeof(float) * 16);
	memcpy(&matProj, m_matUIProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

	m_pBuffer->Render(DXGI_16);
}

HRESULT CButton::AddComponent(void)
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

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_CustomSlideLButton");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Texture"));

	return S_OK;
}

CButton * CButton::Create(CDevice * pDevice)
{
	CButton*	pObj = new CButton(pDevice);

	if (FAILED(pObj->Init()))
		Safe_Delete(pObj);

	return pObj;
}

void CButton::Release()
{
	Safe_Release(m_pShader);
}

void CButton::RenderForShadow(LightMatrixStruct tLight)
{
}



