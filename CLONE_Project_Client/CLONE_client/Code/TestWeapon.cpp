#include "stdafx.h"
#include "TestWeapon.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "CollisionMgr.h"
//Object
#include "StaticMesh.h"
#include "StaticMeshShader.h"
#include "DefaultShader.h"

CTestWeapon::CTestWeapon(CDevice * pDevice)
	: CObj(pDevice)
	, m_bCreate(false)
{

}

CTestWeapon::~CTestWeapon()
{
	Release();
}

HRESULT CTestWeapon::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_pInfo->m_vScale = D3DXVECTOR3(0.3f, 0.3f, 0.3f);

	m_eRenderType = RENDERTYPE_NONEALPHA;

	return S_OK;
}

int CTestWeapon::Update()
{
	//여기서 플레이어 팔 행렬이랑 합쳐주자...


	CObj::Update_Component();

	return 0;
}

void CTestWeapon::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CStaticMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pLightInfo, m_pTexture->GetResource());

	m_pBuffer->Render(DXGI_32);

#ifdef _DEBUG	//디버그 모드일때만 바운딩박스 그리자 
	if (((CMesh*)m_pBuffer)->GetBoundingBoxBuffer() != NULL)
	{
		((CDefaultShader*)m_pBoundingBoxShader)->SetShaderParameter(matWorld, m_matView, m_matProj, ((CMesh*)m_pBuffer)->GetBoundingBoxBuffer(), m_bCollision);

		((CMesh*)m_pBuffer)->GetBoundingBoxBuffer()->Render(DXGI_16);
	}
#endif
}

HRESULT CTestWeapon::AddComponent(void)
{
	CComponent*		pComponent = NULL;

	//Transform
	pComponent = m_pInfo = CTransform::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Transform", pComponent));

	//Static Buffer
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Mesh_Player");
	m_pBuffer = dynamic_cast<CStaticMesh*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Player");
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

	//Bounding Box Shader
	m_pBoundingBoxShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Default"));

	return S_OK;
}

void CTestWeapon::Set_ConstantTable(void)
{

}

CTestWeapon * CTestWeapon::Create(CDevice * pDevice)
{
	CTestWeapon*	pObj = new CTestWeapon(pDevice);

	if (FAILED(pObj->Init()))
		Safe_Delete(pObj);

	return pObj;
}

void CTestWeapon::Release()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pBoundingBoxShader);
}

void CTestWeapon::RenderForShadow(LightMatrixStruct tLight)
{
}



