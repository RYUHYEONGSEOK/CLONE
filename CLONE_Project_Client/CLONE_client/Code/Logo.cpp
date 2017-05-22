#include "stdafx.h"
#include "Logo.h"
//Manager
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "SceneMgr.h"
#include "InputMgr.h"
//Object
#include "Renderer.h"
#include "LogoBack.h"
#include "Loading.h"
#include "LogoTitle.h"
//server
#include "ServerMgr.h"

CLogo::CLogo(CDevice * pDevice)
	: CScene(pDevice)
	, m_pTitle(NULL)
	, m_pText(NULL)
	, m_pLogoBack(NULL)
	, m_bIsChange(false)
{

}

CLogo::~CLogo(void)
{
	Release();
}

HRESULT CLogo::InitScene(void)
{
	m_pLoading = CLoading::Create(CLoading::LOADID_STAGE);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	FAILED_CHECK(Loading_LogoResource());
	FAILED_CHECK(Add_Environment_Layer());
	FAILED_CHECK(Add_GameLogic_Layer());
	FAILED_CHECK(Add_UI_Layer());
	FAILED_CHECK(Add_Light());

	return S_OK;
}

void CLogo::Update(void)
{
	if (m_pLoading->GetComplete() == true)
	{
		//�ε��� ������ Ÿ��Ʋ�� �����ش�.
		((CLogoTitle*)m_pTitle)->SetComplete(true);
		
		//Text �ٲ��ִ� Flag
		if (((CLogoTitle*)m_pTitle)->GetTitleMoveFlag())
			((CLogoBack*)m_pLogoBack)->SetTextChange(TRUE);
	}

	//Title�� ��ư���� Flag�� ������ ���� �ѱ��.
	if (((CLogoTitle*)m_pTitle)->GetSceneChangeFlag()
		&& (CServerMgr::GetInstance()->GetIsChangeToRoomScene()))
	{
		m_bIsChange = false;
		CServerMgr::GetInstance()->SetChangeScene(0, false);

		// �� ��ȯ
		CSceneMgr::GetInstance()->SetSceneAftherLoading(CSceneMgr::SCENE_ID_CUSTOM);
		return;
	}
	if (!m_bIsChange)
	{
		m_bIsChange = true;
		if (!CServerMgr::GetInstance()->Initialize("127.0.0.1", g_hWnd))
		{
			cout << "ServerMgr Initialize() error" << endl;
			exit(1);
		}
	}

	CScene::Update_Layer();
}

void CLogo::Render(void)
{
	m_pDevice->Blend_Begin();
	CLogo::Render_Layer();
	m_pDevice->Blend_End();
}

CLogo * CLogo::Create(CDevice * pDevice)
{
	CLogo* pLogo = new CLogo(pDevice);

	if (FAILED(pLogo->InitScene()))
		Safe_Delete(pLogo);

	return pLogo;
}

HRESULT CLogo::Loading_LogoResource(void)
{
	//Font �ε� ------------------------------------------------------
	AddFontResourceEx(L"../bin/Resources/Font/koverwatch.ttf", FR_PRIVATE, NULL);
	CResourceMgr::GetInstance()->Add_Font(m_pDevice, CResourceMgr::RESOURCE_ATTRI_STATIC,
		L"Font_koverwatch", L"../bin/Resources/Font/koverwatch.ttf");
	//Font �ε� �� ---------------------------------------------------

	//Logo_Back Resource �ε� ----------------------------------------
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_LogoBack", L"../bin/Resources/Texture/Logo/loading_Back_%d.png", 8);
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_LoadingProgress", L"../bin/Resources/Texture/Logo/loading_progress.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_LoginCheck", L"../bin/Resources/Texture/Logo/login_check_%d.png", 4);
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_TitleBottom", L"../bin/Resources/Texture/Logo/title_bottom.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_TitleTop", L"../bin/Resources/Texture/Logo/title_top.png");

	CResourceMgr::GetInstance()->Add_Buffer(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::BUFFER_TYPE_LOGO, L"Buffer_Logo");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_LOGOBACK, L"Shader_LogoBack");
	//Logo_Back �ε� �� ----------------------------------------------

	//Text�� UI Resource �ε� ----------------------------------------
	CResourceMgr::GetInstance()->Add_Buffer(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::BUFFER_TYPE_UI, L"Buffer_UI");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_TEXTURE, L"Shader_Texture");
	//Text�� UI Resource �ε� �� -------------------------------------

	return S_OK;
}

HRESULT CLogo::Add_Environment_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	

	m_pLayer[CLayer::LAYERTYPE_ENVIRONMENT] = pLayer;

	return S_OK;
}

HRESULT CLogo::Add_GameLogic_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	m_pLayer[CLayer::LAYERTYPE_GAMELOGIC] = pLayer;

	return S_OK;
}

HRESULT CLogo::Add_UI_Layer(void)
{
	CLayer*		pLayer = CLayer::Create(m_pDevice);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CObj* pObj = NULL;

	//Logo_Back
	pObj = CLogoBack::Create(m_pDevice);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLogoBack = pObj;	//LogoBack ����
	((CLogoBack*)pObj)->SetLoadingObj(m_pLoading);
	pLayer->AddObj(L"LogoBack", pObj);

	//Logo_Title
	pObj = CLogoTitle::Create(m_pDevice);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pTitle = pObj;	//Title ����
	((CLogoTitle*)pObj)->SetLoading(m_pLoading);
	pLayer->AddObj(L"LogoTitle", pObj);

	m_pLayer[CLayer::LAYERTYPE_UI] = pLayer;

	return S_OK;
}

HRESULT CLogo::Add_Light(void)
{
	HRESULT hr = NULL;

	//LightInfo  LightInfo;
	//
	//// ���� ����
	//LightInfo.eLightType = LIGHT_DIR;
	//// ���� ����
	//LightInfo.ambientColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	//// �ݻ��ϴ°� ��ü�� ���� (�����...) // �ݻ��ϰ� ���� ������ ����...
	//LightInfo.diffuseColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.f);
	//// ���� ����
	//LightInfo.direction = XMFLOAT3(-1.f, -1.f, 0.f);
	//// ������ ������ ����
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

void CLogo::Release(void)
{
	//Logo���� ����� Obj�� �����ش�.
	for(auto i = 0; i < CLayer::LAYERTYPE_END; ++i)
		m_pLayer[i]->Release_Obj();

	Safe_Delete(m_pLoading);
}