#include "stdafx.h"
#include "Player.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
#include "TimeMgr.h"
#include "InputMgr.h"
#include "NaviMgr.h"
#include "CollisionMgr.h"
#include "ServerMgr.h"
#include "SceneMgr.h"
#include "RenderTargetMgr.h"
#include "SoundMgr.h"
//Object
#include "StaticMeshShader.h"
#include "DynamicMeshShader.h"
#include "DefaultShader.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "AniBuffer.h"
#include "Scene.h"
#include "Layer.h"
#include "RenderTarget.h"
#include "DepthDynamicShader.h"
#include "AllianceTag.h"
#include "ReflectionShader.h"
#include "RefactionForDynamicShader.h"
//server
#include "ServerMgr.h"
#include "../../CLONE_server/CLONE_server/Protocol.h"

CPlayer::CPlayer(CDevice * pDevice)
	: CObj(pDevice)	
	, m_dwIdx(0)
	, m_fStamina(10.f)
	, m_fMaxStamina(20.f)
	, m_fInitTime(0.f)
	, m_bCreate(FALSE)
	, m_bRun(FALSE)
	, m_bSit(FALSE)
	, m_bAtt(FALSE)
	, m_bJump(FALSE)
	, m_bKeyDown(FALSE)
	, m_pAllianceTag(NULL)

	, m_iID(UNKNOWN_VALUE)
	, m_bIsMe(false)
	, m_bIsMyTeam(false)
	, m_dwKey(0)
	, m_bIsFirstDie(false)
	, m_bFillStart(FALSE)
{
}
CPlayer::~CPlayer()
{
	Release();
}

HRESULT CPlayer::Init()
{
	HRESULT hr = NULL;

	FAILED_CHECK_RETURN_MSG(AddComponent(), E_FAIL, L"AddComponent Add Failed");

	m_eRenderType = RENDERTYPE_NONEALPHA;
	// 반사를 위함, 스테이지2는 플레이어가 물에 들어가있다. 
	if (g_GameInfo.bMapType == MAP_KJK_PLAZA)
		m_eObjRenderRefType = OBJ_RENDER_REF_UPPER;
	else
		m_eObjRenderRefType = OBJ_RENDER_REF_BOTH;

	//m_pInfo->m_vScale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
	//m_pInfo->m_vPos = D3DXVECTOR3(50.f, 0.f, 20.f);
	m_pInfo->m_fAngle[ANGLE_Y] = 0.f;

	m_eObjState		= OBJ_STATE_IDEL;
	m_eStageState	= STAGESTATE_READY;
	
	//Test 임의 팀 선택
	//m_eTeamType		= TEAM_RED;

	return S_OK;
}

int CPlayer::Update()
{
	// Collision Check 오류 방지용 --------------------------
	if (m_fInitTime < 1.f)
	{
		m_fInitTime += CTimeMgr::GetInstance()->GetTime();
	}
	else
	{
		m_bCreate = TRUE;
	}
	// ------------------------------------------------------

	if (m_eStageState != STAGESTATE_READY)
	{
		if (m_bIsMe)
		{
			InputKeyCheck();
		}
		//Enemy일 경우에 플레이어와 충돌체크
		else if (!m_bIsMe && m_bCreate)
		{
			CollisionAABB();	//Collision Check
		}
		KeyCheck();
	}

	//AllianceTag Update
	FAILED_CHECK_RETURN(Update_AllianceTag(), E_FAIL);

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	CObj::Update_Component();

	//알파값 랜더가 필요한 놈들만 쓴다
	//Compute_ViewZ(&m_pInfo->m_vPos);

	return 0;
}

void CPlayer::Render()
{
	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	//월드 XM매트릭스 생성
	XMFLOAT4X4 matWorld;
	memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);
	//셰이더 파라미터 세팅
	/*((CStaticMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pLightInfo, m_pTexture->GetResource());*/
	((CDynamicMeshShader*)m_pShader)->SetShaderParameter(matWorld, m_matView, m_matProj,
		m_pBuffer, m_pLightInfo, m_pTexture->GetResource());
	
	//m_pBuffer->Render(DXGI_32);	//StaticMesh
	PlayAnimation();				//DynamicMesh
	StateCheck();					//DynamicMesh	Play가 끝난 후 State 체크를해야된다ㅠㅠ


#ifdef _DEBUG	//디버그 모드일때만 바운딩박스 그리자 
	if (((CMesh*)m_pBuffer)->GetBoundingBoxBuffer() != NULL)
	{
 		((CDefaultShader*)m_pBoundingBoxShader)->SetShaderParameter(matWorld, m_matView, m_matProj, ((CMesh*)m_pBuffer)->GetBoundingBoxBuffer(), m_bCollision);

		((CMesh*)m_pBuffer)->GetBoundingBoxBuffer()->Render(DXGI_16);
	}
#endif
}

void CPlayer::RenderForShadow(LightMatrixStruct tLight)
{
	if (m_bFillStart)
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

void CPlayer::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
	XMFLOAT4X4 matWorld;

	const CComponent*	pTrapWaterInfo = CSceneMgr::GetInstance()->GetScene()->GetComponent(CLayer::LAYERTYPE_GAMELOGIC, L"TrapWater", L"Transform");

	if (pTrapWaterInfo == NULL)	//이게 NULL 이면 STAGE 1 인것임
		return;

	//카메라에서 행렬 뽑아옴
	SetViewProjTransform();

	if (eRefType == CObj::REFTYPE_REFRACT)
	{
		if (((CTransform*)pTrapWaterInfo)->m_vPos.y > 0.f)
		{
			if (!m_bFillStart)
				m_bFillStart = TRUE;

			//월드 XM매트릭스 생성
			memcpy(&matWorld, m_pInfo->m_matWorld, sizeof(float) * 16);

			//셰이더 파라미터 세팅
			D3DXVECTOR4 vClipPlaneForTrapWater = D3DXVECTOR4(0.f, -1.f, 0.f, ((CTransform*)pTrapWaterInfo)->m_vPos.y + 0.1f);

			((CRefactionForDynamicShader*)m_pRefractionShader)->SetShaderParameter(matWorld, m_matView, m_matProj, m_pBuffer,
				CLightMgr::GetInstance()->GetLightInfo(), m_pTexture->GetResource(), vClipPlaneForTrapWater);

			((CDynamicMesh*)m_pBuffer)->RenderMesh(m_eObjState);
		}
	}
	else if (eRefType == CObj::REFTYPE_REFLECT)
	{

	}
}

HRESULT CPlayer::AddComponent(void)
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

	//LightInfom 
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

	//RefactionShader && ReflectionShader
	m_pRefractionShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_RefactionForDynamic"));
	m_pReflectionShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Reflection"));
	

	return S_OK;
}

HRESULT CPlayer::Update_AllianceTag(void)
{
	//내가 아니다(개인전 전용)
	if (!m_bIsMe && !m_bIsMyTeam)
	{
		return S_OK;
	}

	//AllianceTag Update
	if (m_pAllianceTag == NULL)
	{
		m_pAllianceTag = CAllianceTag::Create(m_pDevice);
		m_pAllianceTag->SetCam(m_pCam);
		CSceneMgr::GetInstance()->Add_RenderGroup(RENDERTYPE_UI, m_pAllianceTag, CSceneMgr::ADD_GROUP_FRONT);
	}

	D3DXVECTOR3 vPos = m_pInfo->m_vPos;
	vPos.y += 2.5f;

	m_pAllianceTag->SetPos(vPos);

	m_pAllianceTag->Update();

	return S_OK;
}

CPlayer * CPlayer::Create(CDevice * pDevice, OBJTYPE eType)
{
	CPlayer*	pPlayer = new CPlayer(pDevice);

	if (FAILED(pPlayer->Init()))
		Safe_Delete(pPlayer);

	pPlayer->SetObjType(eType);

	return pPlayer;
}

void CPlayer::Release()
{
	Safe_Delete(m_pAllianceTag);
	Safe_Release(m_pShader);
	Safe_Release(m_pBoundingBoxShader);
	Safe_Release(m_pDepthShader);
	Safe_Release(m_pReflectionShader);
	Safe_Release(m_pRefractionShader);
}

void CPlayer::KeyCheck(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();

	// 좌클릭 -> 공격
	if (KEY_LMOUSE & m_dwKey)
	{
		if (!m_bAtt)
		{
#ifdef _PLAY_SOUND
			CSoundMgr::GetInstance()->PlayPlayerSound(L"hit_ES.mp3");
#endif
			m_bAtt = TRUE;
		}
		m_eObjState = OBJ_STATE_ATT;
	}

	// LSHIFT -> 질주
	if (KEY_LSHIFT & m_dwKey)
	{
		if (!m_bRun)
		{
			m_bRun = TRUE;
		}
		
		if(m_bRun)	// 질주 상태일 시 스테미너 체크
		{
			m_fStamina -= fTime;	// 질주시 스테미너 소비
			
			if (m_fStamina <= 0.f)
			{
				m_fStamina = 0.f;
				m_bRun = FALSE;
			}
		}
	}
	else
	{
		m_bRun = FALSE;
	}

	// LCONTROL -> 웅크리기
	if (KEY_LCONTROL & m_dwKey)
	{
		if (!m_bSit)
		{
			m_bSit = TRUE;
		}
		if (!m_bJump && !m_bAtt)
		{
			m_eObjState = OBJ_STATE_SIT;
		}
	}
	else
	{
		m_bSit = FALSE;
	}
	
	// 이동
	if (KEY_W & m_dwKey)
	{
		if (!m_bRun)
		{
			////Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos
				, &(m_pInfo->m_vDir * 2.f * fTime), m_dwIdx, (eMapType)g_GameInfo.bMapType);

			//m_pInfo->m_vPos += m_pInfo->m_vDir * 2.f * fTime;
			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_MOVE;
			}
		}
		else
		{
			////Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos
				, &(m_pInfo->m_vDir * 4.f * fTime), m_dwIdx, (eMapType)g_GameInfo.bMapType);

			//m_pInfo->m_vPos += m_pInfo->m_vDir * 4.f * fTime;
			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_RUN;
			}
		}
		m_bKeyDown = TRUE;
	}

	if (KEY_S & m_dwKey)
	{
		if (!m_bRun)
		{
			////Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos
				, &(m_pInfo->m_vDir * -2.f * fTime), m_dwIdx, (eMapType)g_GameInfo.bMapType);

			//m_pInfo->m_vPos -= m_pInfo->m_vDir * 2.f * fTime;
			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_MOVE;
			}
		}
		else
		{
			////Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos
				, &(m_pInfo->m_vDir * -4.f * fTime), m_dwIdx, (eMapType)g_GameInfo.bMapType);

			//m_pInfo->m_vPos -= m_pInfo->m_vDir * 4.f * fTime;
			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_RUN;
			}
		}
		m_bKeyDown = TRUE;
	}

	// SPACE -> 점프 => 이동 뒤에 점프를 넣어 이동중에도 점프가능..
	if (KEY_SPACE & m_dwKey)
	{
		if (!m_bJump)
		{
#ifdef _PLAY_SOUND
			CSoundMgr::GetInstance()->PlayPlayerSound(L"jump_ES.wav");
#endif
			m_bJump = TRUE;
			m_bAtt = FALSE;
		}
		m_eObjState = OBJ_STATE_JUMP;
	}

	if (KEY_D & m_dwKey)
	{
		m_pInfo->m_fAngle[ANGLE_Y] += D3DXToRadian(90.f * fTime);
	}

	if (KEY_A & m_dwKey)
	{
		m_pInfo->m_fAngle[ANGLE_Y] -= D3DXToRadian(90.f * fTime);
	}
}

void CPlayer::StateCheck(void)
{
	switch (m_eObjState)
	{
	case OBJ_STATE_MOVE:
		if (!m_bKeyDown)
		{
			m_eObjState = OBJ_STATE_IDEL;
		}
		if (((CDynamicMesh*)m_pBuffer)->Animend)
		{
			SendAnimationEnd(OBJ_STATE_MOVE);
			m_eObjState = OBJ_STATE_IDEL;
		}
		if (m_fStamina < m_fMaxStamina)	//대기 or 이동시 스테미너 회복
		{
			m_fStamina += CTimeMgr::GetInstance()->GetTime() * 0.5f;
			if (m_fStamina >= m_fMaxStamina)
			{
				m_fStamina = m_fMaxStamina;
			}
		}
		break;
	case OBJ_STATE_RUN:
		if (!m_bKeyDown)
		{
			m_eObjState = OBJ_STATE_IDEL;
		}
		if (((CDynamicMesh*)m_pBuffer)->Animend)
		{
			SendAnimationEnd(OBJ_STATE_RUN);
			m_eObjState = OBJ_STATE_IDEL;
		}
		break;
	case OBJ_STATE_IDEL:
		if (((CDynamicMesh*)m_pBuffer)->Animend)
		{
			SendAnimationEnd(OBJ_STATE_IDEL);
			m_eObjState = OBJ_STATE_IDEL;
		}
		if (m_fStamina < m_fMaxStamina) //대기 or 이동시 스테미너 회복
		{
			m_fStamina += CTimeMgr::GetInstance()->GetTime() * 0.5f;
			if (m_fStamina >= m_fMaxStamina)
			{
				m_fStamina = m_fMaxStamina;
			}
		}
		break;
	case OBJ_STATE_ATT:
		if (((CDynamicMesh*)m_pBuffer)->Animend)
		{
			SendAnimationEnd(OBJ_STATE_ATT);
			m_eObjState = OBJ_STATE_IDEL;
			m_bAtt = FALSE;
			m_bJump = FALSE;
			m_bSit = FALSE;
		}
		break;
	case OBJ_STATE_DIE:
		break;
	case OBJ_STATE_JUMP:
		if (((CDynamicMesh*)m_pBuffer)->Animend)
		{
			SendAnimationEnd(OBJ_STATE_JUMP);
			m_eObjState = OBJ_STATE_IDEL;
			m_bJump = FALSE;
		}
		break;
	case OBJ_STATE_SIT:
		if (((CDynamicMesh*)m_pBuffer)->Animend && m_bSit)
		{
			SendAnimationEnd(OBJ_STATE_SIT);
			m_eObjState = OBJ_STATE_SIT;
		}
		else if (!m_bSit)
		{
			m_eObjState = OBJ_STATE_IDEL;
		}
		break;
	default:
		break;
	}

	m_bKeyDown = FALSE; //애니메이션 중복방지용 변수 초기화
}

void CPlayer::PlayAnimation(void)
{
	switch (m_eObjState)
	{
	case OBJ_STATE_MOVE:
		((CDynamicMesh*)m_pBuffer)->PlayPlayerAnimation(OBJ_STATE_MOVE);
		break;
	case OBJ_STATE_RUN:
		((CDynamicMesh*)m_pBuffer)->PlayPlayerAnimation(OBJ_STATE_RUN);
		break;
	case OBJ_STATE_IDEL:
		((CDynamicMesh*)m_pBuffer)->PlayPlayerAnimation(OBJ_STATE_IDEL);
		break;
	case OBJ_STATE_ATT:
		((CDynamicMesh*)m_pBuffer)->PlayPlayerAnimation(OBJ_STATE_ATT);
		break;
	case OBJ_STATE_DIE:
		((CDynamicMesh*)m_pBuffer)->PlayPlayerAnimation(OBJ_STATE_DIE);
		break;
	case OBJ_STATE_JUMP:
		((CDynamicMesh*)m_pBuffer)->PlayPlayerAnimation(OBJ_STATE_JUMP);
		break;
	case OBJ_STATE_SIT:
		((CDynamicMesh*)m_pBuffer)->PlayPlayerAnimation(OBJ_STATE_SIT);
		break;
	case OBJ_STATE_END:
		break;
	}
}

void CPlayer::SendAnimationEnd(CObj::OBJSTATE _objstate)
{
	CS_AnimationEnd tPacket;
	CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
}

void CPlayer::InputKeyCheck(void)
{
	DWORD dwBeforeKeyValue = m_dwKey;
	m_dwKey = 0;

	//죽었을경우에는 키를 입력받지 않는다
	if (m_eObjState == OBJ_STATE_DIE)
	{
		if (!m_bIsFirstDie)
		{
			m_bIsFirstDie = true;

			if (m_dwKey != dwBeforeKeyValue)
			{
				CS_InputKey tPacket;
				tPacket.m_dwKey = m_dwKey;
				CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
			}
		}
		return;
	}

	if (CInput::GetInstance()->GetDIMouseState(CInput::MOUSE_TYPE_L) & 0x80)
	{
		m_dwKey |= KEY_LMOUSE;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_LSHIFT) & 0x80)
	{
		m_dwKey |= KEY_LSHIFT;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_LCONTROL) & 0x80)
	{
		m_dwKey |= KEY_LCONTROL;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_W) & 0x80)
	{
		m_dwKey |= KEY_W;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_S) & 0x80)
	{
		m_dwKey |= KEY_S;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_SPACE) & 0x80)
	{
		m_dwKey |= KEY_SPACE;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_D) & 0x80)
	{
		m_dwKey |= KEY_D;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_A) & 0x80)
	{
		m_dwKey |= KEY_A;
	}


	if (m_dwKey != dwBeforeKeyValue)
	{
		CS_InputKey tPacket;
		tPacket.m_dwKey = m_dwKey;
		CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
	}
}

void CPlayer::CollisionAABB(void)
{
	if (m_eObjState == OBJ_STATE_DIE)
		return;

	if (CCollisionMgr::GetInstance()->Collision_AABB(L"Player", this))
	{
		m_bCollision = TRUE;

		CS_CollisionPlayerPlayer tPacket;
		tPacket.m_iMyID = CServerMgr::GetInstance()->GetClientID();
		tPacket.m_iTargetPlayerID = m_iID;
		CServerMgr::GetInstance()->SendPacket(reinterpret_cast<char*>(&tPacket));
	}
	else
		m_bCollision = FALSE;
}