#include "stdafx.h"
#include "Bot.h"

#include "NaviMgr.h"
#include "GameMgr.h"
#include "ObjMgr.h"
#include "PacketMgr.h"

CBot::CBot()
	: m_bRotateEnd(true)
	, m_bInitState(false)
	, m_dwIdx(0)

	, m_eCurState(OBJ_STATE_IDEL)
	, m_fAccrueTime(0.f)
	, m_iCurAniCount(0)
	, m_iSaveAniCount(0)
{
	m_iID = UNKNOWN_VALUE;
	m_vPos = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_fAngle[ANGLE_Y] = 0.f;
}
CBot::CBot(D3DXVECTOR3 _pos, int _iID)
	: m_bRotateEnd(true)
	, m_bInitState(false)
	, m_dwIdx(0)

	, m_eCurState(OBJ_STATE_IDEL)
	, m_fAccrueTime(0.f)
	, m_iCurAniCount(0)
	, m_iSaveAniCount(0)
{
	m_iID = _iID;
	m_vPos = _pos;

	m_fAngle[ANGLE_Y] = 0.f;
}
CBot::~CBot()
{
}

int CBot::Progress(float _fFrameTime)
{
	if (m_eObjState != OBJ_STATE_DIE)
	{
		if (m_iSaveAniCount < m_iCurAniCount)
		{
			//end moving -> OBJ_STATE_IDEL
			g_GlobalMutex.lock();
			SetState(OBJ_STATE_IDEL, 20, m_fAngle[ANGLE_Y]);
			g_GlobalMutex.unlock();

			//packet setting
			m_tChangeStateBot.m_iBotID = m_iID;
			m_tChangeStateBot.m_iBotState = OBJ_STATE_IDEL;
			m_tChangeStateBot.m_iBotTimeCount = 20;
			m_tChangeStateBot.m_fAngleY = m_fAngle[ANGLE_Y];
			m_tChangeStateBot.m_vBotPos = m_vPos;

			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CPacketMgr::GetInstance()->SendPacket(iter->first, &m_tChangeStateBot);
			}
		}

		if (CGameMgr::GetInstance()->GetStageStateType() == STAGE_PLAY)
		{
			m_fAccrueTime += _fFrameTime;
		}

		CheckAni(_fFrameTime);
		CheckState(_fFrameTime );
	}

	return 0;
}

void CBot::CheckAni(float _fFrameTime)
{
	if (m_fAccrueTime > 1.f)
	{
		m_fAccrueTime = 0.f;
		++m_iCurAniCount;
	}
}
void CBot::CheckState(float _fFrameTime)
{
	D3DXMATRIX	matRotate;
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
			g_GlobalMutex.lock();
			m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_vPos, &(vDir * _fFrameTime), m_dwIdx, _fFrameTime, m_eMapType);
			g_GlobalMutex.unlock();

			m_fAngle[ANGLE_Y] = m_fRadious;

			m_eCurState = m_eObjState;

			m_bRotateEnd = true;
			m_bInitState = true;
		}
		else
		{
			// 상태가 이동이나 회전이 끝나지 않았다.
			if (!m_bRotateEnd)
			{
				float fIncludeAngle = m_fAngle[ANGLE_Y] - m_fRadious;

				D3DXMatrixIdentity(&matRotate);

				//현재 각이 서버로부터 받은 각보다 작다 == 플레이어의 회전속도와 같게 회전
				if (m_fAngle[ANGLE_Y] < m_fRadious)
					m_fAngle[ANGLE_Y] += (float)D3DXToRadian(90.f * _fFrameTime);
				else //현재 각이 서버로부터 받은 각보다 크다 == 플레이어의 회전속도와 같게 회전
					m_fAngle[ANGLE_Y] -= (float)D3DXToRadian(90.f * _fFrameTime);

				// 회전이 끝나지 않았다면 IDEL 상태를 넣어준다.
				m_eCurState = OBJ_STATE_IDEL;

				// 서버로 부터 받은 각과의 차이가 같다.
				if (abs(fIncludeAngle) < 0.1f)
				{
					m_eCurState = m_eObjState;	// 회전이 끝난 후 현재 상태에 넣어준다.
					m_fAngle[ANGLE_Y] = m_fRadious;	// 회전이 끝났으니 각을 고정해준다.
					m_bRotateEnd = true;
				}
			}
			else // 회전이 끝난 상태
			{
				D3DXMatrixIdentity(&matRotate);
				D3DXMatrixRotationY(&matRotate, m_fAngle[ANGLE_Y]);

				D3DXVec3TransformNormal(&vDir, &vDir, &matRotate);

				//Move On Navi
				g_GlobalMutex.lock();
				m_dwIdx = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_vPos, &(vDir * _fFrameTime), m_dwIdx, _fFrameTime, m_eMapType);
				g_GlobalMutex.unlock();
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
void CBot::SetState(eObjState _eState, int _iCount/* = 0*/, float _fRadious/* = 0.f*/)
{
	m_fAccrueTime = 0.f;
	m_iCurAniCount = 0;
	m_iSaveAniCount = _iCount;
	m_fRadious = _fRadious;
	m_eObjState = _eState;
	m_bRotateEnd = false;
}