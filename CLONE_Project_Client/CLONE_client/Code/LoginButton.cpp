#include "stdafx.h"
#include "LoginButton.h"
//Manager
#include "ResourceMgr.h"
#include "SceneMgr.h"
#include "LightMgr.h"
#include "InputMgr.h"
#include "TimeMgr.h"
//Object
#include "TextureShader.h"

CLoginButton::CLoginButton(CDevice * pDevice)
	: CUI(pDevice)
	, m_bClickDown(FALSE)
	, m_bClickUp(FALSE)
{

}

CLoginButton::~CLoginButton()
{
	Release();
}

HRESULT CLoginButton::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_UI;

	m_eType = BUTTON_NORMAL;

	//Size
	m_fSizeX = WINCX / 25.f;
	m_fSizeY = WINCY / 40.f;

	//Pos
	m_fX = WINCX / 2.f;
	m_fY = WINCY / 2.f + 30.f;

	return S_OK;
}

int CLoginButton::Update()
{
	D3DXMatrixOrthoLH(&m_matUIProj, WINCX, WINCY, 0.f, 1.f);
	m_matUIView._11 = m_fSizeX;
	m_matUIView._22 = m_fSizeY;
	m_matUIView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matUIView._41 = m_fX - (WINCX >> 1);
	m_matUIView._42 = -m_fY + (WINCY >> 1);

	RECT	rcUI = {long(m_fX - m_fSizeX/* * 0.5*/)
		, long(m_fY - m_fSizeY/* * 0.5*/)
		, long(m_fX + m_fSizeX/* * 0.5*/)
		, long(m_fY + m_fSizeY/* * 0.5*/)};

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	
	if(PtInRect(&rcUI, ptMouse))
	{
		m_eType = CLoginButton::BUTTON_MOUSEON;
		KeyCheck();
	}
	else
	{
		m_eType = CLoginButton::BUTTON_NORMAL;
		m_bClickDown = FALSE;
	}

	CObj::Update_Component();

	return 0;
}

void CLoginButton::Render()
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
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource(m_eType));

	m_pBuffer->Render(DXGI_16);
}

HRESULT CLoginButton::AddComponent(void)
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
		L"Buffer_Logo");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Buffer", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_LoginCheck");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//LightInfo
	//m_pLightInfo = CLightMgr::GetInstance()->GetLightInfo(0);

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Texture"));

	return S_OK;
}

void CLoginButton::KeyCheck(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();

	if (CInput::GetInstance()->GetDIMouseState(CInput::MOUSE_TYPE_L) & 0x80)
	{
		if (!m_bClickDown)
		{
			m_bClickDown = TRUE;
		}
		else
		{
			m_eType = CLoginButton::BUTTON_CLICKED;
		}
	}
	else
	{
		if (m_bClickDown)
		{
			m_bClickDown = FALSE;
			m_bClickUp = TRUE;
		}
	}
}

CLoginButton * CLoginButton::Create(CDevice * pDevice)
{
	CLoginButton*	pUI = new CLoginButton(pDevice);

	if (FAILED(pUI->Init()))
		Safe_Delete(pUI);

	return pUI;
}

void CLoginButton::Release()
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
}

void CLoginButton::RenderForShadow(LightMatrixStruct tLight)
{
}



