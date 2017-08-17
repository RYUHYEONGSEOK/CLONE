#include "stdafx.h"
#include "GameMgr.h"

#include "PacketMgr.h"
#include "TimerQMgr.h"
#include "ObjMgr.h"

#include "Player.h"

IMPLEMENT_SINGLETON(CGameMgr)

CGameMgr::CGameMgr()
	: m_iHostID(UNKNOWN_VALUE)
	, m_eMapType(MAP_KJK_PLAZA)
	, m_iBotCount(70)
{
	m_eGameType = GAME_END;
	m_eGameModeType = MODE_END;
	m_eStageType = STAGE_READY;

	for (int i = 0; i < MAX_USER; ++i) m_iRoomIndexID[i] = UNKNOWN_VALUE;
}
CGameMgr::~CGameMgr()
{
	Release();
}

bool CGameMgr::Initialize(void)
{
	m_eMapType		= MAP_KJK_PLAZA;
	m_iBotCount		= 70;

	m_eGameType		= GAME_ROOM;
	m_eGameModeType = MODE_SOLO;

	return true;
}
bool CGameMgr::Release(void)
{
	for (int i = 0; i < MAX_USER; ++i) m_iRoomIndexID[i] = UNKNOWN_VALUE;
	return true;
}

bool CGameMgr::SetHostID(int _iID)
{
	m_mutexGameMgr.lock();
	m_iHostID = _iID;
	m_mutexGameMgr.unlock();

	return true;
}
bool CGameMgr::SetMapType(void)
{
	m_mutexGameMgr.lock();
	if (m_eMapType == MAP_KJK_PLAZA) m_eMapType = MAP_KJK_POOL;
	else if (m_eMapType == MAP_KJK_POOL) m_eMapType = MAP_KJK_PLAZA;
	else if (m_eMapType == MAP_END) cout << "CGameMgr SetMapType() error!" << endl;
	m_mutexGameMgr.unlock();

	return true;
}
bool CGameMgr::SetBotCount(bool _bUpAndDown)
{
	if (_bUpAndDown)//up
	{
		m_mutexGameMgr.lock();
		if (m_iBotCount < MAX_BOT_COUNT)
		{
			m_iBotCount += BOT_UNIT;
			m_mutexGameMgr.unlock();
		}
		else m_mutexGameMgr.unlock();
	}
	else//down
	{
		m_mutexGameMgr.lock();
		if (m_iBotCount > MIN_BOT_COUNT)
		{
			m_iBotCount -= BOT_UNIT;
			m_mutexGameMgr.unlock();
		}
		else m_mutexGameMgr.unlock();
	}
	return true;
}
bool CGameMgr::SetGameModeType(eGameModeType _eGameModeType)
{
	m_mutexGameMgr.lock();
	m_eGameModeType = _eGameModeType;
	m_mutexGameMgr.unlock();

	return true;
}
bool CGameMgr::SetGameType(eGameType _eGameType)
{
	m_mutexGameMgr.lock();
	m_eGameType = _eGameType;
	m_mutexGameMgr.unlock();

	if (m_eGameType == GAME_LOADING)
	{
		cout << "------------------------" << endl;
		cout << "<< Go to In Loading >>" << endl;

		m_mutexGameMgr.lock();
		eMapType eTempMapType = CGameMgr::GetInstance()->GetMapType();
		m_mutexGameMgr.unlock();

		//ObjMgr setting
		CObjMgr::GetInstance()->SetObjectsPos(eTempMapType);

		//release player's ready + send all player isn't ready
		CObjMgr::GetInstance()->ResetAllPlayerReady();

		//setting InitBot packet
		for (int i = 0; i < MAX_BOT_COUNT; ++i)
		{
			m_tInitBotPacket.m_vBotPos[i].y = 1.f;
		}
		int iBotCount = 0;
		unordered_map<int, CObj*>::iterator bot_iter = CObjMgr::GetInstance()->GetBotList()->begin();
		unordered_map<int, CObj*>::iterator bot_iter_end = CObjMgr::GetInstance()->GetBotList()->end();
		for (bot_iter; bot_iter != bot_iter_end; ++bot_iter)
		{
			m_tInitBotPacket.m_vBotPos[bot_iter->first] = bot_iter->second->GetPos();
			++iBotCount;
		}
		m_tInitBotPacket.m_iBotCount = iBotCount;

		//setting InitBead packet
		unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
		unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
		for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
		{
			m_tInitBeadPacket.m_vBeadPos[bead_iter->first] = bead_iter->second->GetPos();
		}

		//setting InitPlayer packet
		for (int i = 0; i < MAX_USER; ++i)
		{
			m_tInitPlayerPacket.m_vPlayerPos[i].y = 1.f;
		}
		unordered_map<int, CObj*>::iterator player_iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator player_iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (player_iter; player_iter != player_iter_end; ++player_iter)
		{
			switch (m_eGameModeType)
			{
			case MODE_SOLO:
			{
				//Don't Use This Flag
				m_tInitPlayerPacket.m_bIsRedOrBlue[player_iter->first] = true;	
				break;
			}
			case MODE_TEAM:
			{
				if (0 == (reinterpret_cast<CPlayer*>(player_iter->second)->GetRoomIndex() % 2)) m_tInitPlayerPacket.m_bIsRedOrBlue[player_iter->first] = true;
				else m_tInitPlayerPacket.m_bIsRedOrBlue[player_iter->first] = false;
				break;
			}
			default: cout << "m_tInitPlayerPacket m_eGameModeType error!" << endl; break;
			}
			m_tInitPlayerPacket.m_vPlayerPos[player_iter->first] = player_iter->second->GetPos();
		}

		//packet send to all
		player_iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		player_iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (player_iter; player_iter != player_iter_end; ++player_iter)
		{
			CPacketMgr::GetInstance()->SendPacket(player_iter->first, &m_tUnReadyPacket);
			CPacketMgr::GetInstance()->SendPacket(player_iter->first, &m_tInitBotPacket);
			CPacketMgr::GetInstance()->SendPacket(player_iter->first, &m_tInitBeadPacket);
			CPacketMgr::GetInstance()->SendPacket(player_iter->first, &m_tInitPlayerPacket);
		}

		//change scene GAME_PLAY
		SetGameType(GAME_PLAY);
	}
	else if (m_eGameType == GAME_PLAY)
	{
		cout << "<< Go to In Game >>" << endl;

		//game countdown setting(registry timerQ)
		CTimerQMgr::GetInstance()->AddTimerQ(EVENT_COUNTDOWN, high_resolution_clock::now(), GAME_START_COUNT - 1);

		//game sync setting(registry timerQ)
		CTimerQMgr::GetInstance()->AddTimerQ(EVENT_SYNC_OBJ, high_resolution_clock::now(), UNKNOWN_VALUE);

		//game bot move setting
		for (int i = 0; i < m_iBotCount; ++i)
		{
			CTimerQMgr::GetInstance()->AddTimerQ(EVENT_BOT_MOVE, high_resolution_clock::now() + 4s, i);
		}
	}
	else if (m_eGameType == GAME_ROOM)
	{
		cout << "<< Go to In Room >>" << endl;
		cout << "------------------------" << endl;

		//SetStageStateType STAGE_RESULT
		SetStageStateType(STAGE_RESULT);

		//packet setting
		m_tGameStatePacket.m_iGameState = STAGE_RESULT;

		switch (m_eGameModeType)
		{
		case MODE_SOLO:
		{
			//player
			if (CObjMgr::GetInstance()->CheckPlayerState(m_eGameModeType))	
			{
				unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					if (iter->second->GetObjState() == OBJ_STATE_DIE) m_tGameStatePacket.m_bGameWinOrLose = false;
					else m_tGameStatePacket.m_bGameWinOrLose = true;
					CPacketMgr::GetInstance()->SendPacket(iter->first, &m_tGameStatePacket);
				}
			}
			//bead
			else 
			{
				//bead max count
				int iMaxBeadCount = CObjMgr::GetInstance()->CheckHavingBeadPerPlayerAndReturnMaxCount();
				int* pPlayerPerHavingBead = CObjMgr::GetInstance()->GetHavingBeadPerPlayer();

				//packet send to all
				unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					if (pPlayerPerHavingBead[iter->first] == iMaxBeadCount) m_tGameStatePacket.m_bGameWinOrLose = true;
					else m_tGameStatePacket.m_bGameWinOrLose = false;
					CPacketMgr::GetInstance()->SendPacket(iter->first, &m_tGameStatePacket);
				}
			}

			break;
		}
		case MODE_TEAM:
		{
			bool bIsRedWin = true;

			if (CObjMgr::GetInstance()->CheckPlayerState(m_eGameModeType))
			{
				int iRedTeamPlayerCount = CObjMgr::GetInstance()->GetTeamAlivedPlayerCount(PLAYER_RED);
				int iBlueTeamPlayerCount = CObjMgr::GetInstance()->GetTeamAlivedPlayerCount(PLAYER_BLUE);

				if (iRedTeamPlayerCount > iBlueTeamPlayerCount) bIsRedWin = true;
				else bIsRedWin = false;
			}
			else
			{
				int iRedTeamBeadCount = CObjMgr::GetInstance()->GetTeamBeadCount(PLAYER_RED);
				int iBlueTeamBeadCount = CObjMgr::GetInstance()->GetTeamBeadCount(PLAYER_BLUE);

				if (iRedTeamBeadCount > iBlueTeamBeadCount) bIsRedWin = true;
				else bIsRedWin = false;
			}

			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				if (0 == reinterpret_cast<CPlayer*>(iter->second)->GetRoomIndex() % 2) m_tGameStatePacket.m_bGameWinOrLose = bIsRedWin;
				else m_tGameStatePacket.m_bGameWinOrLose = !bIsRedWin;
				CPacketMgr::GetInstance()->SendPacket(iter->first, &m_tGameStatePacket);
			}

			break;
		}
		case MODE_END:
		default: cout << "CGameMgr SetGameType() GAME_ROOM type error!" << endl; break;
		}


		//after 3sec, move room
		CTimerQMgr::GetInstance()->AddTimerQ(EVENT_MOVE_ROOM, high_resolution_clock::now() + 3s);
	}

	return true;
}
void CGameMgr::SetStageStateType(eStageType _eStageType)
{
	m_mutexGameMgr.lock();
	m_eStageType = _eStageType;
	m_mutexGameMgr.unlock();
}

void CGameMgr::DisconnectRoomMember(int _iID)
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_iRoomIndexID[i] == _iID)
		{
			m_mutexGameMgr.lock();
			m_iRoomIndexID[i] = UNKNOWN_VALUE;
			m_mutexGameMgr.unlock();
			return;
		}
	}

	cout << "CGameMgr DisconnectRoomMember() error" << endl;
}
int	CGameMgr::GetEmptyRoomIndex(int _iID)
{
	int iTempRoomIndex = UNKNOWN_VALUE;

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_iRoomIndexID[i] == UNKNOWN_VALUE)
		{
			m_mutexGameMgr.lock();
			m_iRoomIndexID[i] = _iID;
			iTempRoomIndex = i;
			m_mutexGameMgr.unlock();
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
			m_mutexGameMgr.lock();
			iTempIndex = i;
			m_mutexGameMgr.unlock();
			return CalculateChangeIndex(iTempIndex, _iID);
		}
	}
	cout << "CGameMgr ChangeRoomIndex() error" << endl;
	return UNKNOWN_VALUE;
}

int CGameMgr::CalculateChangeIndex(int _iIndex, int _iID)
{
	//ready error check
	if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID)) return _iIndex;
	if (reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->GetIsReady()) return _iIndex;

	//calculate change index
	switch (_iIndex % 2)
	{
	case 0://even number
		for (int i = 0; i < RED_COUNT; ++i)
		{
			if (m_iRoomIndexID[2 * i + 1] == UNKNOWN_VALUE)
			{
				m_mutexGameMgr.lock();
				m_iRoomIndexID[_iIndex] = UNKNOWN_VALUE;
				m_iRoomIndexID[2 * i + 1] = _iID;
				m_mutexGameMgr.unlock();
				return (2 * i + 1);
			}
		}
		break;
	case 1://odd number
		for (int i = 0; i < BLUE_COUNT; ++i)
		{
			if (m_iRoomIndexID[2 * i] == UNKNOWN_VALUE)
			{
				m_mutexGameMgr.lock();
				m_iRoomIndexID[_iIndex] = UNKNOWN_VALUE;
				m_iRoomIndexID[2 * i] = _iID;
				m_mutexGameMgr.unlock();
				return (2 * i);
			}
		}
		break;
	}

	//don't exist empty index
	return _iIndex;
}