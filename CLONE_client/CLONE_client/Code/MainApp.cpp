#include "stdafx.h"
#include "MainApp.h"
//Manager
#include "GraphicDevice.h"
#include "TimeMgr.h"
#include "InputMgr.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "ObjMgr.h"
#include "LightMgr.h"
#include "NaviMgr.h"
#include "SoundMgr.h"
#include "Frustum.h"
//Object
#include "Scene.h"
#include "Loading.h"

CMainApp::CMainApp(void)
	: m_pDevice(NULL)
	, m_pTimeMgr(NULL)
	, m_pInput(NULL)
	, m_pSceneMgr(NULL)
	, m_pRenderTargetMgr(NULL)
	, m_pLightMgr(NULL)
	, m_pResourceMgr(NULL)
	, m_pObjMgr(NULL)
	, m_pNaviMgr(NULL)
	, m_pFrustum(NULL)
	, m_pSoundMgr(NULL)
{
	////콘솔창
	//AllocConsole();
	//freopen("CONOUT$", "wt", stdout);
	//SetConsoleTitleA("Debug");
}

CMainApp::~CMainApp(void)
{
	//FreeConsole();

	Release();
}

HRESULT CMainApp::InitApp(void)
{
	//rand 초기화
	srand(unsigned int(time(NULL)));

	//Manager 할당
	m_pDevice			= CDevice::GetInstance();
	m_pTimeMgr			= CTimeMgr::GetInstance();
	m_pInput			= CInput::GetInstance();
	m_pSceneMgr			= CSceneMgr::GetInstance();
	m_pRenderTargetMgr	= CRenderTargetMgr::GetInstance();
	m_pLightMgr			= CLightMgr::GetInstance();
	m_pResourceMgr		= CResourceMgr::GetInstance();
	m_pObjMgr			= CObjMgr::GetInstance();
	m_pNaviMgr			= CNaviMgr::GetInstance();
	m_pFrustum			= CFrustum::GetInstance();
	m_pSoundMgr			= CSoundMgr::GetInstance();

	//Manager 초기화
	m_pDevice->Init();
	m_pTimeMgr->InitTime();
	m_pInput->Init(g_hInst, g_hWnd);
	m_pSceneMgr->Init(m_pDevice);
	m_pSceneMgr->SetScene(CSceneMgr::SCENE_ID_LOGO);
	m_pSoundMgr->Initialize();
	m_pSoundMgr->LoadSoundFile();

	//글로벌 Draw 변수
	g_bDraw = TRUE;

	return S_OK;
}

void CMainApp::Update(void)
{
	m_pTimeMgr->SetTime();
	m_pInput->UpdateInputState();
	m_pSceneMgr->Update();
}

void CMainApp::Render(void)
{
	m_pSceneMgr->Render(m_pTimeMgr->GetTime());
}

void CMainApp::Release(void)
{
	//Client Delete
	unordered_map<wstring, list<CLIENT*>>::iterator	iterMap = g_mapClient.begin();
	unordered_map<wstring, list<CLIENT*>>::iterator iterMap_end = g_mapClient.end();

	list<CLIENT*>::iterator	iterList;
	list<CLIENT*>::iterator	iterList_end;


	for (; iterMap != iterMap_end; ++iterMap)
	{
		iterList = iterMap->second.begin();
		iterList_end = iterMap->second.end();

		for (; iterList != iterList_end; ++iterList)
			::Safe_Delete(*iterList);

		iterMap->second.clear();
	}
	g_mapClient.clear();

	//Destroy Manager
	Safe_Single_Destory(m_pFrustum);
	Safe_Single_Destory(m_pDevice);
	Safe_Single_Destory(m_pTimeMgr);
	Safe_Single_Destory(m_pInput);
	Safe_Single_Destory(m_pSceneMgr);
	Safe_Single_Destory(m_pRenderTargetMgr);
	Safe_Single_Destory(m_pLightMgr);
	Safe_Single_Destory(m_pResourceMgr);
	Safe_Single_Destory(m_pObjMgr);
	Safe_Single_Destory(m_pSoundMgr);
	Safe_Single_Destory(m_pNaviMgr);
}

