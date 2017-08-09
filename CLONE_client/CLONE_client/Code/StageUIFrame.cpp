#include "stdafx.h"
#include "StageUIFrame.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "SceneMgr.h"
#include "TimeMgr.h"
#include "InputMgr.h"
#include "SoundMgr.h"
//Object
#include "TextureShader.h"
#include "Player.h"
#include "Bead.h"
#include "Scene.h"
#include "Font.h"
#include "KillLog.h"
//Server
#include "ServerMgr.h"

CStageUIFrame::CStageUIFrame(CDevice * pDevice)
	: CUI(pDevice)
	, m_bReady(FALSE)
	, m_pPlayTime(nullptr)
	, m_pReadyCount(nullptr)
	, m_bChatOn(FALSE)
	, m_bChatKeyDown(FALSE)
	, m_pObjList(NULL)
	, m_fShowResultDelay(2.f)
	, m_iKillLogCnt(0)
{
	m_pReadyCount = CServerMgr::GetInstance()->GetCountDown();

	for (size_t i = 0; i < UIFRAME_END; ++i)
	{
		D3DXMatrixIdentity(&CStageUIFrame::m_matUIView[i]);
		D3DXMatrixIdentity(&CStageUIFrame::m_matUIProj[i]);
	}
	for (size_t i = 0; i < BEAD_END; ++i)
	{
		D3DXMatrixIdentity(&CStageUIFrame::m_matBeadView[i]);
		D3DXMatrixIdentity(&CStageUIFrame::m_matBeadProj[i]);
	}
	for (size_t i = 0; i < FONT_END; ++i)
		m_pFont[i] = NULL;

	m_vKillLog.clear();
}

CStageUIFrame::~CStageUIFrame()
{
	Release();
}

HRESULT CStageUIFrame::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_UI;

	// Up Frame -------------------------------------------
	CStageUIFrame::m_fSizeX[UIFRAME_UPFRAME] = WINCX / 5.0f;
	CStageUIFrame::m_fSizeY[UIFRAME_UPFRAME] = WINCY / 14.f;

	CStageUIFrame::m_fX[UIFRAME_UPFRAME] = WINCX / 2.f;
	CStageUIFrame::m_fY[UIFRAME_UPFRAME] = WINCY / 11.f;

	// Left Frame -----------------------------------------
	CStageUIFrame::m_fSizeX[UIFRAME_LEFTFRAME] = WINCX / 20.f;
	CStageUIFrame::m_fSizeY[UIFRAME_LEFTFRAME] = WINCY / 5.f;

	CStageUIFrame::m_fX[UIFRAME_LEFTFRAME] = WINCX / 15.f;
	CStageUIFrame::m_fY[UIFRAME_LEFTFRAME] = WINCY / 2.f;

	// Notice Frame ---------------------------------------
	CStageUIFrame::m_fSizeX[UIFRAME_NOTICEFRAME] = WINCX / 5.f;
	CStageUIFrame::m_fSizeY[UIFRAME_NOTICEFRAME] = WINCY / 14.f;

	CStageUIFrame::m_fSizeX[UIFRAME_NOTICECOUNTFRAME] = WINCX / 15.f;
	CStageUIFrame::m_fSizeY[UIFRAME_NOTICECOUNTFRAME] = WINCY / 10.f;

	CStageUIFrame::m_fX[UIFRAME_NOTICEFRAME] = CStageUIFrame::m_fX[UIFRAME_NOTICECOUNTFRAME] = WINCX / 2.f;
	CStageUIFrame::m_fY[UIFRAME_NOTICEFRAME] = CStageUIFrame::m_fY[UIFRAME_NOTICECOUNTFRAME] = WINCY / 2.f;

	// Chat Frame -----------------------------------------
	CStageUIFrame::m_fSizeX[UIFRAME_CHATFRAME] = WINCX / 5.8f;
	CStageUIFrame::m_fSizeY[UIFRAME_CHATFRAME] = WINCY / 6.f;

	CStageUIFrame::m_fX[UIFRAME_CHATFRAME] = WINCX - WINCX / 7.f;
	CStageUIFrame::m_fY[UIFRAME_CHATFRAME] = WINCY - WINCY / 3.f;

	// VicCheck Frame -------------------------------------
	CStageUIFrame::m_fBeadSizeX = WINCX / 50.f;
	CStageUIFrame::m_fBeadSizeY = WINCY / 25.f;

	CStageUIFrame::m_fBeadX[BEAD_C] = WINCX / 23.5f;
	CStageUIFrame::m_fBeadY[BEAD_C] = WINCY / 2.f - WINCY / 6.9f;

	CStageUIFrame::m_fBeadX[BEAD_L] = WINCX / 15.f;
	CStageUIFrame::m_fBeadY[BEAD_L] = WINCY / 2.f - WINCY / 13.8f;

	CStageUIFrame::m_fBeadX[BEAD_O] = WINCX / 23.5f;
	CStageUIFrame::m_fBeadY[BEAD_O] = WINCY / 2.f;

	CStageUIFrame::m_fBeadX[BEAD_N] = WINCX / 15.f;
	CStageUIFrame::m_fBeadY[BEAD_N] = WINCY / 2.f + WINCY / 13.8f;
	
	CStageUIFrame::m_fBeadX[BEAD_E] = WINCX / 23.5f;
	CStageUIFrame::m_fBeadY[BEAD_E] = WINCY / 2.f + WINCY / 6.9f;

	// Font Setting ---------------------------------------
	FontSetting();

	return S_OK;
}

int CStageUIFrame::Update()
{
	if (m_pObjList == NULL)	// Scene 내의 ObjList 연결
		m_pObjList = const_cast<unordered_map<wstring, list<CObj*>>*>(CSceneMgr::GetInstance()->GetScene()->GetObjList(CLayer::LAYERTYPE_GAMELOGIC));

	//Key Check
	KeyCheck();

	//Frame Update
	for (size_t i = 0; i < UIFRAME_END; ++i)
	{
		D3DXMatrixOrthoLH(&CStageUIFrame::m_matUIProj[i], WINCX, WINCY, 0.f, 1.f);
		CStageUIFrame::m_matUIView[i]._11 = m_fSizeX[i];
		CStageUIFrame::m_matUIView[i]._22 = m_fSizeY[i];
		CStageUIFrame::m_matUIView[i]._33 = 1.f;

		CStageUIFrame::m_matUIView[i]._41 = m_fX[i] - (WINCX >> 1);
		CStageUIFrame::m_matUIView[i]._42 = -m_fY[i] + (WINCY >> 1);
	}

	//BeadCheck Update
	for (size_t i = 0; i < BEAD_END; ++i)
	{
		D3DXMatrixOrthoLH(&CStageUIFrame::m_matBeadProj[i], WINCX, WINCY, 0.f, 1.f);
		CStageUIFrame::m_matBeadView[i]._11 = m_fBeadSizeX;
		CStageUIFrame::m_matBeadView[i]._22 = m_fBeadSizeY;
		CStageUIFrame::m_matBeadView[i]._33 = 1.f;

		CStageUIFrame::m_matBeadView[i]._41 = m_fBeadX[i] - (WINCX >> 1);
		CStageUIFrame::m_matBeadView[i]._42 = -m_fBeadY[i] + (WINCY >> 1);
	}

	//Font Update
	FontUpdate();

	//Result Update
	if (CServerMgr::GetInstance()->GetStageResultType() != RESULT_END)
	{
		m_eStageState = STAGESTATE_ROUNDOVER;
		m_fShowResultDelay -= CTimeMgr::GetInstance()->GetTime();

		if (m_fShowResultDelay <= 0.f)
		{
			m_fShowResultDelay = 0.f;
			switch (CServerMgr::GetInstance()->GetStageResultType())
			{
			case RESULT_WIN:
#ifdef _PLAY_SOUND
				CSoundMgr::GetInstance()->PlayUISound(L"win_ES.mp3");
#endif
				m_eStageState = STAGESTATE_VIC;
				break;
			case RESULT_DEFEAT:
#ifdef _PLAY_SOUND
				CSoundMgr::GetInstance()->PlayUISound(L"defeat_ES.mp3");
#endif
				m_eStageState = STAGESTATE_DEFT;
				break;
			default: break;
			}
		}
	}

	//KillLog Update
	KillLogUpdate();

	CObj::Update_Component();

	return 0;
}

void CStageUIFrame::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matView;
	XMFLOAT4X4 matProj;

	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	
	// Draw Bead Check ----------------------------------
	RenderBeadCheck(matWorld, matView, matProj);

	// Up Frame -------------------------------------------
	memcpy(&matView, CStageUIFrame::m_matUIView[UIFRAME_UPFRAME], sizeof(float) * 16);
	memcpy(&matProj, CStageUIFrame::m_matUIProj[UIFRAME_UPFRAME], sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_UPFRAME]->GetResource(UIFRAME_UPFRAME));

	m_pBuffer->Render(DXGI_16);

	// Left Frame -----------------------------------------
	// Stage1 에서만 그린다.
	if (g_GameInfo.bMapType == MAP_KJK_PLAZA)
	{
		memcpy(&matView, CStageUIFrame::m_matUIView[UIFRAME_LEFTFRAME], sizeof(float) * 16);
		memcpy(&matProj, CStageUIFrame::m_matUIProj[UIFRAME_LEFTFRAME], sizeof(float) * 16);

		//셰이더 파라미터 세팅
		((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_UPFRAME]->GetResource(UIFRAME_LEFTFRAME));

		m_pBuffer->Render(DXGI_16);
	}

	// Notice Frame -------------------------------------
	RenderNotice(matWorld, matView, matProj);

	// Chat Frame ---------------------------------------
	RenderChat(matWorld, matView, matProj);

	// KillLog Render -----------------------------------
	RenderKillLog();

	// Font ---------------------------------------------
	m_pFont[FONT_TIME]->Render();
	m_pFont[FONT_PLAYERCNT]->Render();
	m_pFont[FONT_BOTCNT]->Render();
}

void CStageUIFrame::RenderNotice(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj)
{
	if (m_eStageState == STAGESTATE_PLAY)
		return;

	// Count Texture 변수
	int iTexCount = 0;

	switch (m_eStageState)
	{
	case STAGESTATE_READY:
		if ((*m_pReadyCount) == 0)
		{
			//레디카운트가 0보다 작아지면 Play로 상태를 바꾸고 안그리고 리턴
#ifdef _PLAY_SOUND
			CSoundMgr::GetInstance()->PlayUISound(L"start_ES2.mp3");
#endif
			m_eStageState = STAGESTATE_PLAY;
			return;
		}
		// Notice Frame ---------------------------------------
		memcpy(&matView, CStageUIFrame::m_matUIView[UIFRAME_NOTICECOUNTFRAME], sizeof(float) * 16);
		memcpy(&matProj, CStageUIFrame::m_matUIProj[UIFRAME_NOTICECOUNTFRAME], sizeof(float) * 16);
		//셰이더 파라미터 세팅
		((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_NOTICECOUNTFRAME]->GetResource(*m_pReadyCount));
		break;
	case STAGESTATE_DEFT:
		// Notice Frame ---------------------------------------
		memcpy(&matView, CStageUIFrame::m_matUIView[UIFRAME_NOTICEFRAME], sizeof(float) * 16);
		memcpy(&matProj, CStageUIFrame::m_matUIProj[UIFRAME_NOTICEFRAME], sizeof(float) * 16);
		//셰이더 파라미터 세팅
		((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_NOTICEFRAME]->GetResource(0));
		break;
	case STAGESTATE_VIC:
		// Notice Frame ---------------------------------------
		memcpy(&matView, CStageUIFrame::m_matUIView[UIFRAME_NOTICEFRAME], sizeof(float) * 16);
		memcpy(&matProj, CStageUIFrame::m_matUIProj[UIFRAME_NOTICEFRAME], sizeof(float) * 16);
		//셰이더 파라미터 세팅
		((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_NOTICEFRAME]->GetResource(2));
		break;
	case STAGESTATE_ROUNDOVER:
		// Notice Frame ---------------------------------------
		memcpy(&matView, CStageUIFrame::m_matUIView[UIFRAME_NOTICEFRAME], sizeof(float) * 16);
		memcpy(&matProj, CStageUIFrame::m_matUIProj[UIFRAME_NOTICEFRAME], sizeof(float) * 16);
		//셰이더 파라미터 세팅
		((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_NOTICEFRAME]->GetResource(1));
		break;
	}

	m_pBuffer->Render(DXGI_16);
}

void CStageUIFrame::RenderChat(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj)
{
	if (!m_bChatOn)
		return;

	memcpy(&matView, CStageUIFrame::m_matUIView[UIFRAME_CHATFRAME], sizeof(float) * 16);
	memcpy(&matProj, CStageUIFrame::m_matUIProj[UIFRAME_CHATFRAME], sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
		m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_CHATFRAME]->GetResource(0));

	m_pBuffer->Render(DXGI_16);
}

void CStageUIFrame::RenderBeadCheck(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj)
{
	if (m_pObjList != NULL)
	{
		list<CObj*>::iterator iter_Beadlist = (*m_pObjList)[L"Bead"].begin();
		list<CObj*>::iterator iter_Beadlist_end = (*m_pObjList)[L"Bead"].end();

		for (iter_Beadlist; iter_Beadlist != iter_Beadlist_end; ++iter_Beadlist)
		{
			//구슬의 소유주가 있음
			int iBeadOwnerID = reinterpret_cast<CBead*>(*iter_Beadlist)->GetOwnerID();
			if (iBeadOwnerID != UNKNOWN_VALUE)
			{
				//내가 소유할때 그리기
				if (iBeadOwnerID == CServerMgr::GetInstance()->GetClientID())
				{
					int iBeadID = reinterpret_cast<CBead*>(*iter_Beadlist)->GetBeadType();
					memcpy(&matView, CStageUIFrame::m_matBeadView[(iBeadID)], sizeof(float) * 16);
					memcpy(&matProj, CStageUIFrame::m_matBeadProj[(iBeadID)], sizeof(float) * 16);

					//셰이더 파라미터 세팅
					((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
						m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_CHECKVIC]->GetResource(0));

					m_pBuffer->Render(DXGI_16);
				}
				//우리팀이 소유할때 그리기
				else
				{
					list<CObj*>::iterator iter_Enemylist = (*m_pObjList)[L"Enemy"].begin();
					list<CObj*>::iterator iter_Enemylist_end = (*m_pObjList)[L"Enemy"].end();
					for (iter_Enemylist; iter_Enemylist != iter_Enemylist_end; ++iter_Enemylist)
					{
						//우리팀 확인
						if (reinterpret_cast<CPlayer*>(*iter_Enemylist)->GetPlayerIsMyTeam() == true)
						{
							int iBeadID = reinterpret_cast<CBead*>(*iter_Beadlist)->GetBeadType();
							memcpy(&matView, CStageUIFrame::m_matBeadView[(iBeadID)], sizeof(float) * 16);
							memcpy(&matProj, CStageUIFrame::m_matBeadProj[(iBeadID)], sizeof(float) * 16);

							//셰이더 파라미터 세팅
							((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
								m_pBuffer, m_pLightInfo, m_pUITexture[UIFRAME_CHECKVIC]->GetResource(0));

							m_pBuffer->Render(DXGI_16);
							break;
						}
					}
				}
			}
		}
	}
}

void CStageUIFrame::RenderKillLog(void)
{
	for (auto iter = m_vKillLog.begin(); iter != m_vKillLog.end(); ++iter)
	{
		(*iter)->Render();
	}
}

void CStageUIFrame::FontSetting(void)
{
	//Font Setting
	for (size_t i = 0; i < FONT_END; ++i)
	{
		m_pFont[i]->m_eType = CFont::FONT_TYPE_OUTLINE;
		m_pFont[i]->m_nColor = 0xFFFFFFFF;
		m_pFont[i]->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
		switch (i)
		{
		case FONT_TIME:
			m_pFont[i]->m_wstrText = L"TIME CHECK";
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 2.f - WINCX / 78.f, WINCY / 9.5f);
			m_pFont[i]->m_fSize = WINCY / 50.f;
			break;
		case FONT_PLAYERCNT:
			m_pFont[i]->m_wstrText = L"PLAYER";
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 2.f - WINCX / 9.5f, WINCY / 11.f);
			m_pFont[i]->m_fSize = WINCY / 45.f;
			break;
		case FONT_BOTCNT:
			m_pFont[i]->m_wstrText = L"BOT";
			m_pFont[i]->m_vPos = D3DXVECTOR2(WINCX / 2.f + WINCX / 7.4f, WINCY / 11.f);
			m_pFont[i]->m_fSize = WINCY / 45.f;
			break;
		}
		m_pFont[i]->m_fOutlineSize = 1.f;
		m_pFont[i]->m_nOutlineColor = 0xFF000000;
	}
}

void CStageUIFrame::FontUpdate(void)
{
	// Time Font Update
	int iMin = 0, iSec = 0;
	
	if (m_eStageState == STAGESTATE_PLAY)
	{
		iMin = (*m_pPlayTime) / 60;
		iSec = (*m_pPlayTime) % 60;
	}

	wstring wMin = to_wstring(iMin);
	wstring wSemicol = L" : ";
	wstring wSec = to_wstring(iSec);
	wstring wTime = wMin + wSemicol + wSec;
	m_pFont[FONT_TIME]->m_wstrText = wTime;

	// Bot, Player Font Update
	int iPlayerCnt = 0, iBotCnt = 0;

	unordered_map<wstring, list<CObj*>>::iterator iter = m_pObjList->begin();
	unordered_map<wstring, list<CObj*>>::iterator iter_end = m_pObjList->end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObj*> plist = iter->second;

		list<CObj*>::iterator iter_list = plist.begin();
		list<CObj*>::iterator iter_list_end = plist.end();

		for(iter_list; iter_list != iter_list_end; ++iter_list)
		{
			OBJTYPE		eType	= (*iter_list)->GetObjType();
			OBJSTATE	eState	= (*iter_list)->GetObjState();

			if (eType == OBJ_PLAYER)
			{
				if(eState != OBJ_STATE_DIE)
					iPlayerCnt++;
			}	
			else if (eType == OBJ_BOT)
			{
				if(eState != OBJ_STATE_DIE)
					iBotCnt++;
			}
		}
	}

	m_pFont[FONT_PLAYERCNT]->m_wstrText = to_wstring(iPlayerCnt);
	m_pFont[FONT_BOTCNT]->m_wstrText = to_wstring(iBotCnt);
}

void CStageUIFrame::KillLogUpdate(void)
{
	bool bChange = FALSE;

	vector<CKillLog*>::iterator iter = m_vKillLog.begin();
	vector<CKillLog*>::iterator iter_end = m_vKillLog.end();

	for (iter; iter != iter_end; )
	{
		if ((*iter)->Update() == CObj::OBJ_STATE_DIE)
		{
			m_iKillLogCnt--;
			bChange = TRUE;

			Safe_Delete(*iter);
			iter = m_vKillLog.erase(iter);

			break;
		}
		else
			++iter;
	}

	if (bChange)
	{
		iter = m_vKillLog.begin();
		iter_end = m_vKillLog.end();

		for (iter; iter != iter_end; ++iter)
		{
			D3DXVECTOR3 vOrgPos = (*iter)->GetKillLogPos();
			(*iter)->SetKillLogPos(D3DXVECTOR3(vOrgPos.x, vOrgPos.y - KILLLOG_AUG_Y, 1.f));
		}
	}
}

void CStageUIFrame::KeyCheck(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();

	if (CInput::GetInstance()->GetDIKeyState(DIK_RETURN) & 0x80)
	{
		if (!m_bChatKeyDown)
		{
			m_bChatKeyDown = TRUE;
			if (m_bChatOn)
				m_bChatOn = FALSE;
			else
				m_bChatOn = TRUE;
		}
	}
	else
		m_bChatKeyDown = FALSE;
}

void CStageUIFrame::AddKillLog(const wstring & wstrKiller, const wstring & wstrVictim)
{
	m_iKillLogCnt++;	// 우선 킬로그 숫자를 올린다.
	
	CKillLog* pKillLog = CKillLog::Create(m_pDevice);
	NULL_CHECK_RETURN(pKillLog, );
	pKillLog->SetLogStr(wstrKiller, wstrVictim);
	pKillLog->SetKillLogPos(D3DXVECTOR3(KILLLOG_ORIGEN_X, KILLLOG_ORIGEN_Y * m_iKillLogCnt, 1.f));

	m_vKillLog.push_back(pKillLog);	// Vector에 PushBack
}

HRESULT CStageUIFrame::AddComponent(void)
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

	//Up, Left Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_UIFrame");
	//Upframe 텍스쳐에 left텍스쳐도 들어있다.
	m_pUITexture[UIFRAME_UPFRAME] = dynamic_cast<CTexture*>(pComponent);	
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Notice Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_UINotice");
	m_pUITexture[UIFRAME_NOTICEFRAME] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Chat Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_UIChatBoard");
	m_pUITexture[UIFRAME_CHATFRAME] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//NoticeCount Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_UINoticeCount");
	m_pUITexture[UIFRAME_NOTICECOUNTFRAME] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//VicCheck Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_UICheckVic");
	m_pUITexture[UIFRAME_CHECKVIC] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Font
	for (size_t i = 0; i < FONT_END; ++i)
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
		L"Shader_Texture"));

	return S_OK;
}

CStageUIFrame * CStageUIFrame::Create(CDevice * pDevice)
{
	CStageUIFrame*	pObj = new CStageUIFrame(pDevice);

	if (FAILED(pObj->Init()))
		Safe_Delete(pObj);

	return pObj;
}

void CStageUIFrame::Release()
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);

	m_pReadyCount = nullptr;
	m_pPlayTime = nullptr;
	m_pObjList = nullptr;
}

void CStageUIFrame::RenderForShadow(LightMatrixStruct tLight)
{
}

void CStageUIFrame::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}
