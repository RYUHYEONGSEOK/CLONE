#include "stdafx.h"
#include "CustomBack.h"
//Manager
#include "ResourceMgr.h"
#include "TimeMgr.h"
//Object
#include "CustomButton.h"
#include "Font.h"
#include "LogoBackShader.h"
#include "Loading.h"
//server
#include "ServerMgr.h"
#include "../../CLONE_server/CLONE_server/Protocol.h"

CCustomBack::CCustomBack(CDevice * pDevice)
	: CObj(pDevice)
	, m_fSizeX(0.f)
	, m_fSizeY(0.f)
	, m_fX(0.f)
	, m_fY(0.f)
	, m_pCusButton(NULL)
	, m_bIsStartClicked(false)
{

}

CCustomBack::~CCustomBack()
{
	Release();
}

HRESULT CCustomBack::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");
	FAILED_CHECK_RETURN_MSG(FontSetting(), E_FAIL, L"FontSetting Failed");

	//버튼다운 초기화
	for (int i = 0; i < 3; ++i)
		m_bButtonDown[i] = FALSE;

	//Matrix 초기화
	D3DXMatrixIdentity(&m_matLogoBackView);
	D3DXMatrixIdentity(&m_matLogoBackProj);
	
	//CustomBack Setting
	m_fSizeX = m_fX = WINCX / 2.f;
	m_fSizeY = m_fY = WINCY / 2.f;

	//Key Delay 용
	m_dwDelayTime = GetTickCount();

	return S_OK;
}

int CCustomBack::Update()
{
	//start 충돌부분
	if (CServerMgr::GetInstance()->GetIsHost() && CServerMgr::GetInstance()->GetIsAllReady() && !m_bIsStartClicked)
	{
		if (((CCustomButton*)m_pCusButton)->GetButtonState(CCustomButton::CUS_BUTTON_START, CCustomButton::CUS_BUTTON_STATE_CHECKED))
		{
			if (m_dwDelayTime + 100.f < GetTickCount())
			{
				m_dwDelayTime = GetTickCount();
				m_bIsStartClicked = true;

				CS_Ready tPacket;
				tPacket.m_bIsHost = CServerMgr::GetInstance()->GetIsHost();
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
	}
	//ready 충돌부분
	else if (!CServerMgr::GetInstance()->GetIsHost())
	{
		if (((CCustomButton*)m_pCusButton)->GetButtonState(CCustomButton::CUS_BUTTON_READY, CCustomButton::CUS_BUTTON_STATE_CHECKED))
		{
			if (m_dwDelayTime + 100.f < GetTickCount())
			{
				m_dwDelayTime = GetTickCount();

				CS_Ready tPacket;
				tPacket.m_bIsHost = CServerMgr::GetInstance()->GetIsHost();
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
	}

	//change 충돌부분
	if (((CCustomButton*)m_pCusButton)->GetButtonState(CCustomButton::CUS_BUTTON_CHANGE, CCustomButton::CUS_BUTTON_STATE_CHECKED))
	{
		int iMyID = CServerMgr::GetInstance()->GetClientID();
		bool bIsMyReady = CServerMgr::GetInstance()->FindClient(iMyID)->m_bIsReady;

		if (!bIsMyReady)
		{
			if (m_dwDelayTime + 100.f < GetTickCount())
			{
				m_dwDelayTime = GetTickCount();

				CS_Change tPacket;
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
	}

	//Program Exit
	// 클라이언트 EXIT버튼
	if (!((CCustomButton*)m_pCusButton)->GetButtonState(CCustomButton::CUS_BUTTON_EXIT, CCustomButton::CUS_BUTTON_STATE_CHECKED)
		&& ((CCustomButton*)m_pCusButton)->GetButtonState(CCustomButton::CUS_BUTTON_EXIT, CCustomButton::CUS_BUTTON_STATE_CHECKED))
		PostQuitMessage(TRUE);	// 프로그램 종료

	//버튼 상태 reset
	((CCustomButton*)m_pCusButton)->ResetButtonState();

	// Font Update!
	FontUpdate();

	//CustomBack
	D3DXMatrixOrthoLH(&m_matLogoBackProj, WINCX, WINCY, 0.f, 1.f);
	m_matLogoBackView._11 = m_fSizeX;
	m_matLogoBackView._22 = m_fSizeY;
	m_matLogoBackView._33 = 1.f;

	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matLogoBackView._41 = m_fX - (WINCX >> 1);
	m_matLogoBackView._42 = -m_fY + (WINCY >> 1);

	CObj::Update_Component();

	return 0;
}

void CCustomBack::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matView;
	XMFLOAT4X4 matProj;

	//loading percent Input
	XMFLOAT4 xmvPercent = XMFLOAT4(1.f, 0.f, 0.f, 0.f);

	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&matView, m_matLogoBackView, sizeof(float) * 16);
	memcpy(&matProj, m_matLogoBackProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	if(g_GameInfo.bMode == FALSE)
		((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pTexture->GetResource(0), xmvPercent);
	else
		((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pTexture->GetResource(1), xmvPercent);

	m_pBuffer->Render(DXGI_16);

	//Font Render
	for (size_t i = 0; i < CUS_FONT_END; ++i)
	{
		m_pFont[i]->Render();
	}
}

HRESULT CCustomBack::AddComponent(void)
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
		L"Texture_CustomBack");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Font
	for (size_t i = 0; i < CUS_FONT_END; ++i)
	{
		pComponent = CResourceMgr::GetInstance()->CloneResource(
			CResourceMgr::RESOURCE_ATTRI_STATIC,
			CResourceMgr::RESOURCE_TYPE_FONT,
			L"Font_koverwatch1");
		m_pFont[i] = dynamic_cast<CFont*>(pComponent);
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent.insert(make_pair(L"Font", pComponent));
	}

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_LogoBack"));

	return S_OK;
}

HRESULT CCustomBack::FontSetting(void)
{
	//Font Setting
	for (size_t i = 0; i < CUS_FONT_END; ++i)
	{
		m_pFont[i]->m_eType = CFont::FONT_TYPE_OUTLINE;
		m_pFont[i]->m_fSize = 30.f;
		m_pFont[i]->m_nColor = 0xFFFFFFFF;
		m_pFont[i]->m_nFlag = FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE;
		switch (i)
		{
		case CUS_FONT_MODE:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 2.f, WINCY / 2.f + WINCY / 4.2f);
			break;
		case CUS_FONT_ROUND:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 2.f, WINCY / 2.f + WINCY / 2.97f);
			break;
		case CUS_FONT_BOTCNT:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 2.f, WINCY / 2.f + WINCY / 2.3f);
			break;
		case CUS_FONT_CLIENT0:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 7.f, WINCY / 3.6f);
			break;
		case CUS_FONT_CLIENT1:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX - WINCX / 7.f, WINCY / 3.6f);
			break;
		case CUS_FONT_CLIENT2:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 7.f, WINCY / 2.3f);
			break;
		case CUS_FONT_CLIENT3:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX - WINCX / 7.f, WINCY / 2.3f);
			break;
		case CUS_FONT_CLIENT4:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 7.f, WINCY / 1.68f);
			break;
		case CUS_FONT_CLIENT5:
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX - WINCX / 7.f, WINCY / 1.68f);
			break;
		}
		m_pFont[i]->m_fOutlineSize = 1.f;
		m_pFont[i]->m_nOutlineColor = 0xFF000000;
	}

	return S_OK;
}

CCustomBack * CCustomBack::Create(CDevice * pDevice)
{
	CCustomBack*	pTerrain = new CCustomBack(pDevice);

	if (FAILED(pTerrain->Init()))
		Safe_Delete(pTerrain);

	return pTerrain;
}

void CCustomBack::Release()
{
	m_pCusButton = NULL;
	Safe_Release(m_pShader);
}

void CCustomBack::RenderForShadow(LightMatrixStruct tLight)
{
}

void CCustomBack::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}

void CCustomBack::FontUpdate(void)
{
	// 어짜피 마우스는 다른 버튼이랑 중복으로 누를수가없다!
	// 어떤 버튼이 눌렸는지만 얻어오자 버튼은 총 6개	
	bool bCheck[6];
	bool bMouseOn[6];

	for (int j = 0; j < 6; ++j)
	{
		// i의 값이 -> 6~11이 원하는 슬라이드 값이다.
		// 두번째 인자 -> 2 가 눌렸다는 Flag 값이다.
		bCheck[j] = ((CCustomButton*)m_pCusButton)->GetButtonState(j + 6, 2);
		// 마우스 오버가 되어있는지도 체크한다.
		bMouseOn[j] = ((CCustomButton*)m_pCusButton)->GetButtonState(j + 6, 1);
	}


	//update
	if (CServerMgr::GetInstance()->GetIsHost())
	{
		// 상단 좌, 우 모드 슬라이드 
		if (bCheck[0] || bCheck[1])
		{
			if (bMouseOn[0] || bMouseOn[1])
			{
				if (!m_bButtonDown[0])
				{
					m_bButtonDown[0] = TRUE;
					CS_OptionChange tPacket;
					tPacket.m_bIsHost = CServerMgr::GetInstance()->GetIsHost();
					tPacket.m_bDownMode = true;
					tPacket.m_bUpMode = true;
					tPacket.m_bCheckMap = false;
					tPacket.m_bDownBot = false;
					tPacket.m_bUpBot = false;
					CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
				}
			}
		}
		else
			m_bButtonDown[0] = FALSE;
		// 중단 좌 스테이지 슬라이드
		if (bCheck[2] && bMouseOn[2])
		{
			if (!m_bButtonDown[1])
			{
				m_bButtonDown[1] = TRUE;
				CS_OptionChange tPacket;
				tPacket.m_bIsHost = CServerMgr::GetInstance()->GetIsHost();
				tPacket.m_bDownMode = false;
				tPacket.m_bUpMode = false;
				tPacket.m_bCheckMap = true;
				tPacket.m_bDownBot = false;
				tPacket.m_bUpBot = false;
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
		else
			m_bButtonDown[1] = FALSE;
		// 중단 우 스테이지 슬라이드
		if (bCheck[3] && bMouseOn[3])
		{
			if (!m_bButtonDown[2])
			{
				m_bButtonDown[2] = TRUE;
				CS_OptionChange tPacket;
				tPacket.m_bIsHost = CServerMgr::GetInstance()->GetIsHost();
				tPacket.m_bDownMode = false;
				tPacket.m_bUpMode = false;
				tPacket.m_bCheckMap = true;
				tPacket.m_bDownBot = false;
				tPacket.m_bUpBot = false;
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
		else
			m_bButtonDown[2] = FALSE;

		// 하단 좌 봇카운트 슬라이드
		if (bCheck[4] && bMouseOn[4])
		{
			if (m_dwDelayTime + 30.f < GetTickCount())
			{
				m_dwDelayTime = GetTickCount();
				m_bButtonDown[2] = TRUE;
				CS_OptionChange tPacket;
				tPacket.m_bIsHost = CServerMgr::GetInstance()->GetIsHost();
				tPacket.m_bDownMode = false;
				tPacket.m_bUpMode = false;
				tPacket.m_bCheckMap = false;
				tPacket.m_bDownBot = true;
				tPacket.m_bUpBot = false;
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
		// 하단 우 봇카운트 슬라이드
		if (bCheck[5] && bMouseOn[5])
		{
			if (m_dwDelayTime + 30.f < GetTickCount())
			{
				m_dwDelayTime = GetTickCount();
				CS_OptionChange tPacket;
				tPacket.m_bIsHost = CServerMgr::GetInstance()->GetIsHost();
				tPacket.m_bDownMode = false;
				tPacket.m_bUpMode = false;
				tPacket.m_bCheckMap = false;
				tPacket.m_bDownBot = false;
				tPacket.m_bUpBot = true;
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
	}


	//render
	//Font Setting - Game Mode
	for (size_t i = 0; i <= CUS_FONT_BOTCNT; ++i)
	{
		switch (i)
		{
		case 0:
			if (g_GameInfo.bMode == true) m_pFont[i]->m_wstrText = L"TEAM";
			else if (g_GameInfo.bMode == false) m_pFont[i]->m_wstrText = L"SINGLE";
			break;
		case 1:
			if(g_GameInfo.bMapType) m_pFont[i]->m_wstrText = L"KJK_POOL";
			else m_pFont[i]->m_wstrText = L"KJK_PLAZA";
			break;
		case 2:
			m_pFont[i]->m_wstrText = to_wstring(g_GameInfo.iBotCnt);
			break;

		}
	}

	//Font Setting - Client
	int iRoomIdx = 0;

	//font reset
	for (int i = 0; i < 6; ++i)
	{
		m_pFont[i + 3]->m_nColor = 0xFFFFFFFF;
		m_pFont[i + 3]->m_wstrText = L"";
	}

	unordered_map<wstring, list<CLIENT*>>::iterator iterMap = g_mapClient.begin();
	unordered_map<wstring, list<CLIENT*>>::iterator iterMap_end = g_mapClient.end();

	for (iterMap; iterMap != iterMap_end; ++iterMap)
	{
		iRoomIdx = iterMap->second.front()->m_iRoomIndex;

		if (iterMap->second.front()->m_bIsReady)
		{
			m_pFont[iRoomIdx + 3]->m_nColor = 0xFFFF0000;
		}
		else if (!iterMap->second.front()->m_bIsReady)
		{
			m_pFont[iRoomIdx + 3]->m_nColor = 0xFFFFFFFF;
		}

		// 여기서 클라이언트의 이름을 넣어준다.
		m_pFont[iRoomIdx + 3]->m_wstrText = iterMap->first;
	}
}



