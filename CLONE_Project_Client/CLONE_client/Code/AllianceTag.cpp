#include "stdafx.h"
#include "AllianceTag.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
//Object
#include "TextureShader.h"
#include "Camera.h"

CAllianceTag::CAllianceTag(CDevice * pDevice)
	: CUI(pDevice)
{

}

CAllianceTag::~CAllianceTag()
{
	Release();
}

HRESULT CAllianceTag::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_NONEALPHA;

	m_pInfo->m_vScale = D3DXVECTOR3(0.3f, 0.3f, 0.3f);

	return S_OK;
}

int CAllianceTag::Update()
{
	SetDirection();

	CObj::Update_Component();

	D3DXMATRIX	matBill;
	D3DXMatrixIdentity(&matBill);

	//카메라의 View Pos 가져옴
	matBill = *((CCamera*)m_pCam)->GetViewMatrix();
	ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

	D3DXMatrixInverse(&matBill, NULL, &matBill);
	m_pInfo->m_matWorld = matBill * m_pInfo->m_matWorld;

	return 0;
}

void CAllianceTag::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

	m_pBuffer->Render(DXGI_16);
}

HRESULT CAllianceTag::AddComponent(void)
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
		L"Buffer_UI");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Buffer", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_CloneIcon");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Texture"));

	return S_OK;
}

void CAllianceTag::SetDirection(void)
{
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);
	D3DXVec3Normalize(&m_pInfo->m_vDir, &m_pInfo->m_vDir);
}

CAllianceTag * CAllianceTag::Create(CDevice * pDevice)
{
	CAllianceTag*	pTerrain = new CAllianceTag(pDevice);

	if (FAILED(pTerrain->Init()))
		Safe_Delete(pTerrain);

	return pTerrain;
}

void CAllianceTag::Release()
{
	Safe_Release(m_pShader);
}

void CAllianceTag::RenderForShadow(LightMatrixStruct tLight)
{
}



