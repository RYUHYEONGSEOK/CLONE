#include "stdafx.h"
#include "MeshMap.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
//Object
#include "StaticMeshShader.h"
#include "StaticMesh.h"
#include "DepthShader.h"

CMeshMap::CMeshMap(CDevice * pDevice)
	: CObj(pDevice)
{
	for (int i = 0; i < MAP_OBJ_END; ++i)
	{
		m_pMapBuffer[i] = NULL;
		m_pMapTexture[i] = NULL;
	}
}

CMeshMap::~CMeshMap()
{
	Release();
}

HRESULT CMeshMap::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_NONEALPHA;

	//위치 조정
	m_pMapInfo[MAP_OBJ_BUILDING_LEFT]->m_vPos	= D3DXVECTOR3(-42.5f, 0.f, 15.f);
	m_pMapInfo[MAP_OBJ_BUILDING_RIGHT]->m_vPos	= D3DXVECTOR3(42.5f, 0.f, -15.f);
	m_pMapInfo[MAP_OBJ_BRIDGE_UP]->m_vPos		= D3DXVECTOR3(0.f, 0.f, -23.f);
	m_pMapInfo[MAP_OBJ_BRIDGE_DOWN]->m_vPos		= D3DXVECTOR3(0.f, 0.f, 23.f);

	m_pMapInfo[MAP_OBJ_FOUNTAIN]->m_vScale = D3DXVECTOR3(0.98f, 0.98f, 0.98f);

	return S_OK;
}

int CMeshMap::Update()
{
	CObj::Update_Component();

	return 0;
}

void CMeshMap::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	for (int i = 0; i < MAP_OBJ_END; ++i)
	{
		//월드 XM매트릭스 생성
		XMFLOAT4X4 matWorld;
		memcpy(&matWorld, m_pMapInfo[i]->m_matWorld, sizeof(float) * 16);

		//셰이더 파라미터 세팅
		((CStaticMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
			m_pLightInfo, m_pMapTexture[i]->GetResource());

		m_pMapBuffer[i]->Render(DXGI_32, DRAW_NORMAL);
	}
}

void CMeshMap::RenderForShadow(LightMatrixStruct tLight)
{
	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);
	
	for (int i = 0; i < MAP_OBJ_END; ++i)
	{
		//월드 XM매트릭스 생성
		XMFLOAT4X4 matWorld;
		memcpy(&matWorld, m_pMapInfo[i]->m_matWorld, sizeof(float) * 16);

		//셰이더 파라미터 세팅
		((CDepthShader*)m_pDepthShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pMapBuffer[i]);

		m_pMapBuffer[i]->Render(DXGI_32, DRAW_NORMAL);
	}
}

HRESULT CMeshMap::AddComponent(void)
{
	CComponent*		pComponent = NULL;

	//Transform
	for (int i = 0; i < MAP_OBJ_END; ++i)
	{
		pComponent = m_pMapInfo[i] = CTransform::Create(g_vLook);
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		wstring wName = L"Transform";
		wstring wCnt = to_wstring(i);
		wName += wCnt;
		m_mapComponent.insert(make_pair(wName, pComponent));
	}

	//Buffer
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_Ground");
	m_pMapBuffer[MAP_OBJ_GROUND] = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_Bridge");
	m_pMapBuffer[MAP_OBJ_BRIDGE_UP] = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_Bridge");
	m_pMapBuffer[MAP_OBJ_BRIDGE_DOWN] = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_Building");
	m_pMapBuffer[MAP_OBJ_BUILDING_LEFT] = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_Building");
	m_pMapBuffer[MAP_OBJ_BUILDING_RIGHT] = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_Fountain");
	m_pMapBuffer[MAP_OBJ_FOUNTAIN] = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Ground_d");
	m_pMapTexture[MAP_OBJ_GROUND] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Bridge_d");
	m_pMapTexture[MAP_OBJ_BRIDGE_UP] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Bridge_d");
	m_pMapTexture[MAP_OBJ_BRIDGE_DOWN] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Building_d");
	m_pMapTexture[MAP_OBJ_BUILDING_LEFT] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Building_d");
	m_pMapTexture[MAP_OBJ_BUILDING_RIGHT] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Fountain_d");
	m_pMapTexture[MAP_OBJ_FOUNTAIN] = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//LightInfo
	m_pLightInfo = CLightMgr::GetInstance()->GetLightInfo(0);

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_StaticMesh"));
	//Depth Shader
	m_pDepthShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Depth"));

	return S_OK;
}

CMeshMap * CMeshMap::Create(CDevice * pDevice, OBJTYPE eType)
{
	CMeshMap*	pMap = new CMeshMap(pDevice);

	if (FAILED(pMap->Init()))
		Safe_Delete(pMap);

	pMap->SetObjType(eType);

	return pMap;
}

void CMeshMap::Release()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pDepthShader);
}





