#include "stdafx.h"
#include "Loading.h"
//Manager
#include "GraphicDevice.h"
#include "NaviMgr.h"
#include "ResourceMgr.h"
#include "GraphicDevice.h"

CLoading::CLoading(LOADID eLoadID)
	: m_pDevice(NULL)
	, m_eLoadID(eLoadID)
	, m_hThread(NULL)
	, m_bComplete(false)
	, m_bLoadingPercent(0.f)
	, m_bCurrentLoadingCnt(0.f)
{
	ZeroMemory(&m_CSKey, sizeof(CRITICAL_SECTION));
	ZeroMemory(m_szLoadingMessage, sizeof(TCHAR) * 128);
}

CLoading::~CLoading(void)
{
	Release();
}

const TCHAR* CLoading::GetLoadingMessage(void)
{
	return m_szLoadingMessage;
}

bool CLoading::GetComplete(void)
{
	return m_bComplete;
}

float CLoading::GetLoadingPercent(void)
{
	switch (this->m_eLoadID)
	{
	case CLoading::LOADID_STAGE:
		m_bLoadingPercent = (m_bCurrentLoadingCnt / (float)RESOURCE_CNT_STAGE);
		break;
	}
	
	return m_bLoadingPercent;
}

HRESULT CLoading::InitLoading(void)
{
	InitializeCriticalSection(&m_CSKey);

	m_pDevice = CDevice::GetInstance();
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadingFunction, this, 0, NULL);
	NULL_CHECK_RETURN(m_hThread, E_FAIL);

	return S_OK;
}

void CLoading::NeviLoading(void)
{
	HANDLE hFile;
	hFile = CreateFile(L"../bin/Navi/Navi.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD	dwByte;
	int iSize = 0;
	int iCount = 0;

	ReadFile(hFile, &iCount, sizeof(int), &dwByte, NULL);

	CNaviMgr::GetInstance()->Reserve_CellContainerSize(m_pDevice, iCount);

	D3DXMATRIX	matRotX;
	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(-90.f));

	while (iSize < iCount)
	{
		D3DXVECTOR3 vPoint1, vPoint2, vPoint3;

		ReadFile(hFile, &vPoint1, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint1, &vPoint1, &matRotX);
		ReadFile(hFile, &vPoint2, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint2, &vPoint2, &matRotX);
		ReadFile(hFile, &vPoint3, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint3, &vPoint3, &matRotX);

		CNaviMgr::GetInstance()->AddCell(&vPoint1, &vPoint2, &vPoint3);
		++iSize;
	}

	CloseHandle(hFile);

	CNaviMgr::GetInstance()->LinkCell();
}

void CLoading::StageLoading1(void)
{
	HRESULT hr = NULL;

	//Font Num == 1
	lstrcpy(m_szLoadingMessage, L"Font Loading...");
	AddFontResourceEx(L"../bin/Resources/Font/koverwatch.ttf", FR_PRIVATE, NULL);
	CResourceMgr::GetInstance()->Add_Font(m_pDevice, CResourceMgr::RESOURCE_ATTRI_STATIC,
		L"Font_koverwatch1", L"../bin/Resources/Font/koverwatch.ttf");	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Font Loading... Font_koverwatch");

	//Texture Num == 11
	lstrcpy(m_szLoadingMessage, L"Texture Loading...");
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Mesh/Test.PNG");	//Test Tex
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Test", L"../bin/Resources/Mesh/Test.PNG");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/Terrain/Terrain0.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Terrain", L"../bin/Resources/Texture/Terrain/Terrain0.png");	
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/UI/보영.jpg");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_UI", L"../bin/Resources/Texture/UI/보영.jpg");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/SkyBox/SkyBox_00.dds");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_DDS, L"Texture_SkyBox", L"../bin/Resources/Texture/SkyBox/SkyBox_00.dds");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Mesh/StaticCharacter/man_diff.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Player", L"../bin/Resources/Mesh/StaticCharacter/man_diff.png");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Mesh/StageMap/bridge_d.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Bridge_d", L"../bin/Resources/Mesh/StageMap/bridge_d.png");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Mesh/StageMap/fountain_d.jpg");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Fountain_d", L"../bin/Resources/Mesh/StageMap/fountain_d.jpg");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Mesh/StageMap/building_d.jpg");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Building_d", L"../bin/Resources/Mesh/StageMap/building_d.jpg");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Mesh/StageMap/ground_d.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Ground_d", L"../bin/Resources/Mesh/StageMap/ground_d.png");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/UI/StageUI/chat_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_UIChatBoard", L"../bin/Resources/Texture/UI/StageUI/chat_%d.png", 2);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/UI/StageUI/check_vic_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_UICheckVic", L"../bin/Resources/Texture/UI/StageUI/check_vic_%d.png", 2);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/UI/StageUI/frame_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_UIFrame", L"../bin/Resources/Texture/UI/StageUI/frame_%d.png", 2);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/UI/StageUI/name_tag_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_UINameTag", L"../bin/Resources/Texture/UI/StageUI/name_tag_0.png");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/UI/StageUI/notice_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_UINotice", L"../bin/Resources/Texture/UI/StageUI/notice_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/UI/StageUI/notice_count_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_UINoticeCount", L"../bin/Resources/Texture/UI/StageUI/notice_count_%d.png", 4);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/Water/water.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_Water", L"../bin/Resources/Texture/Water/water.png");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/Stage/Icon/Clone_Icon.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CloneIcon", L"../bin/Resources/Texture/Stage/Icon/Clone_Icon.png");
	m_bCurrentLoadingCnt++;

	//CustomTexture NUM == 11
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/custom_background.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomChangeButton", L"../bin/Resources/Texture/CustomGame/change_button_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/custom_background_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomBack", L"../bin/Resources/Texture/CustomGame/custom_background_%d.png", 2);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/exit_icon_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomExitButton", L"../bin/Resources/Texture/CustomGame/exit_icon_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/host_icon.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomHostIcon", L"../bin/Resources/Texture/CustomGame/host_icon.png");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/info_icon_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomInfoIcon", L"../bin/Resources/Texture/CustomGame/info_icon_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/option_icon_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomOptionIcon", L"../bin/Resources/Texture/CustomGame/option_icon_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/ready_button_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomReadyButton", L"../bin/Resources/Texture/CustomGame/ready_button_%d.png", 4);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/slide_L_button_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomSlideLButton", L"../bin/Resources/Texture/CustomGame/slide_L_button_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/slide_R_button_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomSlideRButton", L"../bin/Resources/Texture/CustomGame/slide_R_button_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/start_button_.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomStartButton", L"../bin/Resources/Texture/CustomGame/start_button_%d.png", 3);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Texture Loading...   ../bin/Resources/Texture/CustomGame/KJKPlaza.png");
	CResourceMgr::GetInstance()->Add_Texture(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CTexture::TEXTURE_TYPE_BASIC, L"Texture_CustomKJKPlaza", L"../bin/Resources/Texture/CustomGame/KJKPlaza.png");
	m_bCurrentLoadingCnt++;
	
	//Buffer Num == 5
	lstrcpy(m_szLoadingMessage, L"Buffer Loading...");
	lstrcpy(m_szLoadingMessage, L"Buffer Loading...   Buffer_Terrain");
	CResourceMgr::GetInstance()->Add_TerrainBuffer(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::BUFFER_TYPE_TERRAIN, L"Buffer_Terrain", VTXCNTX, VTXCNTZ, VTXITV);
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Buffer Loading...   Buffer_SkyBox");
	CResourceMgr::GetInstance()->Add_Buffer(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::BUFFER_TYPE_SKYBOX, L"Buffer_SkyBox");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Buffer Loading...   Buffer_BillBoard");
	CResourceMgr::GetInstance()->Add_Buffer(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::BUFFER_TYPE_PLANE, L"Buffer_BillBoard");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Buffer Loading...   Buffer_Water");
	CResourceMgr::GetInstance()->Add_Buffer(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::BUFFER_TYPE_WATER, L"Buffer_Water");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Buffer Loading...   Buffer_FountainWater");
	CResourceMgr::GetInstance()->Add_Buffer(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::BUFFER_TYPE_FOUNTAINWATER, L"Buffer_FountainWater");
	m_bCurrentLoadingCnt++;

	//Static Mesh Num == 5
	lstrcpy(m_szLoadingMessage, L"Mesh Loading...");
	lstrcpy(m_szLoadingMessage, L"Mesh Loading...   ../bin/Resources/Mesh/sphere.FBX");	//Test Mesh
	CResourceMgr::GetInstance()->Add_Mesh(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::MESH_TYPE_STATIC, L"Mesh_Test", "../bin/Resources/Mesh/sphere.FBX");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Mesh Loading...   ../bin/Resources/Mesh/StageMap/ground.FBX");
	CResourceMgr::GetInstance()->Add_Mesh(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::MESH_TYPE_STATIC, L"Mesh_Ground", "../bin/Resources/Mesh/StageMap/ground.FBX");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Mesh Loading...   ../bin/Resources/Mesh/StageMap/bridge.FBX");
	CResourceMgr::GetInstance()->Add_Mesh(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::MESH_TYPE_STATIC, L"Mesh_Bridge", "../bin/Resources/Mesh/StageMap/bridge.FBX");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Mesh Loading...   ../bin/Resources/Mesh/StageMap/building1.FBX");
	CResourceMgr::GetInstance()->Add_Mesh(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::MESH_TYPE_STATIC, L"Mesh_Building", "../bin/Resources/Mesh/StageMap/building1.FBX");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Mesh Loading...   ../bin/Resources/Mesh/StageMap/fountain.FBX");
	CResourceMgr::GetInstance()->Add_Mesh(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::MESH_TYPE_STATIC, L"Mesh_Fountain", "../bin/Resources/Mesh/StageMap/fountain.FBX");
	m_bCurrentLoadingCnt++;


	//Dynamic Mesh Num == 1
	lstrcpy(m_szLoadingMessage, L"Mesh Loading...   ../bin/Resources/Mesh/DynamicCharacter/");
	//애니메이션 로딩예제 -------------------------------------------------
	vector<string> vecAniName;
	TCHAR		szAniName[MINPATH] = L"Player_AniMesh";
	TCHAR		szAniNum[MINPATH] = L"";
	//int			iAniNum;	//얘는 나중에 쓰자

	//여기서 char로 바꿔준다
	char		cAniName[MINPATH] = "walk";
	//WideCharToMultiByte(CP_ACP, 0, szAniName, MINPATH, cAniName, MINPATH, NULL, NULL);

	//vector에 넣는다
	vecAniName.push_back(cAniName);

	//리셋 후 다시 넣는다
	ZeroMemory(&cAniName, sizeof(char) * strlen(cAniName));
	strcpy(cAniName, "run");
	vecAniName.push_back(cAniName);

	ZeroMemory(&cAniName, sizeof(char) * strlen(cAniName));
	strcpy(cAniName, "idle");
	vecAniName.push_back(cAniName);

	ZeroMemory(&cAniName, sizeof(char) * strlen(cAniName));
	strcpy(cAniName, "attack");
	vecAniName.push_back(cAniName);

	ZeroMemory(&cAniName, sizeof(char) * strlen(cAniName));
	strcpy(cAniName, "die");
	vecAniName.push_back(cAniName);

	ZeroMemory(&cAniName, sizeof(char) * strlen(cAniName));
	strcpy(cAniName, "jump");
	vecAniName.push_back(cAniName);

	ZeroMemory(&cAniName, sizeof(char) * strlen(cAniName));
	strcpy(cAniName, "sit");
	vecAniName.push_back(cAniName);

	//char를 넣는다
	CResourceMgr::GetInstance()->Add_Mesh(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::MESH_TYPE_DYNAMIC, szAniName, "../bin/Resources/Mesh/DynamicCharacter/", vecAniName);
	m_bCurrentLoadingCnt++;
	//애니메이션 로딩끝 ---------------------------------------------------

	//Shader Num == 13
	lstrcpy(m_szLoadingMessage, L"Shader Loading...");
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_Terrain");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_TERRAIN, L"Shader_Terrain");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_SkyBox");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_SKYBOX, L"Shader_SkyBox");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_StaticMesh");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_STATICMESH, L"Shader_StaticMesh");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_DynamicMesh");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_DYNAMICMESH, L"Shader_DynamicMesh");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_Default");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_DEFAULT, L"Shader_Default");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_Blend");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_BLEND, L"Shader_Blend");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_Deferred");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_DEFERRED, L"Shader_Deferred");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_Line");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_LINE, L"Shader_Line");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_DirLight");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_DIRLIGHT, L"Shader_DirLight");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_PointLight");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_POINTLIGHT, L"Shader_PointLight");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_Depth");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_DEPTH, L"Shader_Depth");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_DepthDynamic");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_DEPTHDYNAMIC, L"Shader_DepthDynamic");
	m_bCurrentLoadingCnt++;
	lstrcpy(m_szLoadingMessage, L"Shader Loading...   Shader_Water");
	CResourceMgr::GetInstance()->Add_Shader(m_pDevice, CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::SHADER_TYPE_WATER, L"Shader_Water");
	m_bCurrentLoadingCnt++;

	//NaviCell Loading == 1
	lstrcpy(m_szLoadingMessage, L"NaviCell Loading...   ../bin/Navi/Navi.dat");
	NeviLoading();
	m_bCurrentLoadingCnt++;
	
	lstrcpy(m_szLoadingMessage, L"로딩완료...");
	m_bComplete = true;
}

void CLoading::StageLoading2(void) {}
void CLoading::StageLoading3(void) {}

CLoading* CLoading::Create(LOADID eLoadID)
{
	CLoading* pLoading = new CLoading(eLoadID);
	
	if (FAILED(pLoading->InitLoading()))
		Safe_Delete(pLoading);

	return pLoading;
}

void CLoading::Release(void)
{
	DeleteCriticalSection(&m_CSKey);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
}

UINT WINAPI CLoading::LoadingFunction(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	EnterCriticalSection(&pLoading->m_CSKey);

	switch (pLoading->m_eLoadID)
	{
	case CLoading::LOADID_STAGE:
		pLoading->StageLoading1();
		break;

	case CLoading::LOADID_STAGE1:
		pLoading->StageLoading2();
		break;

	case CLoading::LOADID_STAGE2:
		pLoading->StageLoading3();
		break;
	}

	LeaveCriticalSection(&pLoading->m_CSKey);

	return 0;
}