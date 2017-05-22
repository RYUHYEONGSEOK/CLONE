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
	// �̰� ��� �������� ������ �޾ƿ´�.
	float fTestRadious = D3DXToRadian((float)(rand() % 360));
	int iTestCount = rand() % 5 + 2;
	ObjState eTestState = (ObjState)(rand() % 2);
	if (eTestState == OBJ_STATE_RUN)
		eTestState = OBJ_STATE_IDEL;
	// ----------------------------------------------------

	//Test ���� ������
	m_eTeamType = TEAM_BLUE;

	// �ʱ��ൿ ����
	StateSetting(eTestState, iTestCount, fTestRadious);

	return S_OK;
}

int CEnemy::Update()
{
	// Collision Check ���� ������ --------------------------
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


	// Save�ִ� ī��Ʈ���� Cur�ִ� ī��Ʈ�� Ŀ���� == �����κ��� �ٽ� �Է¹��� ����
	if (m_iSaveAniCount < m_iCurAniCount)
	{
		//Test Check �̰� ��� �������� ������ �޾ƿ´�.
		float fTestRadious = D3DXToRadian((float)(rand() % 360));
		int iTestCount = rand() % 5 + 2;	// �̰� �����ؾߴ�!
		ObjState eTestState = (ObjState)(rand() % 2);
		if (eTestState == OBJ_STATE_RUN)
			eTestState = OBJ_STATE_IDEL;

		// �����ൿ ����
		StateSetting(eTestState, iTestCount, fTestRadious);
	}

	// �ൿ üũ ���� �Լ���
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
	if (CFrustum::GetInstance()->SphereInFrustum(&m_pInfo->m_vPos, 2.f) == FALSE)	//Radious �� Unit�� Height��ŭ
		return;

	//ī�޶󿡼� ��� �̾ƿ�
	SetViewProjTransform();

	//���� XM��Ʈ���� ����
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//���̴� �Ķ���� ����
	((CDynamicMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());

	//m_pBuffer->Render(DXGI_32);	//Static Mesh
	PlayAnimation();				//DynamicMesh

#ifdef _DEBUG	//����� ����϶��� �ٿ���ڽ� �׸��� 
	if (((CMesh*)m_pBuffer)->GetBoundingBoxBuffer() != NULL)
	{
		((CDefaultShader*)m_pBoundingBoxShader)->SetShaderParameter(matWorld, m_matView, m_matProj, ((CMesh*)m_pBuffer)->GetBoundingBoxBuffer(), m_bCollision);

		((CMesh*)m_pBuffer)->GetBoundingBoxBuffer()->Render(DXGI_16);
	}
#endif
}

void CEnemy::RenderForShadow(LightMatrixStruct tLight)
{
	//���� XM��Ʈ���� ����
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

	memcpy(&m_matView, tLight.LightView, sizeof(float) * 16);
	memcpy(&m_matProj, tLight.LightProj, sizeof(float) * 16);

	//���̴� �Ķ���� ����
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
		// idel ���¿����� �������� �ʴ´�
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
		// ���Ŀ� ����
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_IDEL);
		break;
	case OBJ_STATE_MOVE:
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_MOVE);
		break;
	case OBJ_STATE_DIE:
		// ���Ŀ� ����
		((CDynamicMesh*)m_pBuffer)->PlayBotAnimation(OBJ_STATE_DIE);
		break;
	default:
		break;
	}
}

void CEnemy::StateSetting(ObjState eState, int iCount, float fRadious)
{
	m_iCurAniCount = 0;			//Curī��Ʈ�� 0���� �ʱ�ȭ��Ų��.
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
