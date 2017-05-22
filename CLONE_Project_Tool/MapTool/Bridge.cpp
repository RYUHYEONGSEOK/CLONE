#include "stdafx.h"
#include "Bridge.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Info.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "Camera.h"
#include "ObjMgr.h"
#include "Device.h"


CBridge::CBridge()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_wstrObjKey = L"Bridge";
}


CBridge::~CBridge()
{
	Release();
}

HRESULT CBridge::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;

	return S_OK;
}

int CBridge::Update(void)
{
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	CObj::Update();
	return 0;
}

void CBridge::Render(void)
{
	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);
	//////////////////
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	SelectCheck();
	m_pBuffer->Render();
}

CBridge * CBridge::Create(void)
{
	CBridge*	pObj = new CBridge;
	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}

void CBridge::Release(void)
{
	Safe_Delete(m_pBuffer);
	Safe_Delete(m_pInfo);
	Safe_Delete(m_pVertexShader);
	Safe_Delete(m_pPixelShader);
	Safe_Delete(m_pTexture);
}

HRESULT CBridge::AddComponent(void)
{
	CComponent* pComponent = NULL;
	char cModelPath[MAX_PATH] = "../Resource/Land/bridge.FBX";


	m_pBuffer = CStaticMesh::Create(cModelPath);
	pComponent = m_pBuffer;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Bridge", pComponent));

	m_pInfo = CInfo::Create(g_vLook);
	pComponent = m_pInfo;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Info", pComponent));

	m_pTexture = CTexture::Create(L"../Resource/MeshImage/bridge.dds");
	pComponent = m_pTexture;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");


	return S_OK;
}
