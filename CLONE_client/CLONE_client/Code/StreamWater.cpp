#include "stdafx.h"
#include "StreamWater.h"
//Manager
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "TimeMgr.h"
//Object
#include "WaterShader.h"
#include "DepthShader.h"
#include "Camera.h"

CStreamWater::CStreamWater(CDevice * pDevice)
	: CObj(pDevice)
	, m_fTime(0.f)
	, m_fWaterTranslation(0.f)
	, m_fWaterIntensity(0.02f)
	, m_parrVertex(NULL)
{

}

CStreamWater::~CStreamWater()
{
	Release();
}

HRESULT CStreamWater::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	//Water 버퍼 위치 수정
	m_pInfo->m_vPos = D3DXVECTOR3(0.f, WATER_HEIGHT, 0.f);
	m_pInfo->m_vScale = D3DXVECTOR3(1.9f, 1.f, 9.75f);

	return S_OK;
}

int CStreamWater::Update()
{
	m_fWaterTranslation += CTimeMgr::GetInstance()->GetTime() * 0.01f;
	if (m_fWaterTranslation > 1.f)
		m_fWaterTranslation -= 1.f;

	CObj::Update_Component();

	return 0;
}

void CStreamWater::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CWaterShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj, *((CCamera*)m_pCam)->GetReflectViewMatrix(WATER_HEIGHT),
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource(), CRenderTargetMgr::GetInstance()->GetReflectionView(), CRenderTargetMgr::GetInstance()->GetRefactionView(),
		m_fWaterTranslation, m_fWaterIntensity);

	m_pBuffer->Render(DXGI_32);
}

void CStreamWater::RenderForShadow(LightMatrixStruct tLight)
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

void CStreamWater::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}

HRESULT CStreamWater::AddComponent(void)
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
		L"Buffer_Water");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Buffer", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Water");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//LightInfo
	m_pLightInfo = CLightMgr::GetInstance()->GetLightInfo(0);

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Water"));

	//Depth Shader
	m_pDepthShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Depth"));

	return S_OK;
}

CStreamWater * CStreamWater::Create(CDevice * pDevice, OBJTYPE eType)
{
	CStreamWater*	pWater = new CStreamWater(pDevice);

	if (FAILED(pWater->Init()))
		Safe_Delete(pWater);

	pWater->SetObjType(eType);

	return pWater;
}

void CStreamWater::Release()
{
	Safe_Delete_Array(m_parrVertex);
	Safe_Release(m_pShader);
	Safe_Release(m_pDepthShader);
}



