#include "stdafx.h"
#include "Bot.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "CollisionMgr.h"
#include "NaviMgr.h"
#include "TimeMgr.h"
#include "Frustum.h"
//Object
#include "DynamicMesh.h"
#include "DynamicMeshShader.h"
#include "DefaultShader.h"
#include "DepthDynamicShader.h"
//server
#include "Protocol.h"
#include "ServerMgr.h"

CBot::CBot(CDevice * pDevice)
	: CObj(pDevice)
	, m_bCreate(FALSE)
	, m_fInitTime(0.f)
	, m_dwIdx(0)
	, m_bCulling(FALSE)

	, m_iAniCount(0)
	, m_fRadious(0.f)
	, m_iCurAniCount(0)
	, m_bCheckAniEnd(FALSE)
	, m_bRotateEnd(TRUE)
	, m_bInitState(FALSE)

	, m_iID(-1)
{
}

CBot::~CBot()
{
	Release();
}

HRESULT CBot::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType				= RENDERTYPE_NONEALPHA;
	m_eObjState	= m_eCurState	= OBJ_STATE_IDEL;
	m_pInfo->m_fAngle[ANGLE_Y] = 0.f;
		
	return S_OK;
}

int CBot::Update()
{
	// Collision Check 오류 방지용 --------------------------
	if (m_fInitTime < 1.f)
		m_fInitTime += CTimeMgr::GetInstance()->GetTime();
	else
		m_bCreate = TRUE;

	if(m_bCreate && m_eObjState != OBJ_STATE_DIE)
		Collision_AABB();	//Collision Check
	// ------------------------------------------------------

	
	// 행동 체크 관련 함수들
	StateCheck();
	ActionCheck();

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	//Component Update
	CObj::Update_Component();

	if (m_bCollision == 1)
	{
		// 죽은 모션을 유지하고 놔둔다.
		m_eObjState = OBJ_STATE_DIE;
	}

	return 0;
}

void CBot::Render()
{
	// Frustum Check
	//Radious 는 Unit의 Height만큼
	m_bCulling = CFrustum::GetInstance()->SphereInFrustum(&m_pInfo->m_vPos, 2.f);
	if (m_bCulling == FALSE)	
		return;

	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	((CDynamicMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

	PlayAnimation();				//DynamicMesh

#ifdef _DEBUG	//디버그 모드일때만 바운딩박스 그리자 
	if (((CMesh*)m_pBuffer)->GetBoundingBoxBuffer() != NULL)
	{
		((CDefaultShader*)m_pBoundingBoxShader)->SetShaderParameter(matWorld, m_matView, m_matProj, ((CMesh*)m_pBuffer)->GetBoundingBoxBuffer(), m_bCollision);

		((CMesh*)m_pBuffer)->GetBoundingBoxBuffer()->Render(DXGI_16);
	}
#endif
}

void CBot::RenderForShadow(LightMatrixStruct tLight)
{
	if (!m_bCulling)	// 컬링에 걸린다면 return한다.
		return;

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);

	//셰이더 파라미터 세팅
	((CDepthDynamicShader*)m_pDepthShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pBuffer);

	((CDynamicMesh*)m_pBuffer)->RenderMesh(m_eObjState);
}

HRESULT CBot::AddComponent(void)
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
		L"Shader_DepthDynamic"));

	return S_OK;
}

void CBot::Collision_AABB(void)
{
	if (m_eObjState == OBJ_STATE_DIE)
		return;

	if (CCollisionMgr::GetInstance()->Collision_AABB(L"Player", this))
	{
		m_bCollision = TRUE;


		CS_CollisionPlayerBot tPacket;
		tPacket.m_iMyID = CServerMgr::GetInstance()->GetClientID();
		tPacket.m_iBotID = m_iID;
		CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
	}
	else
		m_bCollision = FALSE;
}

void CBot::StateCheck(void)
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

void CBot::ActionCheck(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();

	D3DXMATRIX matRotate;
	D3DXVECTOR3 vDir = g_vLook;
	float fCurAngle = 0.f;

	switch (m_eObjState)
	{
	case OBJ_STATE_IDEL:
		// idel 상태에서는 움직이지 않는다
		m_eCurState = m_eObjState;
		break;
	case OBJ_STATE_MOVE:	
		// 처음 상태를 받았을 떄는 바로 회전시킨다.
		if (!m_bInitState)
		{
			D3DXMatrixIdentity(&matRotate);
			D3DXMatrixRotationY(&matRotate, m_fRadious);

			D3DXVec3TransformNormal(&vDir, &vDir, &matRotate);

			//Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos
				, &(vDir * fTime), m_dwIdx);

			//m_pInfo->m_vPos += vDir * fTime;
			m_pInfo->m_fAngle[ANGLE_Y] = m_fRadious;
			
			m_eCurState = m_eObjState;

			m_bRotateEnd = TRUE;
			m_bInitState = TRUE;
		}
		else
		{
			float fTime = CTimeMgr::GetInstance()->GetTime();
			
			// 상태가 이동이나 회전이 끝나지 않았다.
			if (!m_bRotateEnd)
			{
				float fIncludeAngle = m_pInfo->m_fAngle[ANGLE_Y] - m_fRadious;

				D3DXMatrixIdentity(&matRotate);

				//현재 각이 서버로부터 받은 각보다 작다 == 플레이어의 회전속도와 같게 회전
				if (m_pInfo->m_fAngle[ANGLE_Y] < m_fRadious)
					m_pInfo->m_fAngle[ANGLE_Y] += D3DXToRadian(90.f * fTime);
				else //현재 각이 서버로부터 받은 각보다 크다 == 플레이어의 회전속도와 같게 회전
					m_pInfo->m_fAngle[ANGLE_Y] -= D3DXToRadian(90.f * fTime);

				// 회전이 끝나지 않았다면 IDEL 상태를 넣어준다.
				m_eCurState = OBJ_STATE_IDEL;

				// 서버로 부터 받은 각과의 차이가 같다.
				if (abs(fIncludeAngle) < 0.1f)
				{
					m_eCurState = m_eObjState;	// 회전이 끝난 후 현재 상태에 넣어준다.
					m_pInfo->m_fAngle[ANGLE_Y] = m_fRadious;	// 회전이 끝났으니 각을 고정해준다.
					m_bRotateEnd = TRUE;
				}
			}
			else // 회전이 끝난 상태
			{
				D3DXMatrixIdentity(&matRotate);
				D3DXMatrixRotationY(&matRotate, m_pInfo->m_fAngle[ANGLE_Y]);

				D3DXVec3TransformNormal(&vDir, &vDir, &matRotate);

				//Move On Navi
				m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos
					, &(vDir * fTime), m_dwIdx);
			}
		}
		break;
	case OBJ_STATE_DIE:
		// Die 상태에서도 움직이지 않는다.
		m_eCurState = m_eObjState;
		break;
	default:
		break;
	}
}

void CBot::PlayAnimation(void)
{
	//플레이는 현재 실제 상태로 플레이한다. == m_eCurState
	switch (m_eCurState)
	{
	case OBJ_STATE_IDEL:
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_IDEL);
		break;
	case OBJ_STATE_MOVE:
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_MOVE);
		break;
	case OBJ_STATE_DIE:
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_DIE);
		break;
	default:
		break;
	}
}

void CBot::StateSetting(ObjState eState, int iCount, float fRadious)
{
	m_iCurAniCount = 0;			//Cur카운트를 0으로 초기화시킨다.
	m_iSaveAniCount = iCount;
	m_fRadious = fRadious;
	m_eObjState = eState;
	m_bRotateEnd = FALSE;
}

CBot * CBot::Create(CDevice * pDevice, OBJTYPE eType)
{
	CBot*	pObj = new CBot(pDevice);

	if (FAILED(pObj->Init()))
		Safe_Delete(pObj);

	pObj->SetObjType(eType);

	return pObj;
}

void CBot::Release()
{
	Safe_Release(m_pShader);
	Safe_Release(m_pBoundingBoxShader);
	Safe_Release(m_pDepthShader);
}