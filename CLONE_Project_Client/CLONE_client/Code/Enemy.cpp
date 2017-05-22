#include "stdafx.h"
#include "Enemy.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "CollisionMgr.h"
#include "TimeMgr.h"
#include "Frustum.h"
//Object
#include "DynamicMesh.h"
#include "DynamicMeshShader.h"
#include "DefaultShader.h"
#include "DepthShader.h"

CEnemy::CEnemy(CDevice * pDevice)
	: CObj(pDevice)
	, m_bCreate(FALSE)
	, m_fInitTime(0.f)

	, m_iAniCount(0)
	, m_fRadious(0.f)
	, m_iCurAniCount(0)
	, m_bCheckAniEnd(FALSE)
{

}

CEnemy::~CEnemy()
{
	Release();
}

HRESULT CEnemy::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_NONEALPHA;
	m_eObjState = OBJ_STATE_MOVE;

	// Test Check ------------------------------------------
	// 이거 대신 서버에서 정보를 받아온다.
	float fTestRadious = D3DXToRadian((float)(rand() % 360));
	int iTestCount = rand() % 5 + 2;
	ObjState eTestState = (ObjState)(rand() % 2);
	if (eTestState == OBJ_STATE_RUN)
		eTestState = OBJ_STATE_IDEL;
	// ----------------------------------------------------

	//Test 임의 팀셋팅
	m_eTeamType = TEAM_BLUE;

	// 초기행동 셋팅
	StateSetting(eTestState, iTestCount, fTestRadious);

	return S_OK;
}

int CEnemy::Update()
{
	// Collision Check 오류 방지용 --------------------------
	if (m_fInitTime < 1.f)
		m_fInitTime += CTimeMgr::GetInstance()->GetTime();
	else
		m_bCreate = TRUE;

	if (m_bCreate)
		Collision_AABB();	//Collision Check
	// ------------------------------------------------------

	// Collision Terrain ------------------------------------
	//if (!m_bCreate)
	//	CCollisionMgr::GetInstance()->Collision_Terrain(this);
	// ------------------------------------------------------


	// Save애니 카운트보다 Cur애니 카운트가 커진다 == 서버로부터 다시 입력받을 시점
	if (m_iSaveAniCount < m_iCurAniCount)
	{
		//Test Check 이거 대신 서버에서 정보를 받아온다.
		float fTestRadious = D3DXToRadian((float)(rand() % 360));
		int iTestCount = rand() % 5 + 2;	// 이거 수정해야댐!
		ObjState eTestState = (ObjState)(rand() % 2);
		if (eTestState == OBJ_STATE_RUN)
			eTestState = OBJ_STATE_IDEL;

		// 다음행동 셋팅
		StateSetting(eTestState, iTestCount, fTestRadious);
	}

	// 행동 체크 관련 함수들
	StateCheck();
	ActionCheck();

	//Component Update
	CObj::Update_Component();

	if (m_bCollision == 1)
		return CObj::OBJ_STATE_DIE;

	return 0;
}

void CEnemy::Render()
{
	// Frustum Check
	if (CFrustum::GetInstance()->SphereInFrustum(&m_pInfo->m_vPos, 2.f) == FALSE)	//Radious 는 Unit의 Height만큼
		return;

	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CDynamicMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

	//m_pBuffer->Render(DXGI_32);	//Static Mesh
	PlayAnimation();				//DynamicMesh

#ifdef _DEBUG	//디버그 모드일때만 바운딩박스 그리자 
	if (((CMesh*)m_pBuffer)->GetBoundingBoxBuffer() != NULL)
	{
		((CDefaultShader*)m_pBoundingBoxShader)->SetShaderParameter(matWorld, m_matView, m_matProj, ((CMesh*)m_pBuffer)->GetBoundingBoxBuffer(), m_bCollision);

		((CMesh*)m_pBuffer)->GetBoundingBoxBuffer()->Render(DXGI_16);
	}
#endif
}

void CEnemy::RenderForShadow(LightMatrixStruct tLight)
{
	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CDepthShader*)m_pDepthShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pBuffer);

	((CDynamicMesh*)m_pBuffer)->RenderMesh(m_eObjState);
}

HRESULT CEnemy::AddComponent(void)
{
	CComponent*		pComponent = NULL;

	//Transform
	pComponent = m_pInfo = CTransform::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(make_pair(L"Transform", pComponent));

	//Dynamic Buffer
	pComponent = CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_MESH,
		L"Player_AniMesh");
	m_pBuffer = dynamic_cast<CDynamicMesh*>(pComponent);
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
		L"Shader_DynamicMesh"));

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

void CEnemy::Collision_AABB(void)
{
	if (CCollisionMgr::GetInstance()->Collision_AABB(L"Player", this))
		m_bCollision = TRUE;
	else
		m_bCollision = FALSE;
}

void CEnemy::StateCheck(void)
{
	if (((CDynamicMesh*)m_pBuffer)->Animend)
	{
		if (!m_bCheckAniEnd)
		{
			m_bCheckAniEnd = TRUE;
			m_iCurAniCount++;
		}
	}
	else
		m_bCheckAniEnd = FALSE;
}

void CEnemy::ActionCheck(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();

	D3DXMATRIX matRotate;
	D3DXVECTOR3 vDir = g_vLook;

	switch (m_eObjState)
	{
	case OBJ_STATE_IDEL:
		// idel 상태에서는 움직이지 않는다
		break;
	case OBJ_STATE_MOVE:
		D3DXMatrixIdentity(&matRotate);
		D3DXMatrixRotationY(&matRotate, m_fRadious);

		D3DXVec3TransformNormal(&vDir, &vDir, &matRotate);

		m_pInfo->m_vPos += vDir * fTime;
		m_pInfo->m_fAngle[ANGLE_Y] = m_fRadious;
		break;
	case OBJ_STATE_DIE:
		break;
	default:
		break;
	}
}

void CEnemy::PlayAnimation(void)
{
	switch (m_eObjState)
	{
	case OBJ_STATE_IDEL:
		// 추후에 수정
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_IDEL);
		break;
	case OBJ_STATE_MOVE:
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_MOVE);
		break;
	case OBJ_STATE_DIE:
		// 추후에 수정
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_DIE);
		break;
	default:
		break;
	}
}

void CEnemy::StateSetting(ObjState eState, int iCount, float fRadious)
{
	m_iCurAniCount = 0;			//Cur카운트를 0으로 초기화시킨다.
	m_iSaveAniCount = iCount;
	m_fRadious = fRadious;
	m_eObjState = eState;
}

CEnemy * CEnemy::Create(CDevice * pDevice, OBJTYPE eType)
{
	CEnemy*	pObj = new CEnemy(pDevice);

	if (FAILED(pObj->Init()))
		Safe_Delete(pObj);

	pObj->SetObjType(eType);

	return pObj;
}

void CEnemy::Release()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pBoundingBoxShader);
	Safe_Release(m_pDepthShader);
}
