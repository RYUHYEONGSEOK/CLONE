#include "stdafx.h"
#include "DynamicCam.h"
//Manager
#include "InputMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
#include "NaviMgr.h"
//Object
#include "Scene.h"

CDynamicCamera::CDynamicCamera(CDevice* pDevice)
	: CCamera(pDevice)
	, m_fCamSpeed(0.f)
	, m_fCamRotY(0.f)
	, m_fCamY(0.f)
	, m_fTargetDistance(0.f)
	, m_bFixKeyDown(FALSE)
	, m_bMouseFix(TRUE)
	, m_bStaticCamKeyDown(FALSE)
	, m_bStaticCam(FALSE)
	, m_bRClick(FALSE)
	, m_bMouseShow(FALSE)
	, m_bInitMove(FALSE)
{

}

CDynamicCamera::~CDynamicCamera(void)
{
	Release();
}

HRESULT CDynamicCamera::Init()
{
	return S_OK;
}

HRESULT CDynamicCamera::InitCamera(const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt)
{
	m_fCamSpeed = 15.f;
	m_fTargetDistance = 5.f;

	m_vEye = *pEye;
	m_vAt = *pAt;
	Invalidate_View();

	m_fFovY = (float)(D3DXToRadian(45.f));
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = 1.f;
	m_fFar = 200.f;
	Invalidate_Proj();
	return S_OK;
}

void CDynamicCamera::KeyCheck(void)
{
	float		fTime = CTimeMgr::GetInstance()->GetTime();

	// 마우스 픽스
	if (CInput::GetInstance()->GetDIKeyState(DIK_Q) & 0x80)
	{
		if (m_bFixKeyDown)
			return;

		m_bFixKeyDown = TRUE;

		if (m_bMouseFix == TRUE)
			m_bMouseFix = FALSE;
		else
			m_bMouseFix = TRUE;
	}
	else
		m_bFixKeyDown = FALSE;

	// 카메라 회전 사용 여부 
	if (CInput::GetInstance()->GetDIMouseState(CInput::MOUSE_TYPE_R))
	{
		if (!m_bRClick)
			m_bRClick = TRUE;
	}
	else
		m_bRClick = FALSE;

	// 1, 3인칭 카메라 전환
	if (CInput::GetInstance()->GetDIKeyState(DIK_P) & 0x80)
	{
		if (!m_bStaticCamKeyDown)
		{
			m_bStaticCamKeyDown = TRUE;
			if (!m_bStaticCam)
			{
				m_bStaticCam = TRUE;
				// Static Cam이 되는 순간 Eye의 인덱스를 찾아준다.
				m_dwNaviIdx = CNaviMgr::GetInstance()->GetNaviIndex(&m_vEye, m_dwNaviIdx, (eMapType)g_GameInfo.bMapType);
			}
			else
				m_bStaticCam = FALSE;
		}
	}
	else
		m_bStaticCamKeyDown = FALSE;

	if (m_bMouseFix == FALSE)
		return;

	// Static Cam이 FALSE일때만 방향키 작동
	if (m_bStaticCam == FALSE)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
		{
			D3DXVECTOR3		vRight;
			D3DXMATRIX		matCamState;

			D3DXMatrixInverse(&matCamState, NULL, &m_matView);
			memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
			D3DXVec3Normalize(&vRight, &vRight);

			m_vEye -= vRight * m_fCamSpeed * fTime;
			m_vAt -= vRight * m_fCamSpeed * fTime;
		}

		if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
		{
			D3DXVECTOR3		vRight;
			D3DXMATRIX		matCamState;

			D3DXMatrixInverse(&matCamState, NULL, &m_matView);
			memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
			D3DXVec3Normalize(&vRight, &vRight);

			m_vEye += vRight * m_fCamSpeed * fTime;
			m_vAt += vRight * m_fCamSpeed * fTime;
		}

		if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
		{
			D3DXVECTOR3		vLook;
			vLook = m_vAt - m_vEye;
			D3DXVec3Normalize(&vLook, &vLook);

			m_vEye += vLook * m_fCamSpeed * fTime;
			m_vAt += vLook * m_fCamSpeed * fTime;
		}

		if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
		{
			D3DXVECTOR3		vLook;
			vLook = m_vAt - m_vEye;
			D3DXVec3Normalize(&vLook, &vLook);

			m_vEye -= vLook * m_fCamSpeed * fTime;
			m_vAt -= vLook * m_fCamSpeed * fTime;
		}
	}
}

int CDynamicCamera::Update(void)
{
	KeyCheck();

	if (m_bMouseFix)
	{
		FixMouse();
		if (!m_bStaticCam)
			MouseMove();
		else
			CamMove();
	}
	else
	{
		if (m_bMouseShow)
		{
			ShowCursor(TRUE);	//이 함수는 1번씩만 호출된다.
			m_bMouseShow = FALSE;
		}
	}

	if (!m_bInitMove)
	{
		InitMove();
	}

	CCamera::Update();

	return 0;
}

void CDynamicCamera::Render()
{
}

void CDynamicCamera::RenderForShadow(LightMatrixStruct tLight)
{
}

void CDynamicCamera::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}

void CDynamicCamera::Release()
{
	ShowCursor(TRUE);
}

CDynamicCamera* CDynamicCamera::Create(CDevice*	pDevice
	, const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt)
{
	CDynamicCamera*		pCamera = new CDynamicCamera(pDevice);
	
	if (FAILED(pCamera->InitCamera(pEye, pAt)))
		Safe_Delete(pCamera);

	return pCamera;
}

void CDynamicCamera::FixMouse(void)
{
	POINT		ptMouse = { WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

	if (!m_bMouseShow)
	{
		ShowCursor(FALSE);	// 이 함수는 1번씩만 호출된다.
		m_bMouseShow = TRUE;
	}
}

void CDynamicCamera::MouseMove(void)
{
	int		iDistance = 0;

	if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::MOUSE_MOVE_X))
	{
		D3DXMATRIX		matAxis;
		D3DXMatrixRotationAxis(&matAxis, &D3DXVECTOR3(0.f, 1.f, 0.f), (float)(D3DXToRadian(iDistance / 10.f)));

		D3DXVECTOR3		vDir;
		vDir = m_vAt - m_vEye;
		D3DXVec3TransformNormal(&vDir, &vDir, &matAxis);

		m_vAt = m_vEye + vDir;
	}

	if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::MOUSE_MOVE_Y))
	{
		D3DXVECTOR3		vRight;
		D3DXMATRIX		matCamState;

		D3DXMatrixInverse(&matCamState, NULL, &m_matView);
		memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
		D3DXVec3Normalize(&vRight, &vRight);

		D3DXMATRIX		matAxis;
		D3DXMatrixRotationAxis(&matAxis, &vRight, (float)(D3DXToRadian(iDistance / 10.f)));

		D3DXVECTOR3		vDir;
		vDir = m_vAt - m_vEye;
		D3DXVec3TransformNormal(&vDir, &vDir, &matAxis);

		m_vAt = m_vEye + vDir;
	}
}

void CDynamicCamera::CamMove(void)
{
	int		iDistance = 0;
	int		iPlayerCamDist = 0;
	const CComponent*	pPlayerInfo = CSceneMgr::GetInstance()->GetScene()->GetComponent(CLayer::LAYERTYPE_GAMELOGIC, L"Player", L"Transform");

	D3DXVECTOR3	 vPlayerDir = ((CTransform*)pPlayerInfo)->m_vDir;
	D3DXMATRIX matRotY;
	D3DXMatrixIdentity(&matRotY);
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(m_fCamRotY / 3.f));
	D3DXVec3TransformCoord(&vPlayerDir, &vPlayerDir, &matRotY);
	D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);

	D3DXVECTOR3	 vCamDir = vPlayerDir * (-1);
	m_vEye = vCamDir;
	D3DXVec3Normalize(&m_vEye, &m_vEye);

	// R클릭했을 시 => 마우스 좌, 우 시점 이동
	if (m_bRClick)
	{
		if (iPlayerCamDist = CInput::GetInstance()->GetDIMouseMove(CInput::MOUSE_MOVE_X))
		{
			if (iPlayerCamDist > 0)
				m_fCamRotY += 600.f * CTimeMgr::GetInstance()->GetTime();
			else
				m_fCamRotY -= 600.f * CTimeMgr::GetInstance()->GetTime();
		}
	}
	else	//R클릭 떼면 초기화
		m_fCamRotY = 0.f;

	// 마우스 휠버튼 카메라 거리 조정
	if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::MOUSE_MOVE_Z))
	{
		if (iDistance > 0)
			m_fTargetDistance -= 300.f * CTimeMgr::GetInstance()->GetTime();
		else
			m_fTargetDistance += 300.f * CTimeMgr::GetInstance()->GetTime();

		if (m_fTargetDistance > 15.f)
			m_fTargetDistance = 15.f;
		if (m_fTargetDistance < 1.f)
			m_fTargetDistance = 1.f;
	}

	// 마우스 상, 하 시점 이동
	if (iDistance = CInput::GetInstance()->GetDIMouseMove(CInput::MOUSE_MOVE_Y))
	{
		if (iDistance > 0)
			m_fCamY += 15.f * CTimeMgr::GetInstance()->GetTime();
		else
			m_fCamY -= 10.f * CTimeMgr::GetInstance()->GetTime();

		//최대로 내릴수 있는 각 고정
		if (m_fCamY < 0.1f)
			m_fCamY = 0.1f;
		//최대로 멀리 볼 수 있는 각 고정
		if (m_fCamY > 3.5f)
			m_fCamY = 3.5f;
	}

	//여기서 타겟 Dist를 구한다...
	m_vEye *= m_fTargetDistance;

	D3DXVECTOR3		vRight;
	memcpy(&vRight, &((CTransform*)pPlayerInfo)->m_matWorld.m[0][0], sizeof(float) * 3);

	D3DXMATRIX	matRotAxis;
	D3DXMatrixRotationAxis(&matRotAxis, &vRight, D3DXToRadian(2.f/*((Engine::CTransform*)pPlayerInfo)->m_fAngle[Engine::ANGLE_Y]*/));
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRotAxis);

	D3DXVECTOR3 vTargetPos = ((CTransform*)pPlayerInfo)->m_vPos;
	m_vEye += D3DXVECTOR3(vTargetPos.x, vTargetPos.y + m_fCamY + 2.f, vTargetPos.z);
	m_vAt = D3DXVECTOR3(vTargetPos.x, vTargetPos.y + 2.f, vTargetPos.z);

	// 여기서 벽뚫는것 처리한다.
	m_dwNaviIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh_Cam(&m_vEye, &m_vAt, &vCamDir, m_fTargetDistance, m_fCamY, m_dwNaviIdx, (eMapType)g_GameInfo.bMapType);
}

void CDynamicCamera::InitMove(void)
{
	//아니면 START로 바뀌었을떄
	if (/*m_vEye.z > HALFMAPZ * 2.f || */m_eStageState == STAGESTATE_PLAY)
	{
		m_bInitMove = TRUE;

		if (!m_bStaticCamKeyDown)
		{
			m_bStaticCamKeyDown = TRUE;
			if (!m_bStaticCam)
			{
				m_bStaticCam = TRUE;
				// Static Cam이 되는 순간 Eye의 인덱스를 찾아준다.
				m_dwNaviIdx = CNaviMgr::GetInstance()->GetNaviIndex(&m_vEye, m_dwNaviIdx, (eMapType)g_GameInfo.bMapType);
			}
			else
				m_bStaticCam = FALSE;
		}
	}

	float fTime = CTimeMgr::GetInstance()->GetTime();

	D3DXVECTOR3		vLook;
	vLook = m_vAt - m_vEye;
	D3DXVec3Normalize(&vLook, &vLook);

	m_vEye -= vLook * m_fCamSpeed * fTime * 0.5f;
	m_vAt -= vLook * m_fCamSpeed * fTime * 0.5f;
}

