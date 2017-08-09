#include "stdafx.h"
#include "MeshMap2.h"
//Manager
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "SceneMgr.h"
#include "LightMgr.h"
//Object
#include "Layer.h"
#include "Scene.h"
#include "StaticMeshShader.h"
#include "ReflectionShader.h"
#include "RefactionShader.h"
#include "StaticMesh.h"
#include "DepthShader.h"

CMeshMap2::CMeshMap2(CDevice * pDevice)
	: CObj(pDevice)
{
	
}

CMeshMap2::~CMeshMap2()
{
	Release();
}

HRESULT CMeshMap2::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_NONEALPHA;
	m_eObjRenderRefType = OBJ_RENDER_REF_UNDER;

	m_pInfo->m_vScale = D3DXVECTOR3(1.f, 0.2f, 1.f);

	return S_OK;
}

int CMeshMap2::Update()
{
	CObj::Update_Component();

	return 0;
}

void CMeshMap2::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CStaticMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pLightInfo, m_pTexture->GetResource());

	m_pBuffer->Render(DXGI_32, DRAW_NORMAL);
}

void CMeshMap2::RenderForShadow(LightMatrixStruct tLight)
{
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CDepthShader*)m_pDepthShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pBuffer);

	m_pBuffer->Render(DXGI_32);
}

void CMeshMap2::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
	XMFLOAT4X4 matWorld;

	const CComponent*	pTrapWaterInfo = CSceneMgr::GetInstance()->GetScene()->GetComponent(CLayer::LAYERTYPE_GAMELOGIC, L"TrapWater", L"Transform");

	D3DXVECTOR4 vClipPlaneForTrapWater = D3DXVECTOR4(0.f, -1.f, 0.f, ((CTransform*)pTrapWaterInfo)->m_vPos.y + 0.1f);

	if (eRefType == CObj::REFTYPE_REFRACT)
	{
		//아래서 그려지므로 베이스 맵만 그리면 된다
		//월드 XM매트릭스 생성
		memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

		//셰이더 파라미터 세팅
		((CRefactionShader*)m_pRefractionShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pBuffer,
			CLightMgr::GetInstance()->GetLightInfo(), m_pTexture->GetResource(), vClipPlaneForTrapWater);

		m_pBuffer->Render(DXGI_32, DRAW_NORMAL);
	}
	else if (eRefType == CObj::REFTYPE_REFLECT)
	{
		
	}
}

HRESULT CMeshMap2::AddComponent(void)
{
	CComponent*		pComponent = NULL;

	//Transform
	pComponent = m_pInfo = CTransform::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Transform", pComponent));

	//Buffer
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_KJKPool");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Buffer", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_KJKPool");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
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
	//RefactionShader && ReflectionShader
	m_pRefractionShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Refaction"));
	m_pReflectionShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Reflection"));

	return S_OK;
}

CMeshMap2 * CMeshMap2::Create(CDevice * pDevice, OBJTYPE eType)
{
	CMeshMap2*	pMap = new CMeshMap2(pDevice);

	if (FAILED(pMap->Init()))
		Safe_Delete(pMap);

	pMap->SetObjType(eType);

	return pMap;
}

void CMeshMap2::Release()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pDepthShader);
	Safe_Release(m_pReflectionShader);
	Safe_Release(m_pRefractionShader);
}





