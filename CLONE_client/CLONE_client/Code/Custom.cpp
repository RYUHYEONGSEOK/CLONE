#include "stdafx.h"
#include "Custom.h"
//Manager
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "SceneMgr.h"
#include "InputMgr.h"
#include "SoundMgr.h"
//Object
#include "Renderer.h"
#include "Loading.h"
#include "CustomBack.h"
#include "CustomButton.h"
//server
#include "ServerMgr.h"

CCustom::CCustom(CDevice * pDevice)
	: CScene(pDevice)
	, m_pCustomBack(NULL)
{

}

CCustom::~CCustom(void)
{
	Release();
}

HRESULT CCustom::InitScene(void)
{
	FAILED_CHECK(Add_Environment_Layer());
	FAILED_CHECK(Add_GameLogic_Layer());
	FAILED_CHECK(Add_UI_Layer());
	FAILED_CHECK(Add_Light());

#ifdef _PLAY_SOUND
	// BGM
	CSoundMgr::GetInstance()->PlayBGM(L"lobby_bgm.mp3");
#endif

	return S_OK;
}

void CCustom::Update(void)
{
	if (CServerMgr::GetInstance()->GetIsChangeToInGameScene())
	{
		if(g_GameInfo.bMapType == false) CSceneMgr::GetInstance()->SetSceneAftherLoading(CSceneMgr::SCENE_ID_KJKPLAZA);
		else if(g_GameInfo.bMapType == true) CSceneMgr::GetInstance()->SetSceneAftherLoading(CSceneMgr::SCENE_ID_KJKPOOL);
		
		return;
	}

	CScene::Update_Layer();
}

void CCustom::Render(void)
{
	m_pDevice->Blend_Begin();
	CCustom::Render_Layer();
	m_pDevice->Blend_End();
}

CCustom * CCustom::Create(CDevice * pDevice)
{
	CCustom* pLogo = new CCustom(pDevice);

	if (FAILED(pLogo->InitScene()))
		Safe_Delete(pLogo);

	return pLogo;
}

HRESULT CCustom::Add_Environment_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	m_pLayer[CLayer::LAYERTYPE_ENVIRONMENT] = pLayer;

	return S_OK;
}

HRESULT CCustom::Add_GameLogic_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	m_pLayer[CLayer::LAYERTYPE_GAMELOGIC] = pLayer;

	return S_OK;
}

HRESULT CCustom::Add_UI_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	//CustomBack
	pObj = CCustomBack::Create(m_pDevice);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pCustomBack = pObj;
	pLayer->AddObj(L"CustomBack", pObj);

	//Custom Button
	pObj = CCustomButton::Create(m_pDevice);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	// 버튼 Obj 셋팅
	((CCustomBack*)m_pCustomBack)->SetButtonObj(pObj);
	pLayer->AddObj(L"CustomButton", pObj);

	m_pLayer[CLayer::LAYERTYPE_UI] = pLayer;

	return S_OK;
}

HRESULT CCustom::Add_Light(void)
{
	HRESULT hr = NULL;

	//LightInfo  LightInfo;
	//
	//// 빛의 종류
	//LightInfo.eLightType = LIGHT_DIR;
	//// 빛의 강도
	//LightInfo.ambientColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	//// 반사하는거 물체의 색상 (없어도됨...) // 반사하고 싶은 색깔을 정함...
	//LightInfo.diffuseColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.f);
	//// 빛의 방향
	//LightInfo.direction = XMFLOAT3(-1.f, -1.f, 0.f);
	//// 재질의 빛나는 강도
	//LightInfo.specular = XMFLOAT4(1.5f, 1.5f, 1.5f, 1.5f);
	//
	//hr = CLightMgr::GetInstance()->AddLight(m_pDevice, &LightInfo, 0);

	/*LightInfo  LightInfo;
	LightInfo.m_pos = XMFLOAT3(0.f, 100.f, 0.f);
	LightInfo.m_ambientColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	LightInfo.m_diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LightInfo.m_direction = XMFLOAT3(0.f, 0.0f, 1.0f);
	LightInfo.m_specular = XMFLOAT4(5.f, 5.f, 5.f, 5.f);
	hr = CLightMgr::GetInstance()->AddLight(&LightInfo, 0);*/

	return S_OK;
}

void CCustom::Release(void)
{
	m_pCustomBack = NULL;

	//Logo에서 사용한 Obj는 지워준다.
	for (auto i = 0; i < CLayer::LAYERTYPE_END; ++i)
		m_pLayer[i]->Release_Obj();
}