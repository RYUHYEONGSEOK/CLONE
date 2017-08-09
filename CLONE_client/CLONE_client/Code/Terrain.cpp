#include "stdafx.h"
#include "Terrain.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "CollisionMgr.h"
//Object
#include "TerrainCol.h"
#include "TerrainShader.h"
#include "DepthShader.h"

CTerrain::CTerrain(CDevice * pDevice)
	: CObj(pDevice)
{

}

CTerrain::~CTerrain()
{
	Release();
}

HRESULT CTerrain::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_NONEALPHA;

	return S_OK;
}

int CTerrain::Update()
{
	CObj::Update_Component();

	return 0;
}

void CTerrain::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CTerrainShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pLightInfo, m_pTexture->GetResource());

	m_pBuffer->Render(DXGI_32);
}

void CTerrain::RenderForShadow(LightMatrixStruct tLight)
{
	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CDepthShader*)m_pDepthShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pBuffer);

	m_pBuffer->Render(DXGI_32);
}

void CTerrain::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}

HRESULT CTerrain::AddComponent(void)
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
		L"Buffer_Terrain");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	//CollisionMgr에 TerrainVtx 셋팅
	CCollisionMgr::GetInstance()->SetTerrainVtx(((CTerrainCol*)m_pBuffer)->GetTerrainVtx());
	m_mapComponent.insert(make_pair(L"Buffer", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Terrain");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//LightInfo
	m_pLightInfo = CLightMgr::GetInstance()->GetLightInfo(0);

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Terrain"));
	//Depth Shader
	m_pDepthShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Depth"));

	return S_OK;
}

CTerrain * CTerrain::Create(CDevice * pDevice, OBJTYPE eType)
{
	CTerrain*	pTerrain = new CTerrain(pDevice);

	if (FAILED(pTerrain->Init()))
		Safe_Delete(pTerrain);

	pTerrain->SetObjType(eType);

	return pTerrain;
}

void CTerrain::Release()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pDepthShader);
}



