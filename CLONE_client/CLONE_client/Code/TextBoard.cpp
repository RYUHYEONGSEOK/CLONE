#include "stdafx.h"
#include "TextBoard.h"
//Manager
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "InputMgr.h"
//Object
#include "TextureShader.h"
#include "Font.h"
#include "InputHelper.h"

CTextBoard::CTextBoard(CDevice * pDevice)
	: CUI(pDevice)
	, m_bIsActive(FALSE)
{
	
}

CTextBoard::~CTextBoard()
{
	Release();
}

HRESULT CTextBoard::Init()
{
	return S_OK;
}

HRESULT CTextBoard::Init_Text(D3DXVECTOR2 vPos, float fSize, wstring wstrInitWord, int iMaxCnt)
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_UI;

	// Font Init
	m_wstrInitText = wstrInitWord;
	m_iMaxCnt = iMaxCnt;
	m_wstrText = wstrInitWord;

	m_pFont->m_eType = CFont::FONT_TYPE_OUTLINE;
	m_pFont->m_nColor = 0xFFFFFFFF;
	m_pFont->m_nFlag = FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE;
	m_pFont->m_wstrText = m_wstrText;
	m_pFont->m_vPos = vPos;
	m_pFont->m_fSize = fSize;
	m_pFont->m_fOutlineSize = 1.f;
	m_pFont->m_nOutlineColor = 0xFF000000;

	// Buffer 크기 초기화
	m_fX = vPos.x;
	m_fY = vPos.y;

	m_fSizeX = fSize;
	m_fSizeY = fSize;

	return S_OK;
}

void CTextBoard::FontUpdate(void)
{
	if (m_bIsActive)
	{
		if (m_wstrText == m_wstrInitText)
			m_wstrText = L"";

		if (CInput::GetInstance()->CheckKeyboardPressed(DIK_BACKSPACE))
		{
			if (m_wstrText.size() > 0)
				m_wstrText.pop_back();
		}

		if (m_wstrText.size() < m_iMaxCnt)
			m_wstrText += GetKeyboardChar();

		m_pFont->m_wstrText = m_wstrText;
		if (m_wstrText.size() > 0)
			m_fSizeX = m_wstrText.size() * m_fSizeY;
		else
			m_fSizeX = m_fSizeY;
	}
}

int CTextBoard::Update()
{
	D3DXMatrixOrthoLH(&m_matUIProj, WINCX, WINCY, 0.f, 1.f);
	m_matUIView._11 = m_fSizeX;
	m_matUIView._22 = m_fSizeY;
	m_matUIView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matUIView._41 = m_fX - (WINCX >> 1);
	m_matUIView._42 = -m_fY + (WINCY >> 1);

	RECT	rcUI = { long(m_fX - m_fSizeX * 0.5)
		, long(m_fY - m_fSizeY * 0.5)
		, long(m_fX + m_fSizeX * 0.5)
		, long(m_fY + m_fSizeY * 0.5) };

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&rcUI, ptMouse))
	{
		if (CInput::GetInstance()->CheckMousePressed(CInput::MOUSE_TYPE_L))
			m_bIsActive = TRUE;
	}
	else
	{
		if (CInput::GetInstance()->CheckMousePressed(CInput::MOUSE_TYPE_L))
		{
			m_bIsActive = FALSE;
			if (m_wstrText == L"")
			{
				m_wstrText = m_wstrInitText;
				m_pFont->m_wstrText = m_wstrText;
			}
		}
	}

	//Font Update
	FontUpdate();

	CObj::Update_Component();

	return 0;
}

void CTextBoard::Render()
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
		m_pBuffer, m_pLightInfo, NULL);

	m_pBuffer->Render(DXGI_16);

	m_pFont->Render();
}

HRESULT CTextBoard::AddComponent(void)
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

	////Texture
	//pComponent = CResourceMgr::GetInstance()->CloneResource(
	//	CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
	//	CResourceMgr::RESOURCE_TYPE_TEXTURE,
	//	L"Texture_UINameTag");
	//m_pTexture = dynamic_cast<CTexture*>(pComponent);
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Texture"));

	return S_OK;
}

CTextBoard * CTextBoard::Create(CDevice * pDevice, D3DXVECTOR2 vPos, float fSize, wstring wstrInitWord, int iMaxCnt)
{
	CTextBoard*	pObj = new CTextBoard(pDevice);

	if (FAILED(pObj->Init_Text(vPos, fSize, wstrInitWord, iMaxCnt)))
		Safe_Delete(pObj);

	return pObj;
}

void CTextBoard::Release()
{
	Safe_Release(m_pShader);
}

void CTextBoard::RenderForShadow(LightMatrixStruct tLight)
{
}

void CTextBoard::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}

