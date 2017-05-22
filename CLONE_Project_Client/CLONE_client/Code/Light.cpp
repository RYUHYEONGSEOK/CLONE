#include "stdafx.h"
#include "Light.h"
//Manager
#include "ResourceMgr.h"
#include "SceneMgr.h"
#include "GraphicDevice.h"
//Object
#include "RenderTex.h"
#include "Shader.h"
#include "Scene.h"
#include "DirLightShader.h"
#include "PointLightShader.h"

CLight::CLight(CDevice*	pDevice)
	: m_pDevice(pDevice)
	, m_pCam(NULL)
	, m_pShader(NULL)
	, m_pTargetBuffer(NULL)
{
	ZeroMemory(&m_LightInfo, sizeof(LightInfo));
}

CLight::~CLight(void)
{
	Release();
}

HRESULT CLight::Init_Light(const LightInfo* pLightInfo, const WORD& wLightIdx)
{
	if(m_pTargetBuffer == NULL)
		m_pTargetBuffer = CRenderTex::Create(m_pDevice);

	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	memcpy(&m_LightInfo, pLightInfo, sizeof(LightInfo));

	//���̴� ������
	if (m_LightInfo.eLightType == LIGHT_DIR)
	{
		m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
			CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
			CResourceMgr::RESOURCE_TYPE_SHADER,
			L"Shader_DirLight"));
		NULL_CHECK_MSG(m_pShader, L"Get Shader Failed");
	}
	else if (m_LightInfo.eLightType == LIGHT_POINT)
	{
		m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
			CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
			CResourceMgr::RESOURCE_TYPE_SHADER,
			L"Shader_PointLight"));
		NULL_CHECK_MSG(m_pShader, L"Get Shader Failed");
	}
	
	//���Ɽ TEST��
	if (m_LightInfo.eLightType == LIGHT_SPOT)
	{
		float ffieldOfView, fscreenAspect;

		// FoV �ҽ�
		ffieldOfView = (float)(D3DXToRadian(45.f));
		fscreenAspect = float(WINCX) / WINCY;

		// ����Ʈ ���� Proj Matrix ����
		D3DXMatrixPerspectiveFovLH(&m_LightMatrixStruct.LightProj, ffieldOfView, fscreenAspect, 1.f, 1000.f);
	}

	return S_OK;
}

void CLight::Render_Light(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj,
	XMFLOAT4X4 matViewInv, XMFLOAT4X4 matProjInv, LightMatrixStruct tLightMatInfo,
	ID3D11ShaderResourceView* pNormalResource, 
	ID3D11ShaderResourceView* pDepthResource,
	ID3D11ShaderResourceView* pShadowResource)
{
	if (m_pCam == NULL)
		m_pCam = CSceneMgr::GetInstance()->GetScene()->GetMainCam();

	ID3D11ShaderResourceView* pResourceView[SHADERRESOURCEVIEW_NUM] =
	{
		pNormalResource, pDepthResource, pShadowResource
	};

	if (m_LightInfo.eLightType == LIGHT_DIR)
	{
		((CDirLightShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj, 
			matViewInv, matProjInv, &m_LightInfo, &tLightMatInfo, 
			&(*pResourceView), SHADERRESOURCEVIEW_NUM, m_pCam);

		m_pTargetBuffer->Render(DXGI_16);
	}
	else if (m_LightInfo.eLightType == LIGHT_POINT)
	{
		((CPointLightShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj,
			matViewInv, matProjInv,
			&m_LightInfo, &(*pResourceView),
			SHADERRESOURCEVIEW_NUM, m_pCam);

		m_pTargetBuffer->Render(DXGI_16);
	}
}

LightMatrixStruct CLight::GetLightMatrix(void)
{
	D3DXVECTOR3 up = D3DXVECTOR3(0.f, 1.f, 0.f);
	D3DXVECTOR3 vLightPos = D3DXVECTOR3(m_LightInfo.pos.x, m_LightInfo.pos.y, m_LightInfo.pos.z);

	// �� ��Ʈ���� ����												�ӽ� = LookAt�ڸ���
	D3DXMatrixLookAtLH(&m_LightMatrixStruct.LightView, &vLightPos, &D3DXVECTOR3(0.f, 0.f, 0.f), &up);

	return m_LightMatrixStruct;
}
	
CLight* CLight::Create(CDevice*	pDevice, const LightInfo* pLightInfo, const WORD& dwLightIdx)
{
	CLight*		pLight = new CLight(pDevice);

	if (FAILED(pLight->Init_Light(pLightInfo, dwLightIdx)))
		Safe_Delete(pLight);

	return pLight;
}

void CLight::Release(void)
{
	Safe_Release(m_pShader);
	Safe_Delete(m_pTargetBuffer);
}

LightInfo* CLight::GetLightInfo(void)
{
	return &m_LightInfo;
}

