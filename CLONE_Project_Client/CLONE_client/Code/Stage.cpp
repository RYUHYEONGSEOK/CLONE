#include "stdafx.h"
#include "Stage.h"
//Manager
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "CollisionMgr.h"
#include "SceneMgr.h"
#include "TimeMgr.h"
#include "NaviMgr.h"
#include "Frustum.h"
#include "ServerMgr.h"
//Object
#include "Renderer.h"
#include "Loading.h"
#include "Terrain.h"
#include "Player.h"
#include "SkyBox.h"
#include "DynamicCam.h"
//#include "Button.h"
#include "Status.h"
#include "MeshMap.h"
#include "Bot.h"
#include "StageUIFrame.h"
#include "Enemy.h"
#include "Bead.h"
#include "StreamWater.h"
#include "FountainWater.h"

CStage::CStage(CDevice * pDevice)
	: CScene(pDevice)
	, m_pStateUI(NULL)
	, m_pPlayTime(nullptr)
{
}

CStage::~CStage(void)
{
	Release();
}

HRESULT CStage::InitScene(void)
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
	//Navi_Mesh 추가
	FAILED_CHECK(Add_NaviMesh());

	return S_OK;
}

void CStage::Update(void)
{
	//대기실로 이동
	if ((m_eStageState == STAGESTATE_VIC || m_eStageState == STAGESTATE_DEFT )
		&& CServerMgr::GetInstance()->GetIsChangeToRoomScene())
	{
		CServerMgr::GetInstance()->SetChangeScene(0, false);

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

	//Play 시 게임타임 체크
	//if (m_eStageState == STAGESTATE_PLAY)
	//{
	//	m_fPlayTime -= CTimeMgr::GetInstance()->GetTime();
	//	if (m_fPlayTime <= 0.f)
	//		m_fPlayTime = 0.f;
	//}

	//Frustum Update
	CFrustum::GetInstance()->UpdateFrustum(m_pMainCamera);

	CScene::Update_Layer();
}

void CStage::Render(void)
{
	//네비매쉬 랜더
	//나중에 끄던지 하자
//#ifdef _DEBUG
	CNaviMgr::GetInstance()->Render_NaviMesh();
//#endif
	//Render는 렌더러가 한다
	//CScene::Render_Layer();
}

CStage * CStage::Create(CDevice * pDevice)
{
	CStage* pLogo = new CStage(pDevice);

	if (FAILED(pLogo->InitScene()))
		Safe_Delete(pLogo);

	return pLogo;
}

HRESULT CStage::Add_Environment_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	//Cam
	pObj = CDynamicCamera::Create(m_pDevice,
		&D3DXVECTOR3(-HALFMAPX, 20.f, -HALFMAPZ), &D3DXVECTOR3(1.f, -1.f, -1.f));
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

HRESULT CStage::Add_GameLogic_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	//StaticMesh Map
	pObj = CMeshMap::Create(m_pDevice, OBJ_ETC);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetCam(m_pMainCamera);
	pObj->SetObjID(L"MeshMap");
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
	pLayer->AddObj(L"MeshMap", pObj);

	//Player --------------------------------------------------------------------
	int iClientID = CServerMgr::GetInstance()->GetClientID();
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
			pObj->SetObjID(L"Player");
			pObj->SetCam(m_pMainCamera);
			CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
			pLayer->AddObj(L"Player", pObj);
		}
		else if (vPos.y < 0.5f)
		{
			pObj = CPlayer::Create(m_pDevice, OBJ_PLAYER);
			NULL_CHECK_RETURN(pObj, E_FAIL);
			reinterpret_cast<CPlayer*>(pObj)->SetPlayerID(i);
			reinterpret_cast<CPlayer*>(pObj)->SetPlayerIsMe(false);
			pObj->SetPos(vPos);
			pObj->SetObjID(L"Enemy");
			pObj->SetCam(m_pMainCamera);
			CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
			pLayer->AddObj(L"Enemy", pObj);
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

	//Bead ----------------------------------------------------------------------
	for (int i = 0; i < 5; ++i)
	{
		pObj = CBead::Create(m_pDevice, OBJ_BEAD);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		pObj->SetObjID(L"Bead");
		pObj->SetPos((CServerMgr::GetInstance()->GetBeadPos())[i]);
		pObj->SetCam(m_pMainCamera);
		((CBead*)pObj)->SetBeadType(i);
		CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
		pLayer->AddObj(L"Bead", pObj);
	}
	//---------------------------------------------------------------------------



	//StreamWater
	pObj = CStreamWater::Create(m_pDevice, OBJ_ETC);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetObjID(L"StreamWater");
	pObj->SetCam(m_pMainCamera);
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
	pLayer->AddObj(L"StreamWater", pObj);

	//FountainWater
	pObj = CFountainWater::Create(m_pDevice, OBJ_ETC);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	pObj->SetObjID(L"FountainWater");
	pObj->SetCam(m_pMainCamera);
	CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_NONEALPHA, pObj);
	pLayer->AddObj(L"FountainWater", pObj);
	
	m_pLayer[CLayer::LAYERTYPE_GAMELOGIC] = pLayer;


	//LAYERTYPE_GAMELOGIC layer Setting
	CCollisionMgr::GetInstance()->SetObjList(m_pLayer[CLayer::LAYERTYPE_GAMELOGIC]->GetMapObjList());
	CServerMgr::GetInstance()->SetObjList(m_pLayer[CLayer::LAYERTYPE_GAMELOGIC]->GetMapObjList());


	//clear send data + change scene
	CServerMgr::GetInstance()->SetChangeScene(1, false);
	CServerMgr::GetInstance()->ClearSendInitData();

	return S_OK;
}

HRESULT CStage::Add_UI_Layer(void)
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

HRESULT CStage::Add_Light(void)
{
	HRESULT hr = NULL;

	LightInfo tLightInfo;

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

	//Point Light는 좀 더 생각해보자...
	/*ZeroMemory(&tLightInfo, sizeof(LightInfo));
	tLightInfo.eLightType = LIGHT_POINT;
	tLightInfo.diffuseColor = D3DXVECTOR4(1.f, 0.f, 0.f, 0.f);
	tLightInfo.ambientColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.specular = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.direction = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	tLightInfo.pos = D3DXVECTOR4(0.f, 10.f, 0.f, 1.f);
	tLightInfo.range = D3DXVECTOR4(3.f, 0.f, 0.f, 0.f);

	hr = CLightMgr::GetInstance()->AddLight(m_pDevice, &tLightInfo, 1, m_pMainCamera);
	FAILED_CHECK(hr);*/

	ZeroMemory(&tLightInfo, sizeof(LightInfo));
	tLightInfo.eLightType = LIGHT_SPOT;
	tLightInfo.diffuseColor = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.ambientColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.specular = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.direction = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	//tLightInfo.pos = D3DXVECTOR4(-130.f, 150.f, -90.f, 0.f);
	tLightInfo.pos = D3DXVECTOR4(80.f, 130.f, 120.f, 0.f);
	tLightInfo.range = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);

	hr = CLightMgr::GetInstance()->AddLight(m_pDevice, &tLightInfo, 1, m_pMainCamera);
	FAILED_CHECK(hr);

	return S_OK;
}

HRESULT CStage::Add_RenderTarget(void)
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

HRESULT CStage::Add_NaviMesh(void)
{
	CNaviMgr::GetInstance()->SetCam(m_pMainCamera);

	/*HRESULT		hr = NULL;
	D3DXVECTOR3		vPoint[3];

	vPoint[0] = D3DXVECTOR3(0.f, 0.f, 2.f);
	vPoint[1] = D3DXVECTOR3(2.f, 0.f, 0.f);
	vPoint[2] = D3DXVECTOR3(0.f, 0.f, 0.f);
	hr = CNaviMgr::GetInstance()->AddCell(&vPoint[0], &vPoint[1], &vPoint[2]);
	FAILED_CHECK(hr);

	vPoint[0] = D3DXVECTOR3(0.f, 0.f, 2.f);
	vPoint[1] = D3DXVECTOR3(2.f, 0.f, 2.f);
	vPoint[2] = D3DXVECTOR3(2.f, 0.f, 0.f);
	hr = CNaviMgr::GetInstance()->AddCell(&vPoint[0], &vPoint[1], &vPoint[2]);
	FAILED_CHECK(hr);

	vPoint[0] = D3DXVECTOR3(0.f, 0.f, 4.f);
	vPoint[1] = D3DXVECTOR3(2.f, 0.f, 2.f);
	vPoint[2] = D3DXVECTOR3(0.f, 0.f, 2.f);
	hr = CNaviMgr::GetInstance()->AddCell(&vPoint[0], &vPoint[1], &vPoint[2]);
	FAILED_CHECK(hr);

	vPoint[0] = D3DXVECTOR3(2.f, 0.f, 2.f);
	vPoint[1] = D3DXVECTOR3(4.f, 0.f, 0.f);
	vPoint[2] = D3DXVECTOR3(2.f, 0.f, 0.f);
	hr = CNaviMgr::GetInstance()->AddCell(&vPoint[0], &vPoint[1], &vPoint[2]);
	FAILED_CHECK(hr);

	CNaviMgr::GetInstance()->LinkCell();
*/
	return S_OK;
}

void CStage::Release(void)
{
	CCollisionMgr::GetInstance()->SetObjList(nullptr);
	CServerMgr::GetInstance()->SetObjList(nullptr);

	m_pPlayTime = nullptr;

	Safe_Delete(m_pLoading);
}