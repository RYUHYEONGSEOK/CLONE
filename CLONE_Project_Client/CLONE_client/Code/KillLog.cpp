#include "stdafx.h"
#include "KillLog.h"
//Manager
#include "ResourceMgr.h"
#include "TimeMgr.h"
//Object
#include "TextureShader.h"
#include "Font.h"

CKillLog::CKillLog(CDevice * pDevice)
	: CUI(pDevice)
	, m_fAppearTime(5.f)
{
	
}

CKillLog::~CKillLog()
{
	Release();
}

HRESULT CKillLog::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_UI;

	m_fSizeX = WINCX / 10.f;
	m_fSizeY = WINCY / 45.f;

	m_fX = KILLLOG_ORIGEN_X;
	m_fY = KILLLOG_ORIGEN_Y;

	//Font Setting
	m_pKillerFont->m_eType = CFont::FONT_TYPE_OUTLINE;
	m_pKillerFont->m_wstrText = L"Here";
	m_pKillerFont->m_fSize = WINCY / 50.f;
	m_pKillerFont->m_nColor = 0xFFFFFFFF;
	m_pKillerFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pKillerFont->m_vPos = D3DXVECTOR2(m_fX - (m_fSizeX - 20.f), m_fY);
	m_pKillerFont->m_fOutlineSize = 1.f;
	m_pKillerFont->m_nOutlineColor = 0xFF000000;

	//Font Setting
	m_pVictimFont->m_eType = CFont::FONT_TYPE_OUTLINE;
	m_pVictimFont->m_wstrText = L"Here";
	m_pVictimFont->m_fSize = WINCY / 50.f;
	m_pVictimFont->m_nColor = 0xFFFFFFFF;
	m_pVictimFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
	m_pVictimFont->m_vPos = D3DXVECTOR2(m_fX + m_fSizeX / 5.f, m_fY);
	m_pVictimFont->m_fOutlineSize = 1.f;
	m_pVictimFont->m_nOutlineColor = 0xFF000000;

	return S_OK;
}

int CKillLog::Update()
{
	D3DXMatrixOrthoLH(&m_matUIProj, WINCX, WINCY, 0.f, 1.f);
	m_matUIView._11 = m_fSizeX;
	m_matUIView._22 = m_fSizeY;
	m_matUIView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matUIView._41 = m_fX - (WINCX >> 1);
	m_matUIView._42 = -m_fY + (WINCY >> 1);

	//Font Update
	FontUpdate();

	//Timer
	m_fAppearTime -= CTimeMgr::GetInstance()->GetTime();
	if (m_fAppearTime <= 0.f)
		return CObj::OBJ_STATE_DIE;

	CObj::Update_Component();

	return 0;
}

void CKillLog::Render()
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

	m_pKillerFont->Render();
	m_pVictimFont->Render();
}

HRESULT CKillLog::AddComponent(void)
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
	m_pKillerFont = dynamic_cast<CFont*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Font", pComponent));

	//Font
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_STATIC,
		CResourceMgr::RESOURCE_TYPE_FONT,
		L"Font_koverwatch1");
	m_pVictimFont = dynamic_cast<CFont*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Font", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_UINameTag");
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

CKillLog * CKillLog::Create(CDevice * pDevice)
{
	CKillLog*	pObj = new CKillLog(pDevice);

	if (FAILED(pObj->Init()))
		Safe_Delete(pObj);

	return pObj;
}

void CKillLog::Release()
{
	Safe_Release(m_pShader);
}

void CKillLog::RenderForShadow(LightMatrixStruct tLight)
{
}

void CKillLog::SetKillLogPos(D3DXVECTOR3 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;
}

void CKillLog::SetLogStr(const wstring & wstrKiller, const wstring & wstrVictim)
{
	m_wstrKiller = wstrKiller;
	m_wstrVictim = wstrVictim;
}

D3DXVECTOR3 CKillLog::GetKillLogPos(void)
{
	return D3DXVECTOR3(m_fX, m_fY, 1.f);
}

void CKillLog::FontUpdate(void)
{
	m_pKillerFont->m_vPos = D3DXVECTOR2(m_fX - (m_fSizeX - 20.f), m_fY);
	m_pVictimFont->m_vPos = D3DXVECTOR2(m_fX + m_fSizeX / 4.f, m_fY);

	m_pKillerFont->m_wstrText = m_wstrKiller;
	m_pVictimFont->m_wstrText = m_wstrVictim;
}



