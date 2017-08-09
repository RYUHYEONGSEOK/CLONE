#include "stdafx.h"
#include "LogoBack.h"
//Manager
#include "ResourceMgr.h"
#include "TimeMgr.h"
//Object
#include "Font.h"
#include "LogoBackShader.h"
#include "Loading.h"

CLogoBack::CLogoBack(CDevice * pDevice)
	: CObj(pDevice)
	, m_fSizeX(0.f)
	, m_fSizeY(0.f)
	, m_fX(0.f)
	, m_fY(0.f)
	, m_pLoading(NULL)
	, m_pFont(NULL)
	, m_bTextChange(FALSE)
{
	
}

CLogoBack::~CLogoBack()
{
	Release();
}

HRESULT CLogoBack::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	//LogoBack 용 Matrix
	D3DXMatrixIdentity(&m_matLogoBackView);
	D3DXMatrixIdentity(&m_matLogoBackProj);

	//LogoBack Size
	m_fSizeX = m_fX = WINCX / 2.f;
	m_fSizeY = m_fY = WINCY / 2.f;

	//Font Setting
	m_pFont->m_eType = CFont::FONT_TYPE_OUTLINE;
	m_pFont->m_wstrText = L"";
	m_pFont->m_fSize = WINCY / 50.f;
	m_pFont->m_nColor = 0xFFFFFFFF;
	m_pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_vPos = D3DXVECTOR2(20.f, WINCY - 30.f);
	m_pFont->m_fOutlineSize = 1.f;
	m_pFont->m_nOutlineColor = 0xFF000000;

	return S_OK;
}

int CLogoBack::Update()
{
	//LogoBack
	D3DXMatrixOrthoLH(&m_matLogoBackProj, WINCX, WINCY, 0.f, 1.f);
	m_matLogoBackView._11 = m_fSizeX;
	m_matLogoBackView._22 = m_fSizeY;
	m_matLogoBackView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matLogoBackView._41 = m_fX - (WINCX >> 1);
	m_matLogoBackView._42 = -m_fY + (WINCY >> 1);

	//Font Message
	m_pFont->m_wstrText = m_pLoading->GetLoadingMessage();

	CObj::Update_Component();

	return 0;
}

void CLogoBack::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matView;
	XMFLOAT4X4 matProj;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	memcpy(&matView, m_matLogoBackView, sizeof(float) * 16);
	memcpy(&matProj, m_matLogoBackProj, sizeof(float) * 16);

	//loading percent Input
	XMFLOAT4 xmvPercent = XMFLOAT4(1.f, 0.f, 0.f, 0.f);

	//셰이더 파라미터 세팅
	if(!m_bTextChange)
		((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pTexture->GetResource(0), xmvPercent);
	else
		((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pTexture->GetResource(4), xmvPercent);

	m_pBuffer->Render(DXGI_16);

	//Font Render
	m_pFont->Render();
}

HRESULT CLogoBack::AddComponent(void)
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
		L"Texture_LogoBack");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Font
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_STATIC,
		CResourceMgr::RESOURCE_TYPE_FONT,
		L"Font_koverwatch");
	m_pFont = dynamic_cast<CFont*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Font", pComponent));
	
	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_LogoBack"));

	return S_OK;
}

CLogoBack * CLogoBack::Create(CDevice * pDevice)
{
	CLogoBack*	pTerrain = new CLogoBack(pDevice);

	if (FAILED(pTerrain->Init()))
		Safe_Delete(pTerrain);

	return pTerrain;
}

void CLogoBack::Release()
{
	Safe_Release(m_pShader);
}

void CLogoBack::RenderForShadow(LightMatrixStruct tLight)
{
}

void CLogoBack::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}



