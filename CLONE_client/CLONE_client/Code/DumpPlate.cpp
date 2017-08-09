#include "stdafx.h"
#include "DumpPlate.h"
//Manager
#include "ResourceMgr.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
//Object
#include "DepthShader.h"

CDumpPlate::CDumpPlate(CDevice * pDevice)
	: CObj(pDevice)
{

}

CDumpPlate::~CDumpPlate()
{
	Release();
}

HRESULT CDumpPlate::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_pInfo->m_vPos		= D3DXVECTOR3(-50.f, WATER_HEIGHT, -50.f);
	m_pInfo->m_vScale	= D3DXVECTOR3(100.f, 1.f, 100.f);

	m_eRenderType = RENDERTYPE_PRIORITY;

	return S_OK;
}

int CDumpPlate::Update()
{
	CObj::Update_Component();

	return 0;
}

void CDumpPlate::Render()
{
	
}

void CDumpPlate::RenderForShadow(LightMatrixStruct tLight)
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

void CDumpPlate::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}

HRESULT CDumpPlate::AddComponent(void)
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

	//LightInfo
	m_pLightInfo = CLightMgr::GetInstance()->GetLightInfo(0);

	//Depth Shader
	m_pDepthShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Depth"));

	return S_OK;
}

CDumpPlate * CDumpPlate::Create(CDevice * pDevice, OBJTYPE eType)
{
	CDumpPlate*	pObj = new CDumpPlate(pDevice);

	if (FAILED(pObj->Init()))
		Safe_Delete(pObj);

	pObj->SetObjType(eType);

	return pObj;
}

void CDumpPlate::Release()
{
	Safe_Release(m_pDepthShader);
}





