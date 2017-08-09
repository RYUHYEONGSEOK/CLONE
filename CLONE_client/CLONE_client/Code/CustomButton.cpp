#include "stdafx.h"
#include "CustomButton.h"
//Manager
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "InputMgr.h"
#include "SoundMgr.h"
//Object
#include "Font.h"
#include "LogoBackShader.h"
#include "Loading.h"
//server
#include "ServerMgr.h"
#include "../../CLONE_server/CLONE_server/Protocol.h"

CCustomButton::CCustomButton(CDevice * pDevice)
	: CObj(pDevice)
	, m_iStageNum(0)
	, m_bStageChangeFlag(FALSE)
{

}

CCustomButton::~CCustomButton()
{
	Release();
}

HRESULT CCustomButton::Init()
{
	HRESULT hr = NULL;

	for (auto i = 0; i < CUS_BUTTON_END; ++i)
	{
		//Matrix 초기화
		D3DXMatrixIdentity(&m_matLogoBackView[i]);
		D3DXMatrixIdentity(&m_matLogoBackProj[i]);
		//Size, Pos 초기화
		m_fSizeX[i] = m_fSizeY[i] = 0.f;
		m_fX[i] = m_fY[i] = 0.f;
		//버튼 상태 초기화
		for (auto j = 0; j < CUS_STATE_BUTTON_END; ++j)
		{
			m_bButtonState[i][j] = FALSE;
		}
	}

	//Team Setting Init
	m_eTeamType = TEAM_END;

	FAILED_CHECK_RETURN_MSG(ButtonSetting(), E_FAIL, L"ButtonSetting Failed");
	FAILED_CHECK_RETURN_MSG(InitHostPos(), E_FAIL, L"InitHostPos Failed");
	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");
	
	return S_OK;
}

int CCustomButton::Update()
{
	// 방장 아이콘의 위치를 잡는다.
	FAILED_CHECK_RETURN_MSG(HostIconSetting(), E_FAIL, L"HostIconSetting Add Failed");

	for (auto i = 0; i < CUS_BUTTON_END; ++i)
	{
		D3DXMatrixOrthoLH(&m_matLogoBackProj[i], WINCX, WINCY, 0.f, 1.f);
		m_matLogoBackView[i]._11 = m_fSizeX[i];
		m_matLogoBackView[i]._22 = m_fSizeY[i];
		m_matLogoBackView[i]._33 = 1.f;

		// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
		m_matLogoBackView[i]._41 = m_fX[i] - (WINCX >> 1);
		m_matLogoBackView[i]._42 = -m_fY[i] + (WINCY >> 1);

		if (i < CUS_ICON_HOST)
		{
			// 최초 방장이 아닐 시 거르는 버튼은 거른다.
			if (!CServerMgr::GetInstance()->GetIsHost())
			{
				if (i == CUS_BUTTON_START ||
					i == CUS_BUTTON_L_DOWN_SLIDE || i == CUS_BUTTON_R_DOWN_SLIDE ||
					i == CUS_BUTTON_L_MID_SLIDE || i == CUS_BUTTON_R_MID_SLIDE ||
					i == CUS_BUTTON_L_UP_SLIDE || i == CUS_BUTTON_R_UP_SLIDE)
					continue;
			}
			//수정
			else // 호스트일 시 READY 버튼은 거른다.
			{
				//이건 무슨 의미일까?
				//if (i == CUS_BUTTON_READY)
				//{
				//	// READY가 START버튼 보다 먼저이다
				//	// START 확인 전에 모두 READY인지 확인한다.
				//	CheckAllReady();	
				//	continue;
				//}
				if (i == CUS_BUTTON_START)
				{
					// 방장이어도 모두 레디가 아니라면 버튼충돌을 하지 않는다.
					if (!CServerMgr::GetInstance()->GetIsAllReady())
						continue;
				}
			}
			//기존
			//else //호스트일 시 READY 버튼은 거른다.
			//{
			//	if (i == CUS_BUTTON_READY)
			//		continue;
			//}

			RECT	rcUI = { long(m_fX[i] - m_fSizeX[i]/* * 0.5*/)
				, long(m_fY[i] - m_fSizeY[i]/* * 0.5*/)
				, long(m_fX[i] + m_fSizeX[i]/* * 0.5*/)
				, long(m_fY[i] + m_fSizeY[i]/* * 0.5*/) };

			POINT		ptMouse;
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			if (PtInRect(&rcUI, ptMouse))
			{
				//마우스 오버 시 CUS_BUTTON_STATE_MOUSEON 상태 TRUE
				m_bButtonState[i][CUS_BUTTON_STATE_MOUSEON] = TRUE;

				// 오버중 && 클릭시 CUS_BUTTON_STATE_CLICKED TRUE
				if (CInput::GetInstance()->GetDIMouseState(CInput::MOUSE_TYPE_L) & 0x80)
				{
					if (!m_bButtonState[i][CUS_BUTTON_STATE_CLICKED]
						&& !m_bButtonState[i][CUS_BUTTON_STATE_CHECKED])
					{
						m_bButtonState[i][CUS_BUTTON_STATE_CLICKED] = TRUE;
						m_bButtonState[i][CUS_BUTTON_STATE_CHECKED] = TRUE;
#ifdef _PLAY_SOUND
						//ButtonClick Sound
						CSoundMgr::GetInstance()->PlaySoundW(L"button_ES.mp3");
#endif
					}
					else if (!m_bButtonState[i][CUS_BUTTON_STATE_CLICKED]
						&& m_bButtonState[i][CUS_BUTTON_STATE_CHECKED])
					{
						m_bButtonState[i][CUS_BUTTON_STATE_CLICKED] = TRUE;
						m_bButtonState[i][CUS_BUTTON_STATE_CHECKED] = FALSE;
					}
				}
				else //클릭 해제시
				{
					// 레디일때는 체크를 그대로 두어 READY와 UNREADY를 구분한다.
					if(i != CUS_BUTTON_READY)
						m_bButtonState[i][CUS_BUTTON_STATE_CHECKED] = FALSE;
					m_bButtonState[i][CUS_BUTTON_STATE_CLICKED] = FALSE;
				}
			}
			else // 마우스 오버 해제시
			{
				m_bButtonState[i][CUS_BUTTON_STATE_MOUSEON] = FALSE;
			}
		}
	}

	CObj::Update_Component();

	return 0;
}

void CCustomButton::Render()
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

	for (auto i = 0; i < CUS_BUTTON_END; ++i)
	{
		// 최초 방장이 아닐 시 거르는 버튼은 거른다.
		if (!CServerMgr::GetInstance()->GetIsHost())
		{
			if (i == CUS_BUTTON_START ||
				i == CUS_BUTTON_L_DOWN_SLIDE || i == CUS_BUTTON_R_DOWN_SLIDE ||
				i == CUS_BUTTON_L_MID_SLIDE || i == CUS_BUTTON_R_MID_SLIDE ||
				i == CUS_BUTTON_L_UP_SLIDE || i == CUS_BUTTON_R_UP_SLIDE)
				continue;
		}
		else //호스트일 시 READY 버튼은 거른다.
		{
			if (i == CUS_BUTTON_READY)
				continue;
		}

		memcpy(&matView, m_matLogoBackView[i], sizeof(float) * 16);
		memcpy(&matProj, m_matLogoBackProj[i], sizeof(float) * 16);

		// 마우스 오버 & 클릭이 필요한 것들만 따로 이미지 체크
		if (i < CUS_ICON_HOST)
		{
			ID3D11ShaderResourceView* pResource = NULL;

			// 마우스 오버시 리소스선택
			if (m_bButtonState[i][CUS_BUTTON_STATE_MOUSEON])
			{
				pResource = m_pCustomTexture[i]->GetResource(1);

				if (m_bButtonState[i][CUS_BUTTON_STATE_CLICKED])
					pResource = m_pCustomTexture[i]->GetResource(2);
			}
			else // 마우스 오버 아닐시 리소스 선택
				pResource = m_pCustomTexture[i]->GetResource(0);

			// 마지막 리소스 4개인 Ready만 예외처리!
			if(i == CUS_BUTTON_READY && m_bButtonState[i][CUS_BUTTON_STATE_CHECKED])
				pResource = m_pCustomTexture[i]->GetResource(3);

			//방장의 Start 버튼도 예외처리!
			if (i == CUS_BUTTON_START && CServerMgr::GetInstance()->GetIsHost()
				&& !CServerMgr::GetInstance()->GetIsAllReady())
				pResource = m_pCustomTexture[i]->GetResource(1);

			//셰이더 파라미터 세팅
			((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
				m_pBuffer, pResource, xmvPercent);
		}
		else if (i == CUS_STAGEIMG) // 스테이지 미리보기 이미지
		{
			ID3D11ShaderResourceView* pResource = NULL;

			//if (m_bButtonState[CUS_BUTTON_L_MID_SLIDE][CUS_BUTTON_STATE_CLICKED]||
			//	m_bButtonState[CUS_BUTTON_R_MID_SLIDE][CUS_BUTTON_STATE_CLICKED] )
			//{
			//	if (!m_bStageChangeFlag)
			//	{
			//		m_bStageChangeFlag = TRUE;

			//		if (g_GameInfo.bMapType)
			//		{
			//			m_iStageNum = 0;
			//			g_GameInfo.bMapType = FALSE;
			//		}
			//		else
			//		{
			//			m_iStageNum = 1;
			//			g_GameInfo.bMapType = TRUE;
			//		}
			//	}
			//}
			//else
			//	m_bStageChangeFlag = FALSE;

			if(g_GameInfo.bMapType)
				pResource = m_pCustomTexture[CUS_STAGEIMG]->GetResource(1);
			else
				pResource = m_pCustomTexture[CUS_STAGEIMG]->GetResource(0);

			//셰이더 파라미터 세팅
			((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
				m_pBuffer, pResource, xmvPercent);
		}
		else // 마우스 오버가 필요 없는 것들
			//셰이더 파라미터 세팅
			((CLogoBackShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
				m_pBuffer, m_pCustomTexture[i]->GetResource(0), xmvPercent);

		m_pBuffer->Render(DXGI_16);
	}
}

HRESULT CCustomButton::AddComponent(void)
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
	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomInfoIcon");
	m_pCustomTexture[CUS_BUTTON_INFO] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomOptionIcon");
	m_pCustomTexture[CUS_BUTTON_SETTING] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomExitButton");
	m_pCustomTexture[CUS_BUTTON_EXIT] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomChangeButton");
	m_pCustomTexture[CUS_BUTTON_CHANGE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomReadyButton");
	m_pCustomTexture[CUS_BUTTON_READY] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomStartButton");
	m_pCustomTexture[CUS_BUTTON_START] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomSlideLButton");
	m_pCustomTexture[CUS_BUTTON_L_UP_SLIDE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomSlideRButton");
	m_pCustomTexture[CUS_BUTTON_R_UP_SLIDE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomSlideLButton");
	m_pCustomTexture[CUS_BUTTON_L_MID_SLIDE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomSlideRButton");
	m_pCustomTexture[CUS_BUTTON_R_MID_SLIDE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomSlideLButton");
	m_pCustomTexture[CUS_BUTTON_L_DOWN_SLIDE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomSlideRButton");
	m_pCustomTexture[CUS_BUTTON_R_DOWN_SLIDE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomHostIcon");
	m_pCustomTexture[CUS_ICON_HOST] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	/*pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomModeText");
	m_pCustomTexture[CUS_ICON_MODETEXT_SINGLE] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomModeText");
	m_pCustomTexture[CUS_ICON_MODETEXT_TEAM] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));*/

	pComponent = CResourceMgr::GetInstance()->CloneResource(CResourceMgr::RESOURCE_ATTRI_DYNAMIC, CResourceMgr::RESOURCE_TYPE_TEXTURE, L"Texture_CustomStageImg");
	m_pCustomTexture[CUS_STAGEIMG] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_LogoBack"));

	return S_OK;
}

HRESULT CCustomButton::ButtonSetting(void)
{
	//CUS_BUTTON_INFO
	m_fSizeX[CUS_BUTTON_INFO] = WINCX / 38.f;								m_fSizeY[CUS_BUTTON_INFO] = WINCY / 25.f;
	m_fX[CUS_BUTTON_INFO] = WINCX - m_fSizeX[CUS_BUTTON_INFO] * 8;			m_fY[CUS_BUTTON_INFO] = m_fSizeY[CUS_BUTTON_INFO] * 1.5f;

	//CUS_BUTTON_SETTING
	m_fSizeX[CUS_BUTTON_SETTING] = WINCX / 38.f;							m_fSizeY[CUS_BUTTON_SETTING] = WINCY / 25.f;
	m_fX[CUS_BUTTON_SETTING] = WINCX - m_fSizeX[CUS_BUTTON_SETTING] * 5;	m_fY[CUS_BUTTON_SETTING] = m_fSizeY[CUS_BUTTON_INFO] * 1.5f;

	//CUS_BUTTON_EXIT
	m_fSizeX[CUS_BUTTON_EXIT] = WINCX / 38.f;								m_fSizeY[CUS_BUTTON_EXIT] = WINCY / 25.f;
	m_fX[CUS_BUTTON_EXIT] = WINCX - m_fSizeX[CUS_BUTTON_SETTING] * 2;		m_fY[CUS_BUTTON_EXIT] = m_fSizeY[CUS_BUTTON_INFO] * 1.5f;

	//CUS_BUTTON_CHANGE
	m_fSizeX[CUS_BUTTON_CHANGE] = WINCX / 17.f;								m_fSizeY[CUS_BUTTON_CHANGE] = WINCY / 25.f;
	m_fX[CUS_BUTTON_CHANGE] = m_fSizeX[CUS_BUTTON_CHANGE] * 2.5f;			m_fY[CUS_BUTTON_CHANGE] = WINCY - m_fSizeY[CUS_BUTTON_CHANGE] * 7;

	//CUS_BUTTON_READY
	m_fSizeX[CUS_BUTTON_READY] = WINCX / 8.f;								m_fSizeY[CUS_BUTTON_READY] = WINCY / 12.f;
	m_fX[CUS_BUTTON_READY] = m_fSizeX[CUS_BUTTON_READY] * 1.2f;				m_fY[CUS_BUTTON_READY] = WINCY - m_fSizeY[CUS_BUTTON_READY] * 1.5f;

	//CUS_BUTTON_START
	m_fSizeX[CUS_BUTTON_START] = WINCX / 8.f;								m_fSizeY[CUS_BUTTON_START] = WINCY / 12.f;
	m_fX[CUS_BUTTON_START] = m_fSizeX[CUS_BUTTON_START] * 1.2f;				m_fY[CUS_BUTTON_START] = WINCY - m_fSizeY[CUS_BUTTON_START] * 1.5f;

	//CUS_BUTTON_L_UP_SLIDE
	m_fSizeX[CUS_BUTTON_L_UP_SLIDE] = WINCX / 60.f;											m_fSizeY[CUS_BUTTON_L_UP_SLIDE] = WINCY / 35.f;
	m_fX[CUS_BUTTON_L_UP_SLIDE] = WINCX / 2.f - m_fSizeX[CUS_BUTTON_L_UP_SLIDE] * 6.5f;		m_fY[CUS_BUTTON_L_UP_SLIDE] = WINCY - m_fSizeY[CUS_BUTTON_L_UP_SLIDE] * 9;

	//CUS_BUTTON_R_UP_SLIDE
	m_fSizeX[CUS_BUTTON_R_UP_SLIDE] = WINCX / 60.f;											m_fSizeY[CUS_BUTTON_R_UP_SLIDE] = WINCY / 35.f;
	m_fX[CUS_BUTTON_R_UP_SLIDE] = WINCX / 2.f + m_fSizeX[CUS_BUTTON_R_UP_SLIDE] * 6.3f;		m_fY[CUS_BUTTON_R_UP_SLIDE] = WINCY - m_fSizeY[CUS_BUTTON_R_UP_SLIDE] * 9;

	//CUS_BUTTON_L_MID_SLIDE
	m_fSizeX[CUS_BUTTON_L_MID_SLIDE] = WINCX / 60.f;										m_fSizeY[CUS_BUTTON_L_MID_SLIDE] = WINCY / 35.f;
	m_fX[CUS_BUTTON_L_MID_SLIDE] = WINCX / 2.f - m_fSizeX[CUS_BUTTON_L_MID_SLIDE] * 6.5f;	m_fY[CUS_BUTTON_L_MID_SLIDE] = WINCY - m_fSizeY[CUS_BUTTON_L_MID_SLIDE] * 5.6f;

	//CUS_BUTTON_R_MID_SLIDE
	m_fSizeX[CUS_BUTTON_R_MID_SLIDE] = WINCX / 60.f;										m_fSizeY[CUS_BUTTON_R_MID_SLIDE] = WINCY / 35.f;
	m_fX[CUS_BUTTON_R_MID_SLIDE] = WINCX / 2.f + m_fSizeX[CUS_BUTTON_R_MID_SLIDE] * 6.3f;	m_fY[CUS_BUTTON_R_MID_SLIDE] = WINCY - m_fSizeY[CUS_BUTTON_R_MID_SLIDE] * 5.6f;

	//CUS_BUTTON_L_DOWN_SLIDE
	m_fSizeX[CUS_BUTTON_L_DOWN_SLIDE] = WINCX / 60.f;										m_fSizeY[CUS_BUTTON_L_DOWN_SLIDE] = WINCY / 35.f;
	m_fX[CUS_BUTTON_L_DOWN_SLIDE] = WINCX / 2.f - m_fSizeX[CUS_BUTTON_L_DOWN_SLIDE] * 6.5f;	m_fY[CUS_BUTTON_L_DOWN_SLIDE] = WINCY - m_fSizeY[CUS_BUTTON_L_DOWN_SLIDE] * 2.2f;

	//CUS_BUTTON_R_DOWN_SLIDE
	m_fSizeX[CUS_BUTTON_R_DOWN_SLIDE] = WINCX / 60.f;										m_fSizeY[CUS_BUTTON_R_DOWN_SLIDE] = WINCY / 35.f;
	m_fX[CUS_BUTTON_R_DOWN_SLIDE] = WINCX / 2.f + m_fSizeX[CUS_BUTTON_R_DOWN_SLIDE] * 6.3f;	m_fY[CUS_BUTTON_R_DOWN_SLIDE] = WINCY - m_fSizeY[CUS_BUTTON_R_DOWN_SLIDE] * 2.2f;

	//CUS_ICON_HOST
	m_fSizeX[CUS_ICON_HOST] = WINCX / 70.f;										m_fSizeY[CUS_ICON_HOST] = WINCY / 45.f;
	// 좌측 -> * 3.f // 우측 -> WINCX - SizeX * 3.f;							// 최상단 -> * 9.f // 중단 -> * 16.f // 하단 -> 23.5f
	m_fX[CUS_ICON_HOST] = m_fSizeX[CUS_ICON_HOST] * 3.f;						m_fY[CUS_ICON_HOST] = m_fSizeY[CUS_ICON_HOST] * 23.5f;

	////CUS_ICON_MODETEXT_SINGLE
	//m_fSizeX[CUS_ICON_MODETEXT_SINGLE] = WINCX / 40.f;						m_fSizeY[CUS_ICON_MODETEXT_SINGLE] = WINCY / 50.f;
	//m_fX[CUS_ICON_MODETEXT_SINGLE] = WINCX / 2.f;								m_fY[CUS_ICON_MODETEXT_SINGLE] = WINCY - m_fSizeY[CUS_ICON_MODETEXT_SINGLE] * 13;

	//CUS_KJKPLAZA
	m_fSizeX[CUS_STAGEIMG] = WINCX / 8.f;										m_fSizeY[CUS_STAGEIMG] = WINCY / 4.5f;
	m_fX[CUS_STAGEIMG] = WINCX / 2.f;											m_fY[CUS_STAGEIMG] = WINCY / 2.f - m_fSizeY[CUS_STAGEIMG] * 0.3f;

	return S_OK;
}

bool CCustomButton::GetButtonState(const DWORD & dwButtonType, const DWORD & dwButtonState)
{
	return m_bButtonState[dwButtonType][dwButtonState];
}

void CCustomButton::ResetButtonState(void)
{
	for (int i = 0; i < CUS_BUTTON_END; ++i)
	{
		m_bButtonState[i][CUS_BUTTON_STATE_CHECKED] = false;
	}
}

HRESULT CCustomButton::InitHostPos(void)
{
	D3DXVECTOR2 vPos;
	
	// 좌 상단
	vPos = D3DXVECTOR2(m_fSizeX[CUS_ICON_HOST] * 3.f, m_fSizeY[CUS_ICON_HOST] * 9.f);
	m_vecHostIconPos.push_back(vPos);
	// 우 상단
	vPos = D3DXVECTOR2(WINCX - m_fSizeX[CUS_ICON_HOST] * 3.f, m_fSizeY[CUS_ICON_HOST] * 9.f);
	m_vecHostIconPos.push_back(vPos);
	// 좌 중단
	vPos = D3DXVECTOR2(m_fSizeX[CUS_ICON_HOST] * 3.f, m_fSizeY[CUS_ICON_HOST] * 16.f);
	m_vecHostIconPos.push_back(vPos);
	// 우 중단
	vPos = D3DXVECTOR2(WINCX - m_fSizeX[CUS_ICON_HOST] * 3.f, m_fSizeY[CUS_ICON_HOST] * 16.f);
	m_vecHostIconPos.push_back(vPos);
	// 좌 하단
	vPos = D3DXVECTOR2(m_fSizeX[CUS_ICON_HOST] * 3.f, m_fSizeY[CUS_ICON_HOST] * 23.5f);
	m_vecHostIconPos.push_back(vPos);
	// 우 하단
	vPos = D3DXVECTOR2(WINCX - m_fSizeX[CUS_ICON_HOST] * 3.f, m_fSizeY[CUS_ICON_HOST] * 23.5f);
	m_vecHostIconPos.push_back(vPos);

	return S_OK;
}

HRESULT CCustomButton::HostIconSetting(void)
{
	bool bIsClient = FALSE;

	unordered_map<wstring, list<CLIENT*>>::iterator iterMap = g_mapClient.begin();
	unordered_map<wstring, list<CLIENT*>>::iterator iterMap_end = g_mapClient.end();

	// 맵을 순회하며 방장을 찾는다.
	for (iterMap; iterMap != iterMap_end; ++iterMap)
	{
		if (iterMap->second.front()->m_bIsHost)
		{
			m_fX[CUS_ICON_HOST] = m_vecHostIconPos[iterMap->second.front()->m_iRoomIndex].x;
			m_fY[CUS_ICON_HOST] = m_vecHostIconPos[iterMap->second.front()->m_iRoomIndex].y;
			bIsClient = TRUE;
		}
	}
	
	// 클라이언트가 없을 시 무조건 0번이 방장이된다
	if (!bIsClient)
	{
		m_fX[CUS_ICON_HOST] = m_vecHostIconPos[0].x;
		m_fY[CUS_ICON_HOST] = m_vecHostIconPos[0].y;
	}

	return S_OK;
}

CCustomButton * CCustomButton::Create(CDevice * pDevice)
{
	CCustomButton*	pButton = new CCustomButton(pDevice);

	if (FAILED(pButton->Init()))
		Safe_Delete(pButton);

	return pButton;
}

void CCustomButton::Release()
{
	Safe_Release(m_pShader);
}

void CCustomButton::RenderForShadow(LightMatrixStruct tLight)
{
}

void CCustomButton::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}
