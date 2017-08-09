#include "stdafx.h"
#include "Light.h"
//Manager
#include "ResourceMgr.h"
#include "SceneMgr.h"
#include "GraphicDevice.h"
//Object
#include "RenderTex.h"
#include "Shader.h"
#include "Transform.h"
#include "Obj.h"
#include "Camera.h"
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
	if (m_pTargetBuffer == NULL)
		m_pTargetBuffer = CRenderTex::Create(m_pDevice);

	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	memcpy(&m_LightInfo, pLightInfo, sizeof(LightInfo));

	//셰이더 가져옴
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

	//방향광 TEST용
	if (m_LightInfo.eLightType == LIGHT_SPOT)
	{
		float ffieldOfView, fscreenAspect;

		// FoV 소스
		ffieldOfView = (float)(D3DXToRadian(45.f));
		fscreenAspect = float(WINCX) / WINCY;

		// 라이트 기준 Proj Matrix 생성
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
			matViewInv, matProjInv, &m_LightInfo,
			&(*pResourceView), SHADERRESOURCEVIEW_NUM, m_pCam);

		m_pTargetBuffer->Render(DXGI_16);
	}
}

LightMatrixStruct CLight::GetLightMatrix(void)
{
	if (m_pCam == NULL)
		m_pCam = CSceneMgr::GetInstance()->GetScene()->GetMainCam();

	D3DXVECTOR3 up = D3DXVECTOR3(0.f, 1.f, 0.f);
	D3DXVECTOR3 vLightPos = D3DXVECTOR3(m_LightInfo.pos.x, m_LightInfo.pos.y, m_LightInfo.pos.z);

	/*const CObj* pPlayer = CSceneMgr::GetInstance()->GetScene()->GetObj(CLayer::LAYERTYPE_GAMELOGIC, L"Player");

	if (((CObj*)pPlayer)->GetStageState() == STAGESTATE_READY)
	{
		D3DXMatrixLookAtLH(&m_LightMatrixStruct.LightView, &vLightPos, &D3DXVECTOR3(0.f, 0.f, 0.f), &up);

		return m_LightMatrixStruct;
	}*/

	const CComponent*	pPlayerInfo = CSceneMgr::GetInstance()->GetScene()->GetComponent(CLayer::LAYERTYPE_GAMELOGIC, L"Player", L"Transform");
	D3DXVECTOR3 vPlayerPos = ((CTransform*)pPlayerInfo)->m_vPos;
	D3DXVECTOR3 vPlayerToLight;
	vPlayerToLight = D3DXVECTOR3(0.3f, 1.f, -0.3f);

	D3DXVec3Normalize(&vPlayerToLight, &vPlayerToLight);
	vLightPos = vPlayerPos + vPlayerToLight * ((CCamera*)m_pCam)->GetDistEyeToAt() * 3.f;

	//실제 라이트의 Pos도 바꿔준다
	m_LightInfo.pos.x = vLightPos.x;
	m_LightInfo.pos.y = vLightPos.y;
	m_LightInfo.pos.z = vLightPos.z;

	// 뷰 메트릭스 셋팅											
	D3DXMatrixLookAtLH(&m_LightMatrixStruct.LightView, &vLightPos, &vPlayerPos, &up);

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

