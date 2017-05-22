#include "stdafx.h"
#include "Bead.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "TimeMgr.h"
#include "CollisionMgr.h"
#include "SceneMgr.h"
//Object
#include "Scene.h"
#include "Player.h"
#include "StaticMeshShader.h"
#include "StaticMesh.h"
#include "DefaultShader.h"
#include "DepthShader.h"
//server
#include "Protocol.h"
#include "ServerMgr.h"

CBead::CBead(CDevice * pDevice)
	: CObj(pDevice)
	, m_eBeadType(BEAD_END)
	, m_fInitTime(0.f)
	, m_iOwnerID(UNKNOWN_VALUE)
{
}
CBead::~CBead()
{
	Release();
}

HRESULT CBead::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_NONEALPHA;

	return S_OK;
}

int CBead::Update()
{
	if (m_iOwnerID != UNKNOWN_VALUE)
		return 0;

	CObj::Update_Component();

	if (m_fInitTime < 1.f)	//Collision 체크 오류 방지용
		m_fInitTime += CTimeMgr::GetInstance()->GetTime();
	else
	{
		if (CCollisionMgr::GetInstance()->Collision_AABB_Bead(L"Player", this))
		{
			//플레이어와 구슬의 충돌
			CS_CollisionPlayerBead tPacket;
			tPacket.m_iMyID = CServerMgr::GetInstance()->GetClientID();
			tPacket.m_iBeadID = m_eBeadType;
			CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
		}
	}

	return 0;
}

void CBead::Render()
{
	if (m_iOwnerID != UNKNOWN_VALUE)
		return;

	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CStaticMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pLightInfo, m_pTexture->GetResource());

	m_pBuffer->Render(DXGI_32, DRAW_NORMAL);

#ifdef _DEBUG	//디버그 모드일때만 바운딩박스 그리자 
	if (((CMesh*)m_pBuffer)->GetBoundingBoxBuffer() != NULL)
	{
		((CDefaultShader*)m_pBoundingBoxShader)->SetShaderParameter(matWorld, m_matView, m_matProj, ((CMesh*)m_pBuffer)->GetBoundingBoxBuffer(), m_bCollision);

		((CMesh*)m_pBuffer)->GetBoundingBoxBuffer()->Render(DXGI_16);
	}
#endif
}

void CBead::RenderForShadow(LightMatrixStruct tLight)
{
	if (m_iOwnerID != UNKNOWN_VALUE)
		return;

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CDepthShader*)m_pDepthShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pBuffer);

	m_pBuffer->Render(DXGI_32);
}

HRESULT CBead::AddComponent(void)
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
		L"Mesh_Test");
	m_pBuffer = dynamic_cast<CBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Mesh", pComponent));

	//Texture
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_TEXTURE,
		L"Texture_Test");
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

	//Depth Shader
	m_pDepthShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Depth"));

	return S_OK;
}

void CBead::SetBeadType(int iType)
{
	switch (iType)
	{
	case 0:
		m_eBeadType = BEAD_C;
		break;
	case 1:
		m_eBeadType = BEAD_L;
		break;
	case 2:
		m_eBeadType = BEAD_O;
		break;
	case 3:
		m_eBeadType = BEAD_N;
		break;
	case 4:
		m_eBeadType = BEAD_E;
		break;
	default:
		break;
	}
}

CBead * CBead::Create(CDevice * pDevice, OBJTYPE eType)
{
	CBead*	pBead = new CBead(pDevice);

	if (FAILED(pBead->Init()))
		Safe_Delete(pBead);

	pBead->SetObjType(eType);

	return pBead;
}

void CBead::Release()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pDepthShader);
}



