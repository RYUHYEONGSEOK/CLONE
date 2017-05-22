#include "stdafx.h"
#include "LogoTitle.h"
//Manager
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
//Object
#include "Font.h"
#include "LogoBackShader.h"
#include "Loading.h"
#include "LoginButton.h"

CLogoTitle::CLogoTitle(CDevice * pDevice)
	: CObj(pDevice)
	, m_fSizeX(0.f)
	, m_fSizeY(0.f)
	, m_fProgSizeX(0.f)
	, m_fProgSizeY(0.f)
	, m_fTopX(0.f)
	, m_fTopY(0.f)
	, m_fBotX(0.f)
	, m_fBotY(0.f)
	, m_fProgX(0.f)
	, m_fProgY(0.f)
	, m_pTopTexture(NULL)
	, m_pLoading(NULL)
	, m_pButton(NULL)
	, m_pBotTexture(NULL)
	, m_pProgressTexture(NULL)
	, m_bLoadComplete(FALSE)
{

}

CLogoTitle::~CLogoTitle()
{
	Release();
}

HRESULT CLogoTitle::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	// Matrix
	D3DXMatrixIdentity(&m_matTopUIView);
	D3DXMatrixIdentity(&m_matTopUIProj);

	D3DXMatrixIdentity(&m_matBotUIView);
	D3DXMatrixIdentity(&m_matBotUIProj);

	D3DXMatrixIdentity(&m_matProgUIView);
	D3DXMatrixIdentity(&m_matProgUIProj);

	// Title ---------------------------------------
	// Size
	m_fSizeX = WINCX / 2.f;
	m_fSizeY = WINCY / 8.f;

	// Pos
	m_fTopX = WINCX / 2.f;
	m_fTopY = WINCY / 2.f - WINCY / 7.f;

	m_fBotX = WINCX / 2.f;
	m_fBotY = WINCY / 2.f + WINCY / 7.f;

	//Progress Bar ---------------------------------
	//Size
	m_fProgSizeX = WINCX;
	m_fProgSizeY = WINCY / 100.f;
	
	//Pos
	m_fProgX = 0.f;
	m_fProgY = WINCY - WINCY / 8.675f;

	return S_OK;
}

int CLogoTitle::Update()
{
	if (m_bLoadComplete)
	{
		if(m_fTopY > WINCY / 2.f - WINCY / 5.f)
			m_fTopY -= CTimeMgr::GetInstance()->GetTime() * 25.f;
		if (m_fBotY < WINCY / 2.f + WINCY / 5.f)
			m_fBotY += CTimeMgr::GetInstance()->GetTime() * 25.f;
		else
		{
			m_bTitleMoveComplete = TRUE;

			if (m_pButton == NULL)
			{
				m_pButton = CLoginButton::Create(m_pDevice);
				NULL_CHECK_RETURN(m_pButton, E_FAIL);
			}
			else
			{
				m_pButton->Update();
			}
		}
	}

	//Top Title ------------------------------------------
	D3DXMatrixOrthoLH(&m_matTopUIProj, WINCX, WINCY, 0.f, 1.f);
	m_matTopUIView._11 = m_fSizeX;
	m_matTopUIView._22 = m_fSizeY;
	m_matTopUIView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matTopUIView._41 = m_fTopX - (WINCX >> 1);
	m_matTopUIView._42 = -m_fTopY + (WINCY >> 1);


	//Bot Title ------------------------------------------
	D3DXMatrixOrthoLH(&m_matBotUIProj, WINCX, WINCY, 0.f, 1.f);
	m_matBotUIView._11 = m_fSizeX;
	m_matBotUIView._22 = m_fSizeY;
	m_matBotUIView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matBotUIView._41 = m_fBotX - (WINCX >> 1);
	m_matBotUIView._42 = -m_fBotY + (WINCY >> 1);


	//Progress Bar ---------------------------------------
	D3DXMatrixOrthoLH(&m_matProgUIProj, WINCX, WINCY, 0.f, 1.f);
	m_matProgUIView._11 = m_fProgSizeX;
	m_matProgUIView._22 = m_fProgSizeY;
	m_matProgUIView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matProgUIView._41 = m_fProgX - (WINCX >> 1);
	m_matProgUIView._42 = -m_fProgY + (WINCY >> 1);


	CObj::Update_Component();

	return 0;
}

void CLogoTitle::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//Top Title ------------------------------------------
	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matView;
	XMFLOAT4X4 matProj;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	memcpy(&matView, m_matTopUIView, sizeof(float) * 16);
	memcpy(&matProj, m_matTopUIProj, sizeof(float) * 16);

	//loading percent Input
	XMFLOAT4 xmvPercent = XMFLOAT4(m_pLoading->GetLoadingPercent(), 0.f, 0.f, 0.f);

	//셰이더 파라미터 세팅
	((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer, m_pTopTexture->GetResource(), xmvPercent);

	m_pBuffer->Render(DXGI_16);

	//Bot Title ------------------------------------------
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	memcpy(&matView, m_matBotUIView, sizeof(float) * 16);
	memcpy(&matProj, m_matBotUIProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer, m_pBotTexture->GetResource(), xmvPercent);

	m_pBuffer->Render(DXGI_16);

	//Progress Bar ---------------------------------------
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	memcpy(&matView, m_matProgUIView, sizeof(float) * 16);
	memcpy(&matProj, m_matProgUIProj, sizeof(float) * 16);

	xmvPercent = XMFLOAT4(m_pLoading->GetLoadingPercent(), 0.f, 0.f, 0.f);

	//셰이더 파라미터 세팅
	((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer, m_pProgressTexture->GetResource(), xmvPercent);

	m_pBuffer->Render(DXGI_16);

	//LogoButton Render
	if (m_pButton != NULL)
		m_pButton->Render();
}

HRESULT CLogoTitle::AddComponent(void)
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

	//Top Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_TitleTop");
	m_pTopTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Bot Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_TitleBottom");
	m_pBotTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Progress Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_LoadingProgress");
	m_pProgressTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_LogoBack"));

	return S_OK;
}

bool CLogoTitle::GetSceneChangeFlag(void)
{
	if (m_pButton != NULL)
	{
		return m_pButton->GetSceneChangeFlag();
	}
	
	return FALSE;
}

CLogoTitle * CLogoTitle::Create(CDevice * pDevice)
{
	CLogoTitle*	pTerrain = new CLogoTitle(pDevice);

	if (FAILED(pTerrain->Init()))
		Safe_Delete(pTerrain);

	return pTerrain;
}

void CLogoTitle::Release()
{
	Safe_Delete(m_pButton);
	Safe_Release(m_pShader);
}

void CLogoTitle::RenderForShadow(LightMatrixStruct tLight)
{
}
