#include "stdafx.h"
#include "GameMgr.h"

#include "ClientMgr.h"
#include "TimerQMgr.h"
#include "ObjMgr.h"

#include "Protocol.h"

IMPLEMENT_SINGLETON(CGameMgr)

CGameMgr::CGameMgr()
	: m_iHostID(UNKNOWN_VALUE)
	, m_iRoundCount(1)
	, m_iBotCount(20)
{
	InitializeCriticalSection(&m_GameMgrCS);

	m_eGameType = GAME_END;
	m_eGameModeType = MODE_END;
	m_eStageType = STAGE_READY;

	for (int i = 0; i < MAX_USER; ++i)
	{
		m_iRoomIndexID[i] = UNKNOWN_VALUE;
	}
}
CGameMgr::~CGameMgr()
{
	Release();

	DeleteCriticalSection(&m_GameMgrCS);
}

bool CGameMgr::Initialize(void)
{
	/*
	Don't use Critical Section
	Because This Part is Initialized part
	*/

	m_iRoundCount = 1;
	m_iBotCount = 20;

	m_eGameType = GAME_ROOM;
	m_eGameModeType = MODE_SOLO;

	return true;
}
bool CGameMgr::Release(void)
{
	/*
	Don't use Critical Section
	Because This Part is Released part
	*/

	for (int i = 0; i < MAX_USER; ++i)
	{
		m_iRoomIndexID[i] = UNKNOWN_VALUE;
	}
	return true;
}

bool CGameMgr::SetHostID(int _iID)
{
	EnterCriticalSection(&m_GameMgrCS);
	m_iHostID = _iID;
	LeaveCriticalSection(&m_GameMgrCS);

	return true;
}
bool CGameMgr::SetRoundCount(bool _bUpAndDown, int _iValue)
{
	//up
	if (_bUpAndDown)
	{
		EnterCriticalSection(&m_GameMgrCS);
		m_iRoundCount += _iValue;
		if (m_iRoundCount > MAX_ROUND_COUNT)
		{
			m_iRoundCount = MAX_ROUND_COUNT;
		}
		LeaveCriticalSection(&m_GameMgrCS);
	}
	//down
	else
	{
		EnterCriticalSection(&m_GameMgrCS);
		m_iRoundCount -= _iValue;
		if (m_iRoundCount < MIN_ROUND_COUNT)
		{
			m_iRoundCount = MIN_ROUND_COUNT;
		}
		LeaveCriticalSection(&m_GameMgrCS);
	}
	return true;
}
bool CGameMgr::SetBotCount(bool _bUpAndDown, int _iValue)
{
	//up
	if (_bUpAndDown)
	{
		EnterCriticalSection(&m_GameMgrCS);
		m_iBotCount += _iValue;
		if (m_iBotCount > MAX_BOT_COUNT)
		{
			m_iBotCount = MAX_BOT_COUNT;
		}
		LeaveCriticalSection(&m_GameMgrCS);
	}
	//down
	else
	{
		EnterCriticalSection(&m_GameMgrCS);
		m_iBotCount -= _iValue;
		if (m_iBotCount < MIN_BOT_COUNT)
		{
			m_iBotCount = MIN_BOT_COUNT;
		}
		LeaveCriticalSection(&m_GameMgrCS);
	}
	return true;
}
bool CGameMgr::SetGameModeType(eGameModeType _eGameModeType)
{
	EnterCriticalSection(&m_GameMgrCS);
	m_eGameModeType = _eGameModeType;
	LeaveCriticalSection(&m_GameMgrCS);
	return true;
}
bool CGameMgr::SetGameType(eGameType _eGameType)
{
	EnterCriticalSection(&m_GameMgrCS);
	m_eGameType = _eGameType;
	LeaveCriticalSection(&m_GameMgrCS);

	if (m_eGameType == GAME_LOADING)
	{
		//release player's ready + send all player isn't ready
		CClientMgr::GetInstance()->ResetAllPlayerReady();
		//setting unready packet
		SC_AllUnready tUnReadyPacket;


		//ObjMgr setting
		CObjMgr::GetInstance()->SetObjectsPos();


		//setting InitPlayer packet
		SC_InitPlayer tInitPlayerPacket;
		for (int i = 0; i < MAX_USER; ++i)
		{
			tInitPlayerPacket.m_vPlayerPos[i].y = 1.f;
		}
		unordered_map<int, CObj*>::iterator player_iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator player_iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (player_iter; player_iter != player_iter_end; ++player_iter)
		{
			//수정필요 -> 솔로모드 vs 팀모드 / 블루팀 vs 레드팀
			tInitPlayerPacket.m_bIsSoloOrTeam[player_iter->first] = true;
			tInitPlayerPacket.m_bIsRedOrBlue[player_iter->first] = true;
			tInitPlayerPacket.m_vPlayerPos[player_iter->first] = player_iter->second->GetPos();
		}
		

		//setting InitBot packet
		SC_InitBot tInitBotPacket;
		for (int i = 0; i < MAX_BOT_COUNT; ++i)
		{
			tInitBotPacket.m_vBotPos[i].y = 1.f;
		}
		int iBotCount = 0;
		unordered_map<int, CObj*>::iterator bot_iter = CObjMgr::GetInstance()->GetBotList()->begin();
		unordered_map<int, CObj*>::iterator bot_iter_end = CObjMgr::GetInstance()->GetBotList()->end();
		for (bot_iter; bot_iter != bot_iter_end; ++bot_iter)
		{
			tInitBotPacket.m_vBotPos[bot_iter->first] = bot_iter->second->GetPos();
			++iBotCount;
		}
		tInitBotPacket.m_iBotCount = iBotCount;


		//setting InitBead packet
		SC_InitBead tInitBeadPacket;
		unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
		unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
		for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
		{
			tInitBeadPacket.m_vBeadPos[bead_iter->first] = bead_iter->second->GetPos();
		}

		
		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
		unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			CClientMgr::GetInstance()->SendPacket(iter->first, &tUnReadyPacket);
			CClientMgr::GetInstance()->SendPacket(iter->first, &tInitPlayerPacket);
			CClientMgr::GetInstance()->SendPacket(iter->first, &tInitBotPacket);
			CClientMgr::GetInstance()->SendPacket(iter->first, &tInitBeadPacket);
		}

		//loading time
		DWORD dwTime = GetTickCount();
		while (dwTime + 1250 > GetTickCount()) {}

		//change scene GAME_PLAY
		SetGameType(GAME_PLAY);
	}
	else if (m_eGameType == GAME_PLAY)
	{
		cout << "Go to In Game" << endl;

		//game countdown setting(registry timerQ)
		CTimerQMgr::GetInstance()->AddTimerQ(EVENT_COUNTDOWN, 0.f, GAME_START_COUNT - 1);

		//game bot move setting
		for (int i = 0; i < m_iBotCount; ++i)
		{
			CTimerQMgr::GetInstance()->AddTimerQ(EVENT_BOT_MOVE, 4.f, i);
		}
	}
	else if (m_eGameType == GAME_ROOM)
	{
		//SetStageStateType STAGE_RESULT
		SetStageStateType(STAGE_RESULT);
		
		//packet setting
		SC_GameState tPacket;
		tPacket.m_iGameState = STAGE_RESULT;
		//bead max count
		int iMaxBeadCount = CObjMgr::GetInstance()->CheckHavingBeadPerPlayerAndReturnMaxCount();
		int* pPlayerPerHavingBead = CObjMgr::GetInstance()->GetHavingBeadPerPlayer();
		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
		unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			//DB에 추가해야되는 부분
			if (pPlayerPerHavingBead[iter->first] == iMaxBeadCount)
			{
				tPacket.m_bGameWinOrLose = true;
			}
			else
			{
				tPacket.m_bGameWinOrLose = false;

			}
			CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
		}

		//after 3sec, move room
		CTimerQMgr::GetInstance()->AddTimerQ(EVENT_MOVE_ROOM, 3.f);
	}

	return true;
}
void CGameMgr::SetStageStateType(eStageType _eStageType)
{
	EnterCriticalSection(&m_GameMgrCS);
	m_eStageType = _eStageType;
	LeaveCriticalSection(&m_GameMgrCS);
}

void CGameMgr::DisconnectRoomMember(int _iID)
{
	//index
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_iRoomIndexID[i] == _iID)
		{
			EnterCriticalSection(&m_GameMgrCS);
			m_iRoomIndexID[i] = UNKNOWN_VALUE;
			LeaveCriticalSection(&m_GameMgrCS);
			return;
		}
	}
}
int	CGameMgr::GetEmptyRoomIndex(int _iID)
{
	int iTempRoomIndex = UNKNOWN_VALUE;

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_iRoomIndexID[i] == UNKNOWN_VALUE)
		{
			EnterCriticalSection(&m_GameMgrCS);
			m_iRoomIndexID[i] = _iID;
			iTempRoomIndex = i;
			LeaveCriticalSection(&m_GameMgrCS);
			return iTempRoomIndex;
		}
	}
	cout << "CGameMgr GetEmptyRoomIndex() error" << endl;
	return iTempRoomIndex;
}
int CGameMgr::ChangeRoomIndex(int _iID)
{
	int iTempIndex = 0;

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_iRoomIndexID[i] == _iID)
		{
			EnterCriticalSection(&m_GameMgrCS);
			iTempIndex = i;
			LeaveCriticalSection(&m_GameMgrCS);
			return CalculateChangeIndex(iTempIndex, _iID);
		}
	}
	cout << "CGameMgr ChangeRoomIndex() error" << endl;
	return UNKNOWN_VALUE;
}

int CGameMgr::CalculateChangeIndex(int _iIndex, int _iID)
{
	//ready error check
	if (CClientMgr::GetInstance()->FindClient(_iID)->m_bIsReady)
	{
		return _iIndex;
	}

	//calculate change index
	switch (_iIndex % 2)
	{
	//even number
	case 0:
		for (int i = 0; i < RED_COUNT; ++i)
		{
			if (m_iRoomIndexID[2 * i + 1] == UNKNOWN_VALUE)
			{
				EnterCriticalSection(&m_GameMgrCS);
				m_iRoomIndexID[_iIndex] = UNKNOWN_VALUE;
				m_iRoomIndexID[2 * i + 1] = _iID;
				LeaveCriticalSection(&m_GameMgrCS);
				return (2 * i + 1);
			}
		}
		break;
	//odd number
	case 1:
		for (int i = 0; i < BLUE_COUNT; ++i)
		{
			if (m_iRoomIndexID[2 * i] == UNKNOWN_VALUE)
			{
				EnterCriticalSection(&m_GameMgrCS);
				m_iRoomIndexID[_iIndex] = UNKNOWN_VALUE;
				m_iRoomIndexID[2 * i] = _iID;
				LeaveCriticalSection(&m_GameMgrCS);
				return (2 * i);
			}
		}
		break;
	}

	//don't exist empty index
	return _iIndex;
}