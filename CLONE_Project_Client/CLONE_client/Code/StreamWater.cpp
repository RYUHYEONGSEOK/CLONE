#include "stdafx.h"
#include "StreamWater.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "TimeMgr.h"
//Object
#include "WaterShader.h"
#include "DepthShader.h"
#include "Camera.h"

CStreamWater::CStreamWater(CDevice * pDevice)
	: CObj(pDevice)
	, m_fTime(0.f)
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
	//m_pInfo->m_vPos = D3DXVECTOR3(-(WATER_CNTX) + 1.f, WATER_HEIGHT, -(WATER_CNTZ));

	m_pInfo->m_vPos = D3DXVECTOR3(-(WATER_CNTX) + 1.f, WATER_HEIGHT, -(WATER_CNTZ) + 1.f);

	m_parrVertex = new VertexTexture[WATER_CNTX * WATER_CNTZ];

	return S_OK;
}

int CStreamWater::Update()
{
	CObj::Update_Component();

	return 0;
}

void CStreamWater::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//웨이브
	WaveAction();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CWaterShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

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

void CStreamWater::WaveAction(void)
{
	// 물 움직임
	m_fTime += CTimeMgr::GetInstance()->GetTime();

	// 너무 빨리 움직이기 시작하면 터진다...
	if (m_fTime < 1.f)	
		return;
	// 혹시 오버플로우로 터지는것을 막는다.
	if (m_fTime > 10000.f)	
		m_fTime = 1.f;

	ID3D11DeviceContext* pDeviceContext = m_pDevice->GetDeviceContext();
	D3D11_MAPPED_SUBRESOURCE	mappedResource;

	D3DXVECTOR3 vStart[10] = { D3DXVECTOR3(50, 0, 25), D3DXVECTOR3(50, 0, -50), D3DXVECTOR3(-50, 0, -35),
		D3DXVECTOR3(20, 0, -20), D3DXVECTOR3(75, 0, 75) };
	float		fHeight[10] = { 0.35f, 0.07f, 0.4f, 0.1f, 0.15f };
	float		fV[10] = { 0.6f, 1.5f, 0.3f, 1.f, 0.35f };
	float		fLength[10] = { 60, 9.5f, 45, 15.5f, 14.5f };

	pDeviceContext->Map(m_pBuffer->GetVertexBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	int		iIndex = 0;

	for (int z = 0; z < WATER_CNTZ; ++z)
	{
		for (int x = 0; x < WATER_CNTX; ++x)
		{
			iIndex = z * WATER_CNTX + x;

			m_parrVertex[iIndex].vPos = D3DXVECTOR3(float(x) * 2.f, 0.f, float(z) * 2.f);

			float y = 0;

			for (int k = 0; k < 5; ++k)
			{
				float fDist = D3DXVec3Length(&(vStart[k] - m_parrVertex[iIndex].vPos));
				y += fHeight[k] * powf(sin(m_fTime * fV[k] + 6.28 * (fDist / fLength[k])), 8);
			}
			m_parrVertex[iIndex].vPos.y = y - 1.f;
			m_parrVertex[iIndex].vNormal = D3DXVECTOR3(0.f, 1.f, 0.f);
			m_parrVertex[iIndex].vTextureUV = D3DXVECTOR2(x / (WATER_CNTX - 1.f), z / (WATER_CNTZ - 1.f));
		}
	}

	// 노멀을 다시 잡아줄 필요는 없어보인다...
	/*
	arIndex[iCnt]._1 = i * m_xCnt + j;
	arIndex[iCnt]._2 = i * m_xCnt + j + 1;
	arIndex[iCnt++]._3 = (i + 1) * m_xCnt + j + 1;

	arIndex[iCnt]._1 = i * m_xCnt + j;
	arIndex[iCnt]._2 = (i + 1) * m_xCnt + j + 1;
	arIndex[iCnt++]._3 = (i + 1) * m_xCnt + j;
	*/

	/*for (int z = 0; z < WATER_CNTZ; ++z)
	{
		for (int x = 0; x < WATER_CNTX; ++x)
		{
			D3DXVECTOR3 vLeft = m_parrVertex[(z + 1) * WATER_CNTX + x + 1].vPos - m_parrVertex[z * WATER_CNTX + x].vPos;
			D3DXVECTOR3 vRight = m_parrVertex[z * WATER_CNTX + x + 1].vPos - m_parrVertex[z * WATER_CNTX + x].vPos;

			D3DXVECTOR3 vNormal;

			D3DXVec3Cross(&vNormal, &vLeft, &vRight);
			D3DXVec3Normalize(&vNormal, &vNormal);

			m_parrVertex[z * WATER_CNTX + x].vNormal += vNormal;
			m_parrVertex[z * WATER_CNTX + x + 1].vNormal += vNormal;
			m_parrVertex[(z + 1) * WATER_CNTX + x + 1].vNormal += vNormal / 6.f;

			vLeft = m_parrVertex[(z + 1) * WATER_CNTX + x].vPos - m_parrVertex[z * WATER_CNTX + x].vPos;
			vRight = m_parrVertex[(z + 1) * WATER_CNTX + x + 1].vPos - m_parrVertex[z * WATER_CNTX + x].vPos;

			D3DXVec3Cross(&vNormal, &vLeft, &vRight);
			D3DXVec3Normalize(&vNormal, &vNormal);

			m_parrVertex[z * WATER_CNTX + x].vNormal += vNormal;
			m_parrVertex[(z + 1) * WATER_CNTX + x].vNormal += vNormal;
			m_parrVertex[(z + 1) * WATER_CNTX + x + 1].vNormal += vNormal / 6.f;
		}
	}*/

	memcpy(mappedResource.pData, m_parrVertex, sizeof(VertexTexture) * (WATER_CNTX * WATER_CNTZ));

	pDeviceContext->Unmap(m_pBuffer->GetVertexBuffer(), 0);
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



