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
	// Collision Check ���� ������ --------------------------
	if (m_fInitTime < 1.f)
		m_fInitTime += CTimeMgr::GetInstance()->GetTime();
	else
		m_bCreate = TRUE;

	if(m_bCreate && m_eObjState != OBJ_STATE_DIE)
		Collision_AABB();	//Collision Check
	// ------------------------------------------------------

	
	// �ൿ üũ ���� �Լ���
	StateCheck();
	ActionCheck();

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	//Component Update
	CObj::Update_Component();

	if (m_bCollision == 1)
	{
		// ���� ����� �����ϰ� ���д�.
		m_eObjState = OBJ_STATE_DIE;
	}

	return 0;
}

void CBot::Render()
{
	// Frustum Check
	//Radious �� Unit�� Height��ŭ
	m_bCulling = CFrustum::GetInstance()->SphereInFrustum(&m_pInfo->m_vPos, 2.f);
	if (m_bCulling == FALSE)	
		return;

	//ī�޶󿡼� ��� �̾ƿ�
	SetViewProjTransform();

	//���� XM��Ʈ���� ����
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//���̴� �Ķ���� ����
	((CDynamicMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

	PlayAnimation();				//DynamicMesh

#ifdef _DEBUG	//����� ����϶��� �ٿ���ڽ� �׸��� 
	if (((CMesh*)m_pBuffer)->GetBoundingBoxBuffer() != NULL)
	{
		((CDefaultShader*)m_pBoundingBoxShader)->SetShaderParameter(matWorld, m_matView, m_matProj, ((CMesh*)m_pBuffer)->GetBoundingBoxBuffer(), m_bCollision);

		((CMesh*)m_pBuffer)->GetBoundingBoxBuffer()->Render(DXGI_16);
	}
#endif
}

void CBot::RenderForShadow(LightMatrixStruct tLight)
{
	if (!m_bCulling)	// �ø��� �ɸ��ٸ� return�Ѵ�.
		return;

	//���� XM��Ʈ���� ����
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);

	//���̴� �Ķ���� ����
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
		// idel ���¿����� �������� �ʴ´�
		m_eCurState = m_eObjState;
		break;
	case OBJ_STATE_MOVE:	
		// ó�� ���¸� �޾��� ���� �ٷ� ȸ����Ų��.
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
			
			// ���°� �̵��̳� ȸ���� ������ �ʾҴ�.
			if (!m_bRotateEnd)
			{
				float fIncludeAngle = m_pInfo->m_fAngle[ANGLE_Y] - m_fRadious;

				D3DXMatrixIdentity(&matRotate);

				//���� ���� �����κ��� ���� ������ �۴� == �÷��̾��� ȸ���ӵ��� ���� ȸ��
				if (m_pInfo->m_fAngle[ANGLE_Y] < m_fRadious)
					m_pInfo->m_fAngle[ANGLE_Y] += D3DXToRadian(90.f * fTime);
				else //���� ���� �����κ��� ���� ������ ũ�� == �÷��̾��� ȸ���ӵ��� ���� ȸ��
					m_pInfo->m_fAngle[ANGLE_Y] -= D3DXToRadian(90.f * fTime);

				// ȸ���� ������ �ʾҴٸ� IDEL ���¸� �־��ش�.
				m_eCurState = OBJ_STATE_IDEL;

				// ������ ���� ���� ������ ���̰� ����.
				if (abs(fIncludeAngle) < 0.1f)
				{
					m_eCurState = m_eObjState;	// ȸ���� ���� �� ���� ���¿� �־��ش�.
					m_pInfo->m_fAngle[ANGLE_Y] = m_fRadious;	// ȸ���� �������� ���� �������ش�.
					m_bRotateEnd = TRUE;
				}
			}
			else // ȸ���� ���� ����
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
		// Die ���¿����� �������� �ʴ´�.
		m_eCurState = m_eObjState;
		break;
	default:
		break;
	}
}

void CBot::PlayAnimation(void)
{
	//�÷��̴� ���� ���� ���·� �÷����Ѵ�. == m_eCurState
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
	m_iCurAniCount = 0;			//Curī��Ʈ�� 0���� �ʱ�ȭ��Ų��.
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