#include "stdafx.h"
#include "ClientMgr.h"

#include "Protocol.h"

#include "Player.h"
#include "Bot.h"
#include "Bead.h"

#include "ObjMgr.h"
#include "GameMgr.h"

IMPLEMENT_SINGLETON(CClientMgr)

CClientMgr::CClientMgr()
{
	InitializeCriticalSection(&m_ClientMgrCS);
}
CClientMgr::~CClientMgr()
{
	Clear();

	DeleteCriticalSection(&m_ClientMgrCS);
}

void CClientMgr::Clear(void)
{
	/*
	Don't use Critical Section
	Because This Part is Released part
	*/

	//client list
	unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
	unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();

	for (iter; iter != iter_end; ++iter)
	{
		delete iter->second;
		iter->second = nullptr;
	}

	m_ClientList.clear();

	//client isconnect
	for (int i = 0; i < MAX_USER; ++i)
	{
		m_bIsUseID[i] = false;
	}
}

bool CClientMgr::Initialize(void)
{
	/*
	Don't use Critical Section
	Because This Part is Initialized part
	*/

	m_ClientList.clear();

	for (int i = 0; i < MAX_USER; ++i)
	{
		m_bIsUseID[i] = false;
	}

	return true;
}
int	CClientMgr::GetClientID(void)
{
	int iClientID = 0;

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_bIsUseID[i])
		{
			++iClientID;
		}
		else if (!m_bIsUseID[i])
		{
			EnterCriticalSection(&m_ClientMgrCS);
			m_bIsUseID[i] = true;
			LeaveCriticalSection(&m_ClientMgrCS);
			return iClientID;
		}
	}
	return UNKNOWN_VALUE;
}
bool CClientMgr::InsertClient(int _iID, CLIENT* _pClient)
{
	//insert client
	EnterCriticalSection(&m_ClientMgrCS);
	m_ClientList.insert(make_pair(_iID, _pClient));
	LeaveCriticalSection(&m_ClientMgrCS);

	return true;
}
bool CClientMgr::EraseClient(int _iID)
{
	EnterCriticalSection(&m_ClientMgrCS);
	unordered_map<int, CLIENT*>::iterator iter = m_ClientList.find(_iID);
	if (iter == m_ClientList.end())
	{
		cout << "Don't Erase Client!" << endl;
		LeaveCriticalSection(&m_ClientMgrCS);
		return false;
	}

	iter = m_ClientList.erase(iter);
	LeaveCriticalSection(&m_ClientMgrCS);

	return true;
}
CLIENT* CClientMgr::FindClient(int _iID)
{
	EnterCriticalSection(&m_ClientMgrCS);
	unordered_map<int, CLIENT*>::iterator iter = m_ClientList.find(_iID);

	if (iter == m_ClientList.end())
	{
		cout << "CClientMgr FindClient() error" << endl;
		LeaveCriticalSection(&m_ClientMgrCS);
		return nullptr;
	}
	LeaveCriticalSection(&m_ClientMgrCS);

	return iter->second;
}

void CClientMgr::ResetAllPlayerReady(void)
{
	EnterCriticalSection(&m_ClientMgrCS);
	unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
	unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
	for (iter; iter != iter_end; ++iter)
	{
		iter->second->m_bIsReady = false;
	}
	LeaveCriticalSection(&m_ClientMgrCS);
}
bool CClientMgr::CheckReadyByHost(void)
{
	EnterCriticalSection(&m_ClientMgrCS);
	unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
	unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
	for (iter; iter != iter_end; ++iter)
	{
		if (!iter->second->m_bIsReady)
		{
			if (iter->first == CGameMgr::GetInstance()->GetHostID())
			{
				continue;
			}
			LeaveCriticalSection(&m_ClientMgrCS);
			return false;
		}
	}
	LeaveCriticalSection(&m_ClientMgrCS);
	return true;
}

//send packet. process packet
bool CClientMgr::SendPacket(int _iID, void* _pPacket)
{
	int iPacketSize = reinterpret_cast<HEADER*>(_pPacket)->size;

	//check Protocol
	//cout << "<------- PROTOCOL :" << (int)(reinterpret_cast<HEADER*>(_pPacket)->type) << "  /  ID :" << _iID << endl;

	OVERLAPPED_EX* pSendOver = new OVERLAPPED_EX;
	ZeroMemory(pSendOver, sizeof(OVERLAPPED_EX));

	//setting sendover
	pSendOver->m_iOperationType = OP_SEND;
	pSendOver->wsaBuf.buf = pSendOver->m_szIOCPbuf;
	pSendOver->wsaBuf.len = iPacketSize;

	memcpy(pSendOver->m_szIOCPbuf, _pPacket, iPacketSize);

	DWORD dwIOSize = 0;
	WSASend(m_ClientList[_iID]->m_tSock, &pSendOver->wsaBuf, 1, &dwIOSize, NULL, &pSendOver->m_tOverLapped, nullptr);

	return true;
}
bool CClientMgr::ProcessPacket(char* _pPacket, int _iID)
{
	HEADER* pHeader = reinterpret_cast<HEADER*>(_pPacket);

	//check Protocol
	//cout << "-------> PROTOCOL :" << (int)pHeader->type << "  /  ID :" << _iID << endl;

	switch (pHeader->type)
	{
		//room
	case CS_INIT_ROOM:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_ROOM)
		{
			break;
		}

		CS_InitRoom* pPacket = reinterpret_cast<CS_InitRoom*>(_pPacket);

		CLIENT* pClient = FindClient(_iID);

		EnterCriticalSection(&m_ClientMgrCS);
		memcpy(pClient->m_szName, pPacket->m_szName, sizeof(char) * MAX_NAME);
		LeaveCriticalSection(&m_ClientMgrCS);

		//수정필요
		//일단 CLIENT의 문자열에는 적용완료
		//추후에 어떻게 적용시킬지 생각
		////packet send to all
		//unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		//unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		//for (iter; iter != iter_end; ++iter)
		//{
		//	SendPacket(iter->first, &tPacket);
		//}

		break;
	}
	case CS_CLIENT_READY:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_ROOM)
		{
			break;
		}

		CS_Ready* pPacket = reinterpret_cast<CS_Ready*>(_pPacket);

		CLIENT* pClient = FindClient(_iID);

		if (pPacket->m_bIsHost)
		{
			if (CheckReadyByHost())
			{
				EnterCriticalSection(&m_ClientMgrCS);
				pClient->m_bIsReady = true;
				LeaveCriticalSection(&m_ClientMgrCS);
				CGameMgr::GetInstance()->SetGameType(GAME_LOADING);
			}
		}
		else
		{
			bool bIsReady = pClient->m_bIsReady;
			bIsReady = !bIsReady;

			EnterCriticalSection(&m_ClientMgrCS);
			pClient->m_bIsReady = bIsReady;
			LeaveCriticalSection(&m_ClientMgrCS);

			//packet setting
			SC_Ready tPacket;
			tPacket.m_bIsAll = CheckReadyByHost();
			tPacket.m_iID = _iID;
			tPacket.m_bReady = bIsReady;

			//packet send to all
			unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
			unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
			for (iter; iter != iter_end; ++iter)
			{
				SendPacket(iter->first, &tPacket);
			}
		}
		break;
	}
	case CS_CLIENT_CHANGE:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_ROOM)
		{
			break;
		}

		CS_Change* pPacket = reinterpret_cast<CS_Change*>(_pPacket);

		//index setting
		int iIndex = CGameMgr::GetInstance()->ChangeRoomIndex(_iID);
		CLIENT* pClient = FindClient(_iID);
		EnterCriticalSection(&m_ClientMgrCS);
		pClient->m_iRoomIndex = iIndex;
		LeaveCriticalSection(&m_ClientMgrCS);
		//error check
		if (pClient->m_iRoomIndex == UNKNOWN_VALUE)
		{
			cout << "CClientMgr CS_CLIENT_CHANGE index error!" << endl;
			while (true);
		}

		//packet setting
		SC_Change tPacket;
		tPacket.m_iTeamIndex = iIndex;
		tPacket.m_iID = _iID;

		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tPacket);
		}
		break;
	}
	case CS_OPTION_CHANGE:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_ROOM)
		{
			break;
		}

		CS_OptionChagne* pPacket = reinterpret_cast<CS_OptionChagne*>(_pPacket);

		//host check
		if (pPacket->m_bIsHost)
		{
			if (pPacket->m_bDownMode || pPacket->m_bUpMode)
			{
				eGameModeType eTempMode = CGameMgr::GetInstance()->GetGameModeType();
				if (eTempMode == MODE_SOLO)
				{
					CGameMgr::GetInstance()->SetGameModeType(MODE_TEAM);
				}
				else if (eTempMode == MODE_TEAM)
				{
					CGameMgr::GetInstance()->SetGameModeType(MODE_SOLO);
				}
			}

			if (pPacket->m_bDownRound)
			{
				CGameMgr::GetInstance()->SetRoundCount(false, ROUND_UNIT);
			}
			else if (pPacket->m_bUpRound)
			{
				CGameMgr::GetInstance()->SetRoundCount(true, ROUND_UNIT);
			}

			if (pPacket->m_bDownBot)
			{
				CGameMgr::GetInstance()->SetBotCount(false, BOT_UNIT);
			}
			else if (pPacket->m_bUpBot)
			{
				CGameMgr::GetInstance()->SetBotCount(true, BOT_UNIT);
			}
		}

		//packet setting
		SC_OptionChange tPacket;
		tPacket.m_bGameMode = CGameMgr::GetInstance()->GetGameModeType();
		tPacket.m_iBotCount = CGameMgr::GetInstance()->GetBotCount();
		tPacket.m_iRoundCount = CGameMgr::GetInstance()->GetRoundCount();

		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tPacket);
		}
		break;
	}
	case CS_CHAT_ROOM:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_ROOM)
		{
			break;
		}

		//rendering chat
		CS_ChatRoom* pChatPacket = reinterpret_cast<CS_ChatRoom*>(_pPacket);
		cout << _iID << " : " << pChatPacket->m_szChat << endl;

		//수정필요
		//저장해놓는 대기방채팅창에 추가

		//chat resetting
		SC_ChatRoom tChat;
		memcpy(tChat.m_szChat, pChatPacket->m_szChat, sizeof(char) * MAX_CHAT);
		tChat.m_iID = _iID;

		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tChat);
		}
		break;
	}
	//game
	case CS_INPUT_KEY:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_PLAY)
		{
			break;
		}

		CS_InputKey* pPacket = reinterpret_cast<CS_InputKey*>(_pPacket);
		
		EnterCriticalSection(&m_ClientMgrCS);
		reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetKey(pPacket->m_dwKey);
		LeaveCriticalSection(&m_ClientMgrCS);

		cout << "ID: " << _iID <<"Key Value:" << (int)pPacket->m_dwKey << endl;

		//packet setting
		SC_ChangeStatePlayer tPacket;
		EnterCriticalSection(&m_ClientMgrCS);
		tPacket.m_iPlayerID = _iID;
		tPacket.m_fPlayerAngleY = (*CObjMgr::GetInstance()->GetPlayerList())[_iID]->GetAngleY();
		tPacket.m_iPlayerKeyState = reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->GetKey();
		tPacket.m_vPlayerPos = (*CObjMgr::GetInstance()->GetPlayerList())[_iID]->GetPos();
		LeaveCriticalSection(&m_ClientMgrCS);

		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tPacket);
		}
		break;
	}
	case CS_ANIMATION_END:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_PLAY)
		{
			break;
		}

		CS_AnimationEnd* pPacket = reinterpret_cast<CS_AnimationEnd*>(_pPacket);

		EnterCriticalSection(&m_ClientMgrCS);
		reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetAnimationEnd(true);
		LeaveCriticalSection(&m_ClientMgrCS);
		break;
	}
	case CS_CHAT_GAME:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_PLAY)
		{
			break;
		}

		CS_ChatGame* pChatPacket = reinterpret_cast<CS_ChatGame*>(_pPacket);
		cout << _iID << " : " << pChatPacket->m_szChat << endl;

		//수정필요
		//저장해놓는 인게임채팅창에 추가

		//chat resetting
		SC_ChatGame tChat;
		memcpy(tChat.m_szChat, pChatPacket->m_szChat, sizeof(char) * MAX_CHAT);
		tChat.m_iID = _iID;
		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tChat);
		}
		break;
	}
	case CS_COLLISION_PLAYER_PLAYER:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_PLAY)
		{
			break;
		}

		CS_CollisionPlayerPlayer* pPacket = reinterpret_cast<CS_CollisionPlayerPlayer*>(_pPacket);
		if (pPacket->m_iMyID != _iID)
		{
			cout << "CS_COLLISION_PLAYER_PLAYER m_iMyID error" << endl;
			break;
		}
		if (pPacket->m_iMyID == pPacket->m_iTargetPlayerID)
		{
			cout << "CS_COLLISION_PLAYER_PLAYER m_iTargetPlayerID error" << endl;
			break;
		}


		//player's state check
		EnterCriticalSection(&m_ClientMgrCS);
		if ((*CObjMgr::GetInstance()->GetPlayerList())[pPacket->m_iMyID]->GetObjState() == OBJ_STATE_DIE)
		{
			cout << "Attacking Player_" << pPacket->m_iTargetPlayerID << " is already die state!" << endl;
			LeaveCriticalSection(&m_ClientMgrCS);
			break;
		}
		if ((*CObjMgr::GetInstance()->GetPlayerList())[pPacket->m_iTargetPlayerID]->GetObjState() == OBJ_STATE_DIE)
		{
			cout << "Attacked Player_" << pPacket->m_iTargetPlayerID << " is already die state!" << endl;
			LeaveCriticalSection(&m_ClientMgrCS);
			break;
		}
		//change player's state
		((*CObjMgr::GetInstance()->GetPlayerList())[pPacket->m_iTargetPlayerID])->SetObjState(OBJ_STATE_DIE);
		LeaveCriticalSection(&m_ClientMgrCS);


		//died player's bead regen
		unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
		unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
		for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
		{
			int iBeadOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
			if (iBeadOwnerID == pPacket->m_iTargetPlayerID)
			{
				EnterCriticalSection(&m_ClientMgrCS);
				reinterpret_cast<CBead*>(bead_iter->second)->SetOwnerID(UNKNOWN_VALUE);
				LeaveCriticalSection(&m_ClientMgrCS);

				//packet setting
				SC_ChangeStateBead tPacket;
				tPacket.m_iOwnerPlayerID = UNKNOWN_VALUE;
				tPacket.m_iBeadID = bead_iter->first;
				//packet send to all
				unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
				unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
				for (iter; iter != iter_end; ++iter)
				{
					SendPacket(iter->first, &tPacket);
				}
			}
		}
		

		//packet setting
		SC_CheckCollision tPacket;
		tPacket.m_iType = 1;//1.Player Player
		tPacket.m_iMainID = pPacket->m_iMyID;
		tPacket.m_iTargetID = pPacket->m_iTargetPlayerID;
		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tPacket);
		}
		break;
	}
	case CS_COLLISION_PLAYER_BOT:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_PLAY)
		{
			break;
		}

		CS_CollisionPlayerBot* pPacket = reinterpret_cast<CS_CollisionPlayerBot*>(_pPacket);
		if (pPacket->m_iMyID != _iID)
		{
			cout << "CS_COLLISION_PLAYER_BOT m_iMyID error" << endl;
			break;
		}

		//bot's state check
		EnterCriticalSection(&m_ClientMgrCS);
		if ((*CObjMgr::GetInstance()->GetBotList())[pPacket->m_iBotID]->GetObjState() == OBJ_STATE_DIE)
		{
			cout << "Bot_" << pPacket->m_iBotID << " is already die state!" << endl;
			LeaveCriticalSection(&m_ClientMgrCS);
			break;
		}
		//change bot's state
		(*CObjMgr::GetInstance()->GetBotList())[pPacket->m_iBotID]->SetObjState(OBJ_STATE_DIE);
		LeaveCriticalSection(&m_ClientMgrCS);

		//packet setting
		SC_CheckCollision tPacket;
		tPacket.m_iType = 2;//2.Player Bot
		tPacket.m_iMainID = pPacket->m_iMyID;
		tPacket.m_iTargetID = pPacket->m_iBotID;
		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tPacket);
		}
		break;
	}
	case CS_COLLISION_PLAYER_BEAD:
	{
		if (CGameMgr::GetInstance()->GetGameType() != GAME_PLAY)
		{
			break;
		}

		CS_CollisionPlayerBead* pPacket = reinterpret_cast<CS_CollisionPlayerBead*>(_pPacket);
		if (pPacket->m_iMyID != _iID)
		{
			cout << "CS_COLLISION_PLAYER_BEAD m_iMyID error" << endl;
			break;
		}

		//bead's state check
		EnterCriticalSection(&m_ClientMgrCS);
		if (reinterpret_cast<CBead*>((*CObjMgr::GetInstance()->GetBeadList())[pPacket->m_iBeadID])->GetOwnerID() != UNKNOWN_VALUE)
		{
			cout << "Bead_" << pPacket->m_iBeadID << " is already die state!" << endl;
			LeaveCriticalSection(&m_ClientMgrCS);
			break;
		}
		//change bead's state
		reinterpret_cast<CBead*>((*CObjMgr::GetInstance()->GetBeadList())[pPacket->m_iBeadID])->SetOwnerID(pPacket->m_iMyID);
		LeaveCriticalSection(&m_ClientMgrCS);


		//packet setting
		SC_CheckCollision tPacket;
		tPacket.m_iType = 3;//3.Player Bead
		tPacket.m_iMainID = pPacket->m_iMyID;
		tPacket.m_iTargetID = pPacket->m_iBeadID;
		//packet send to all
		unordered_map<int, CLIENT*>::iterator iter = m_ClientList.begin();
		unordered_map<int, CLIENT*>::iterator iter_end = m_ClientList.end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &tPacket);
		}
		break;
	}

	default:
		return false;
	}
	return true;
}