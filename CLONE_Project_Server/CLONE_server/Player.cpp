#include "stdafx.h"
#include "Player.h"

#include "GameMgr.h"
#include "NaviMgr.h"

CPlayer::CPlayer()
	: m_dwNowKey(0)
	, m_dwIdx(0)
	, m_fStamina(10.f)
	, m_fMaxStamina(20.f)
	, m_bRun(false)
	, m_bSit(false)
	, m_bAtt(false)
	, m_bJump(false)
	, m_bKeyDown(false)
	, m_bIsEndAnimation(false)

	, m_vScale(1.f, 1.f, 1.f)
{
	D3DXMatrixIdentity(&m_matWorld);
	m_vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_iID = UNKNOWN_VALUE;

	for (int i = 0; i < BEAD_END; ++i)
	{
		bIsGetBead[i] = false;
	}
}
CPlayer::CPlayer(D3DXVECTOR3 _pos, int _iID)
	: m_dwNowKey(0)
	, m_dwIdx(0)
	, m_fStamina(5.f)
	, m_fMaxStamina(10.f)
	, m_bRun(false)
	, m_bSit(false)
	, m_bAtt(false)
	, m_bJump(false)
	, m_bKeyDown(false)

	, m_vScale(1.f, 1.f, 1.f)
{
	D3DXMatrixIdentity(&m_matWorld);
	m_vPos = _pos;
	m_iID = _iID;

	for (int i = 0; i < BEAD_END; ++i)
	{
		bIsGetBead[i] = false;
	}
}
CPlayer::~CPlayer()
{
	ClearGetBead();
}

int CPlayer::Progress(float _fFrameTime)
{
	if (m_eObjState != OBJ_STATE_DIE 
		&& CGameMgr::GetInstance()->GetStageStateType() == STAGE_PLAY)
	{
		SetWorldMatrix();
		KeyCheck(_fFrameTime);
		StateCheck(_fFrameTime);
	}

	return 0;
}

void CPlayer::SetPlayerGetBead(int _iType, bool _bIsBeadSet)
{
	bIsGetBead[_iType] = _bIsBeadSet;
}
bool CPlayer::GetPlayerGetBead(int _iBeadType)
{
	return bIsGetBead[_iBeadType];
}
void CPlayer::ClearGetBead(void)
{
	for (int i = 0; i < BEAD_END; ++i)
	{
		bIsGetBead[i] = false;
	}
}

void CPlayer::SetWorldMatrix(void)
{
	D3DXMATRIX matScale, matRotX, matRotY, matRotZ, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationX(&matRotX, m_fAngle[ANGLE_X]);
	D3DXMatrixRotationY(&matRotY, m_fAngle[ANGLE_Y]);
	D3DXMatrixRotationZ(&matRotZ, m_fAngle[ANGLE_Z]);
	D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	D3DXVec3TransformNormal(&m_vDir, &g_vLook, &m_matWorld);
}
void CPlayer::KeyCheck(float _fFrameTime)
{
	// 좌클릭 -> 공격
	if (KEY_LMOUSE & m_dwNowKey)
	{
		if (!m_bAtt)
		{
			m_bAtt = true;
		}
		m_eObjState = OBJ_STATE_ATT;
	}

	// LSHIFT -> 질주
	if (KEY_LSHIFT & m_dwNowKey)
	{
		if (!m_bRun)
		{
			m_bRun = true;
		}

		if (m_bRun)	// 질주 상태일 시 스테미너 체크
		{
			m_fStamina -= _fFrameTime;	// 질주시 스테미너 소비

			if (m_fStamina <= 0.f)
			{
				m_fStamina = 0.f;
				m_bRun = false;
			}
		}
	}
	else
	{
		m_bRun = false;
	}

	// LCONTROL -> 웅크리기
	if (KEY_LCONTROL & m_dwNowKey)
	{
		if (!m_bSit)
		{
			m_bSit = true;
		}
		if (!m_bJump && !m_bAtt)
		{
			m_eObjState = OBJ_STATE_SIT;
		}
	}
	else
	{
		m_bSit = false;
	}

	// 이동
	if (KEY_W & m_dwNowKey)
	{
		if (!m_bRun)
		{
			//Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_vPos
				, &(m_vDir * 2.f * _fFrameTime), m_dwIdx, _fFrameTime);

			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_MOVE;
			}
		}
		else
		{
			//Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_vPos
				, &(m_vDir * 4.f * _fFrameTime), m_dwIdx, _fFrameTime);

			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_RUN;
			}
		}
		m_bKeyDown = true;
	}

	if (KEY_S & m_dwNowKey)
	{
		if (!m_bRun)
		{
			//Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_vPos
				, &(m_vDir * -2.f * _fFrameTime), m_dwIdx, _fFrameTime);

			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_MOVE;
			}
		}
		else
		{
			//Move On Navi
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_vPos
				, &(m_vDir * -4.f * _fFrameTime), m_dwIdx, _fFrameTime);

			if (!m_bJump && !m_bAtt)
			{
				m_eObjState = OBJ_STATE_RUN;
			}
		}
		m_bKeyDown = true;
	}

	// SPACE -> 점프 => 이동 뒤에 점프를 넣어 이동중에도 점프가능..
	if (KEY_SPACE & m_dwNowKey)
	{
		if (!m_bJump)
		{
			m_bJump = true;
			m_bAtt = false;
		}
		m_eObjState = OBJ_STATE_JUMP;
	}

	if (KEY_D & m_dwNowKey)
	{
		m_fAngle[ANGLE_Y] += D3DXToRadian(90.f * _fFrameTime);
	}

	if (KEY_A & m_dwNowKey)
	{
		m_fAngle[ANGLE_Y] -= D3DXToRadian(90.f * _fFrameTime);
	}
}
void CPlayer::StateCheck(float _fFrameTime)
{
	switch (m_eObjState)
	{
	case OBJ_STATE_MOVE:
		if (!m_bKeyDown)
		{
			m_eObjState = OBJ_STATE_IDEL;
		}
		if (AnimationEnd())
		{
			m_bIsEndAnimation = false;
			m_eObjState = OBJ_STATE_IDEL;
		}
		if (m_fStamina < m_fMaxStamina)	//대기 or 이동시 스테미너 회복
		{
			m_fStamina += _fFrameTime * 0.5f;
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
		if (AnimationEnd())
		{
			m_bIsEndAnimation = false;
			m_eObjState = OBJ_STATE_IDEL;
		}
		break;
	case OBJ_STATE_IDEL:
		if (AnimationEnd())
		{
			m_bIsEndAnimation = false;
			m_eObjState = OBJ_STATE_IDEL;
		}
		if (m_fStamina < m_fMaxStamina) //대기 or 이동시 스테미너 회복
		{
			m_fStamina += _fFrameTime * 0.5f;
			if (m_fStamina >= m_fMaxStamina)
			{
				m_fStamina = m_fMaxStamina;
			}
		}
		break;
	case OBJ_STATE_ATT:
		if (AnimationEnd())
		{
			m_bIsEndAnimation = false;
			m_eObjState = OBJ_STATE_IDEL;
			m_bAtt = false;
			m_bJump = false;
			m_bSit = false;
		}
		break;
	case OBJ_STATE_DIE:
		break;
	case OBJ_STATE_JUMP:
		if (AnimationEnd())
		{
			m_bIsEndAnimation = false;
			m_eObjState = OBJ_STATE_IDEL;
			m_bJump = false;
		}
		break;
	case OBJ_STATE_SIT:
		if (AnimationEnd() && m_bSit)
		{
			m_bIsEndAnimation = false;
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

	m_bKeyDown = false; //애니메이션 중복방지용 변수 초기화
}