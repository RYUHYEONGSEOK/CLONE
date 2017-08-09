#include "stdafx.h"
#include "Stage2.h"
//Manager
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "CollisionMgr.h"
#include "SceneMgr.h"
#include "TimeMgr.h"
#include "NaviMgr.h"
#include "Frustum.h"
#include "SoundMgr.h"
#include "ServerMgr.h"
//Object
#include "Renderer.h"
#include "Loading.h"
#include "Terrain.h"
#include "Player.h"
#include "SkyBox.h"
#include "DumpPlate.h"
#include "DynamicCam.h"
//#include "Button.h"
#include "Status.h"
#include "MeshMap2.h"
#include "Bot.h"
#include "StageUIFrame.h"
#include "Enemy.h"
#include "Bead.h"
#include "TrapWater.h"

CStage2::CStage2(CDevice * pDevice)
	: CScene(pDevice)
	, m_pStateUI(NULL)
	, m_pPlayTime(nullptr)
{
}

CStage2::~CStage2(void)
{
	Release();
}

HRESULT CStage2::InitScene(void)
{
	//Test Value
	m_eStageState = STAGESTATE_READY;
	m_pPlayTime = CServerMgr::GetInstance()->GetGameTime();

	//우선 RenderGroup을 Clear 한다.
	CSceneMgr::GetInstance()->Clear_RenderGroup();

	//Obj 추가
	FAILED_CHECK(Add_Light());
	FAILED_CHECK(Add_Environment_Layer());
	FAILED_CHECK(Add_GameLogic_Layer());
	FAILED_CHECK(Add_UI_Layer());
	FAILED_CHECK(Add_RenderTarget());
	FAILED_CHECK(Connect_Cam_To_Navi());

#ifdef _PLAY_SOUND
	// BGM
	CSoundMgr::GetInstance()->PlayBGM(L"ingame_bgm.mp3");
#endif

	return S_OK;
}

void CStage2::Update(void)
{
#ifdef _PLAY_SOUND
	if (!m_bFirstInit)
	{
		m_bFirstInit = TRUE;
		CSoundMgr::GetInstance()->PlaySoundW(L"start_ES.wav");
	}
#endif

	//대기실로 이동
	if ((m_eStageState == STAGESTATE_VIC || m_eStageState == STAGESTATE_DEFT)
		&& CServerMgr::GetInstance()->GetIsChangeToRoomScene())
	{
		CServerMgr::GetInstance()->SetChangeScene(0, false);
		CServerMgr::GetInstance()->SetStageResultType(RESULT_END);

		CRenderTargetMgr::GetInstance()->DestroyInstance();
		CLightMgr::GetInstance()->DestroyInstance();
		CSceneMgr::GetInstance()->SetSceneAftherLoading(CSceneMgr::SCENE_ID_CUSTOM);
		return;
	}

	// 스테이트를 UI에서 계속 받아온다 상태값이 바뀔때만 Layer의 스테이트를 갱신한다.
	STAGESTATETYPE eState = m_pStateUI->GetStageState();
	if (eState != m_eStageState)
	{
		m_eStageState = eState;
		CScene::ChangeState_Layer(m_eStageState);
	}

	//Frustum Update
	CFrustum::GetInstance()->UpdateFrustum(m_pMainCamera);

	CScene::Update_Layer();
}

void CStage2::Render(void)
{
	//네비매쉬 랜더
	#ifdef _DEBUG
	CNaviMgr::GetInstance()->Render_NaviMesh();
	#endif
	
	//Render는 렌더러가 한다
	//CScene::Render_Layer();
}

CStage2 * CStage2::Create(CDevice * pDevice)
{
	CStage2* pStage = new CStage2(pDevice);

	if (FAILED(pStage->InitScene()))
		Safe_Delete(pStage);

	return pStage;
}

HRESULT CStage2::Add_Environment_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	//Cam
	pObj = CDynamicCamera::Create(m_pDevice,
		&D3DXVECTOR3(0.f, 20.f, HALFMAPZ), &D3DXVECTOR3(1.f, -1.f, -1.f));
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetObjID(L"Camera");
	pLayer->AddObj(L"Camera", pObj);
	m_pMainCamera = dynamic_cast<CCamera*>(pObj);

	//SkyBox
	pObj = CSkyBox::Create(m_pDevice, OBJ_ETC);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetObjID(L"SkyBox");
	pObj->SetCam(m_pMainCamera);
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_PRIORITY, pObj);
	pLayer->AddObj(L"SkyBox", pObj);

	m_pLayer[CLayer::LAYERTYPE_ENVIRONMENT] = pLayer;

	return S_OK;
}

HRESULT CStage2::Add_GameLogic_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	//StaticMesh Map
	pObj = CMeshMap2::Create(m_pDevice, OBJ_ETC);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetCam(m_pMainCamera);
	pObj->SetObjID(L"MeshMap");
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
	pLayer->AddObj(L"MeshMap", pObj);

	//DumpPlate
	pObj = CDumpPlate::Create(m_pDevice, OBJ_ETC);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetCam(m_pMainCamera);
	pObj->SetObjID(L"DumpPlate");
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
	pLayer->AddObj(L"DumpPlate", pObj);

	//Player --------------------------------------------------------------------
	int iClientID = CServerMgr::GetInstance()->GetClientID();
	if (g_GameInfo.bMode == true)	//team
	{
		bool bIsMyColor = CServerMgr::GetInstance()->GetPlayerIsRedOrBlue()[iClientID];
		for (int i = 0; i < MAX_USER; ++i)
		{
			D3DXVECTOR3 vPos = (CServerMgr::GetInstance()->GetPlayerPos())[i];
			bool bIsTeamColor = (CServerMgr::GetInstance()->GetPlayerIsRedOrBlue())[i];
			if (i == iClientID)
			{
				pObj = CPlayer::Create(m_pDevice, OBJ_PLAYER);
				NULL_CHECK_RETURN(pObj, E_FAIL);
				pObj->SetPos(vPos);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerID(i);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMe(true);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMyTeam(true);
				pObj->SetObjID(L"Player");
				pObj->SetCam(m_pMainCamera);
				CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
				pLayer->AddObj(L"Player", pObj);
			}
			else if (vPos.y < 0.5f)
			{
				if (bIsTeamColor == bIsMyColor)
				{
					pObj = CPlayer::Create(m_pDevice, OBJ_PLAYER);
					NULL_CHECK_RETURN(pObj, E_FAIL);
					pObj->SetPos(vPos);
					reinterpret_cast<CPlayer*>(pObj)->SetPlayerID(i);
					reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMe(false);
					reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMyTeam(true);
					pObj->SetObjID(L"Enemy");
					pObj->SetCam(m_pMainCamera);
					CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
					pLayer->AddObj(L"Enemy", pObj);
				}
				else if (bIsTeamColor != bIsMyColor)
				{
					pObj = CPlayer::Create(m_pDevice, OBJ_PLAYER);
					NULL_CHECK_RETURN(pObj, E_FAIL);
					pObj->SetPos(vPos);
					reinterpret_cast<CPlayer*>(pObj)->SetPlayerID(i);
					reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMe(false);
					reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMyTeam(false);
					pObj->SetObjID(L"Enemy");
					pObj->SetCam(m_pMainCamera);
					CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
					pLayer->AddObj(L"Enemy", pObj);
				}
			}
		}
	}
	else if (g_GameInfo.bMode == false)	//solo
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			D3DXVECTOR3 vPos = (CServerMgr::GetInstance()->GetPlayerPos())[i];
			if (i == iClientID)
			{
				pObj = CPlayer::Create(m_pDevice, OBJ_PLAYER);
				NULL_CHECK_RETURN(pObj, E_FAIL);
				pObj->SetPos(vPos);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerID(i);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMe(true);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMyTeam(true);
				pObj->SetObjID(L"Player");
				pObj->SetCam(m_pMainCamera);
				CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
				pLayer->AddObj(L"Player", pObj);
			}
			else if (vPos.y < 0.5f)
			{
				pObj = CPlayer::Create(m_pDevice, OBJ_PLAYER);
				NULL_CHECK_RETURN(pObj, E_FAIL);
				pObj->SetPos(vPos);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerID(i);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMe(false);
				reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMyTeam(false);
				pObj->SetObjID(L"Enemy");
				pObj->SetCam(m_pMainCamera);
				CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
				pLayer->AddObj(L"Enemy", pObj);
			}
		}
	}
	//---------------------------------------------------------------------------

	//Bot Create ----------------------------------------------------------------
	int iBotCount = CServerMgr::GetInstance()->GetBotCount();
	for (int i = 0; i < iBotCount; ++i)
	{
		pObj = CBot::Create(m_pDevice, OBJ_BOT);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		reinterpret_cast<CBot*>(pObj)->SetBotID(i);
		pObj->SetPos((CServerMgr::GetInstance()->GetBotPos())[i]);
		pObj->SetObjID(L"Bot");
		pObj->SetCam(m_pMainCamera);
		CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
		pLayer->AddObj(L"Bot", pObj);
	}
	//---------------------------------------------------------------------------

	//StreamWater
	pObj = CTrapWater::Create(m_pDevice, OBJ_ETC);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetObjID(L"TrapWater");
	pObj->SetCam(m_pMainCamera);
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_WATER, pObj);
	pLayer->AddObj(L"TrapWater", pObj);

	m_pLayer[CLayer::LAYERTYPE_GAMELOGIC] = pLayer;


	//LAYERTYPE_GAMELOGIC layer Setting
	CCollisionMgr::GetInstance()->SetObjList(m_pLayer[CLayer::LAYERTYPE_GAMELOGIC]->GetMapObjList());
	CServerMgr::GetInstance()->SetObjList(m_pLayer[CLayer::LAYERTYPE_GAMELOGIC]->GetMapObjList());


	//clear send data + change scene
	CServerMgr::GetInstance()->SetChangeScene(1, false);
	CServerMgr::GetInstance()->ClearSendInitData();

	return S_OK;
}

HRESULT CStage2::Add_UI_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	//StageUIFrame
	pObj = CStageUIFrame::Create(m_pDevice);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetObjID(L"StageUIFrame");
	pObj->SetCam(m_pMainCamera);
	pObj->SetStageState(STAGESTATE_READY);	//Ready셋팅
	m_pStateUI = pObj;	//StageUI 셋팅
	((CStageUIFrame*)m_pStateUI)->SetPlayTime(m_pPlayTime);	//플레이타임 연결
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_UI, pObj);
	pLayer->AddObj(L"StageUIFrame", pObj);

	//#ifdef _DEBUGz
	//Status
	pObj = CStatus::Create(m_pDevice);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetObjID(L"Status");
	pObj->SetCam(m_pMainCamera);
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_UI, pObj);
	pLayer->AddObj(L"Status", pObj);
	//#endif

	m_pLayer[CLayer::LAYERTYPE_UI] = pLayer;

	return S_OK;
}

HRESULT CStage2::Add_Light(void)
{
	HRESULT hr = NULL;

	LightInfo tLightInfo;

	//Dir Light
	ZeroMemory(&tLightInfo, sizeof(LightInfo));
	tLightInfo.eLightType = LIGHT_DIR;
	tLightInfo.diffuseColor = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.ambientColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.specular = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.direction = D3DXVECTOR4(1.f, -1.f, -1.f, 0.f);
	tLightInfo.pos = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	tLightInfo.range = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);

	hr = CLightMgr::GetInstance()->AddLight(m_pDevice, &tLightInfo, 0, m_pMainCamera);
	FAILED_CHECK(hr);

	//Point Light
	ZeroMemory(&tLightInfo, sizeof(LightInfo));
	tLightInfo.eLightType = LIGHT_POINT;
	tLightInfo.diffuseColor = D3DXVECTOR4(1.f, 0.f, 0.f, 0.f);
	tLightInfo.ambientColor = D3DXVECTOR4(0.f, 0.f, 0.f, 1.f);
	tLightInfo.specular = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.direction = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	tLightInfo.pos = D3DXVECTOR4(0.f, 5.f, 0.f, 1.f);
	tLightInfo.range = D3DXVECTOR4(7.f, 0.f, 0.f, 0.f);

	hr = CLightMgr::GetInstance()->AddLight(m_pDevice, &tLightInfo, 1, m_pMainCamera);
	FAILED_CHECK(hr);

	//Shadow Spot Light
	ZeroMemory(&tLightInfo, sizeof(LightInfo));
	tLightInfo.eLightType = LIGHT_SPOT;
	tLightInfo.diffuseColor = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.ambientColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.specular = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.direction = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	//tLightInfo.pos = D3DXVECTOR4(-130.f, 150.f, -90.f, 0.f);
	tLightInfo.pos = D3DXVECTOR4(80.f, 130.f, 120.f, 0.f);
	tLightInfo.range = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);

	hr = CLightMgr::GetInstance()->AddLight(m_pDevice, &tLightInfo, 2, m_pMainCamera);
	FAILED_CHECK(hr);

	return S_OK;
}

HRESULT CStage2::Add_RenderTarget(void)
{
	HRESULT hr = NULL;

	D3D11_VIEWPORT ViewPort;
	ZeroMemory(&ViewPort, sizeof(ViewPort));
	UINT numViewport = 1;
	m_pDevice->GetDeviceContext()->RSGetViewports(&numViewport, &ViewPort);

	//Create Degbug Window
	hr = CRenderTargetMgr::GetInstance()->Add_RenderTarget(m_pDevice
		, m_pMainCamera
		, L"Target_Deferred"
		, ViewPort.Width, ViewPort.Height
		, DXGI_FORMAT_R16G16B16A16_FLOAT		//나중에 다르면 바꿔주자
		, D3DXCOLOR(0.f, 0.f, 0.f, 0.f));
	FAILED_CHECK(hr);

	hr = CRenderTargetMgr::GetInstance()->Add_DebugBuffer(L"Target_Deferred", 100.f, 100.f);
	FAILED_CHECK(hr);

	CRenderTargetMgr::GetInstance()->Add_MRT(L"MRT_ALL", L"Target_Deferred");	FAILED_CHECK(hr);

	return S_OK;
}

HRESULT CStage2::Connect_Cam_To_Navi(void)
{
	CNaviMgr::GetInstance()->SetCam(m_pMainCamera);

	return S_OK;
}

void CStage2::Release(void)
{
	CCollisionMgr::GetInstance()->SetObjList(nullptr);
	CServerMgr::GetInstance()->SetObjList(nullptr);

	m_pPlayTime = nullptr;

	Safe_Delete(m_pLoading);
}