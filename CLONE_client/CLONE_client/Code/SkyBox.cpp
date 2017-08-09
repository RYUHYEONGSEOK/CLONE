#include "stdafx.h"
#include "SkyBox.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
//Object
#include "SkyBoxShader.h"
#include "Camera.h"

CSkyBox::CSkyBox(CDevice * pDevice)
	: CObj(pDevice)
{
		
}

CSkyBox::~CSkyBox()
{
	Release();
}

HRESULT CSkyBox::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_pInfo->m_vScale = D3DXVECTOR3(100.f, 100.f, 100.f);

	m_eRenderType = RENDERTYPE_PRIORITY;

	return S_OK;
}

int CSkyBox::Update()
{
	m_pInfo->m_vPos = (*((CCamera*)m_pCam)->GetEye());

	CObj::Update_Component();

	return 0;
}

void CSkyBox::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CSkyBoxShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

	m_pBuffer->Render(DXGI_16);
}

HRESULT CSkyBox::AddComponent(void)
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
		L"Buffer_SkyBox");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Buffer", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_SkyBox");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Texture", pComponent));

	//LightInfo
	m_pLightInfo = CLightMgr::GetInstance()->GetLightInfo(0);

	//Shader
	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_SkyBox"));

	return S_OK;
}

CSkyBox * CSkyBox::Create(CDevice * pDevice, OBJTYPE eType)
{
	CSkyBox*	pSkyBox = new CSkyBox(pDevice);

	if (FAILED(pSkyBox->Init()))
		Safe_Delete(pSkyBox);

	pSkyBox->SetObjType(eType);

	return pSkyBox;
}

void CSkyBox::Release()
{
	Safe_Release(m_pShader);
}

void CSkyBox::RenderForShadow(LightMatrixStruct tLight)
{
}

void CSkyBox::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}



