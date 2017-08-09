#include "stdafx.h"
#include "PacketMgr.h"

#include "ObjMgr.h"
#include "GameMgr.h"

#include "Player.h"
#include "Bead.h"

IMPLEMENT_SINGLETON(CPacketMgr)

CPacketMgr::CPacketMgr()
{
}
CPacketMgr::~CPacketMgr()
{
}

bool CPacketMgr::SendPacket(int _iID, void* _pPacket)
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

	if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID))
	{
		delete pSendOver;
		return false;
	}

	DWORD dwIOSize = 0;
	SOCKET tSocket = reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->GetSocket();
	WSASend(tSocket, &pSendOver->wsaBuf, 1, &dwIOSize, NULL, &pSendOver->m_tOverLapped, nullptr);

	return true;
}
bool CPacketMgr::ProcessPacket(char* _pPacket, int _iID)
{
	g_GlobalMutex.lock();
	eGameType eTmepGameType = CGameMgr::GetInstance()->GetGameType();
	g_GlobalMutex.unlock();

	HEADER* pHeader = reinterpret_cast<HEADER*>(_pPacket);

	//check Protocol
	//cout << "-------> PROTOCOL :" << (int)pHeader->type << "  /  ID :" << _iID << endl;

	switch (pHeader->type)
	{
	case CS_INIT_ROOM:
	{
		if (eTmepGameType != GAME_ROOM) break;

		CS_InitRoom* pPacket = reinterpret_cast<CS_InitRoom*>(_pPacket);
		if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID))
		{
			cout << "CS_INIT_ROOM error" << endl;
			break;
		}

		m_mutexPacketMgr.lock();
		reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetName(pPacket->m_szName);
		m_mutexPacketMgr.unlock();

		//수정필요
		//일단 CLIENT의 문자열에는 적용완료
		//추후에 어떻게 적용시킬지 생각
		//packet send to all
		//unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		//unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		//for (iter; iter != iter_end; ++iter)
		//{
		//	SendPacket(iter->first, &m_tPacket);
		//}

		break;
	}
	case CS_CLIENT_READY:
	{
		if (eTmepGameType != GAME_ROOM) break;

		CS_Ready* pPacket = reinterpret_cast<CS_Ready*>(_pPacket);
		if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID))
		{
			cout << "CS_CLIENT_READY error" << endl;
			break;
		}

		if (pPacket->m_bIsHost)
		{
			if (CObjMgr::GetInstance()->CheckReadyByHost())
			{
				m_mutexPacketMgr.lock();
				reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetIsReady(true);
				m_mutexPacketMgr.unlock();

				CGameMgr::GetInstance()->SetGameType(GAME_LOADING);
			}
		}
		else
		{
			m_mutexPacketMgr.lock();
			bool bIsReady = !(reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->GetIsReady());
			reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetIsReady(bIsReady);
			m_mutexPacketMgr.unlock();

			//packet setting
			m_tReadyPacket.m_bIsAll = CObjMgr::GetInstance()->CheckReadyByHost();
			m_tReadyPacket.m_iID = _iID;
			m_tReadyPacket.m_bReady = bIsReady;

			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				SendPacket(iter->first, &m_tReadyPacket);
			}
		}
		break;
	}
	case CS_CLIENT_CHANGE:
	{
		if (eTmepGameType != GAME_ROOM) break;

		CS_Change* pPacket = reinterpret_cast<CS_Change*>(_pPacket);
		if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID))
		{
			cout << "CS_CLIENT_CHANGE error" << endl;
			break;
		}

		//index setting
		m_mutexPacketMgr.lock();
		int iRoomIndex = CGameMgr::GetInstance()->ChangeRoomIndex(_iID);
		reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetRoomIndex(iRoomIndex);
		m_mutexPacketMgr.unlock();

		//error check
		if (iRoomIndex == UNKNOWN_VALUE)
		{
			cout << "CClientMgr CS_CLIENT_CHANGE index error!" << endl;
			while (true);
		}

		//packet setting
		m_tChangePacket.m_iTeamIndex = iRoomIndex;
		m_tChangePacket.m_iID = _iID;

		//packet send to all
		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tChangePacket);
		}
		break;
	}
	case CS_OPTION_CHANGE:
	{
		if (eTmepGameType != GAME_ROOM) break;

		CS_OptionChange* pPacket = reinterpret_cast<CS_OptionChange*>(_pPacket);
		//host check
		if (pPacket->m_bIsHost)
		{
			if (pPacket->m_bDownMode || pPacket->m_bUpMode)
			{
				eGameModeType eTempMode = CGameMgr::GetInstance()->GetGameModeType();
				if (eTempMode == MODE_SOLO)	CGameMgr::GetInstance()->SetGameModeType(MODE_TEAM);
				else if (eTempMode == MODE_TEAM) CGameMgr::GetInstance()->SetGameModeType(MODE_SOLO);
			}

			if (pPacket->m_bCheckMap) CGameMgr::GetInstance()->SetMapType();

			if (pPacket->m_bDownBot) CGameMgr::GetInstance()->SetBotCount(false);
			else if (pPacket->m_bUpBot) CGameMgr::GetInstance()->SetBotCount(true);
		}

		//packet setting
		m_mutexPacketMgr.lock();
		switch (CGameMgr::GetInstance()->GetGameModeType())
		{
		case MODE_SOLO: m_tOptionChangePacket.m_bGameMode = false; break;
		case MODE_TEAM: m_tOptionChangePacket.m_bGameMode = true; break;
		}
		//maptype setting
		switch (CGameMgr::GetInstance()->GetMapType())
		{
		case MAP_KJK_PLAZA: m_tOptionChangePacket.m_bIsMapType = false; break;
		case MAP_KJK_POOL: m_tOptionChangePacket.m_bIsMapType = true; break;
		}
		m_tOptionChangePacket.m_iBotCount = CGameMgr::GetInstance()->GetBotCount();
		m_mutexPacketMgr.unlock();

		//packet send to all
		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tOptionChangePacket);
		}
		break;
	}
	case CS_CHAT_ROOM:
	{
		if (eTmepGameType != GAME_ROOM) break;

		//rendering chat
		CS_ChatRoom* pChatPacket = reinterpret_cast<CS_ChatRoom*>(_pPacket);
		cout << "<Room Chat>" << _iID << " : " << pChatPacket->m_szChat << endl;

		//수정필요
		//저장해놓는 대기방채팅창에 추가

		//chat resetting
		memcpy(m_tChatRoom.m_szChat, pChatPacket->m_szChat, sizeof(char) * MAX_CHAT);
		m_tChatRoom.m_iID = _iID;

		//packet send to all
		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tChatRoom);
		}
		break;
	}
	//game
	case CS_INPUT_KEY:
	{
		if (eTmepGameType != GAME_PLAY) break;

		CS_InputKey* pPacket = reinterpret_cast<CS_InputKey*>(_pPacket);
		if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID))
		{
			cout << "CS_INPUT_KEY error" << endl;
			break;
		}
		else reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetKey(pPacket->m_dwKey);

		//packet setting
		m_mutexPacketMgr.lock();
		m_tChangeStatePlayer.m_iPlayerID = _iID;
		m_tChangeStatePlayer.m_fPlayerAngleY = (*CObjMgr::GetInstance()->GetPlayerList())[_iID]->GetAngleY();
		m_tChangeStatePlayer.m_iPlayerKeyState = reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->GetKey();
		m_tChangeStatePlayer.m_vPlayerPos = (*CObjMgr::GetInstance()->GetPlayerList())[_iID]->GetPos();
		m_mutexPacketMgr.unlock();

		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tChangeStatePlayer);
		}
		break;
	}
	case CS_ANIMATION_END:
	{
		if (eTmepGameType != GAME_PLAY) break;

		CS_AnimationEnd* pPacket = reinterpret_cast<CS_AnimationEnd*>(_pPacket);
		if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID))
		{
			cout << "CS_ANIMATION_END error" << endl;
			break;
		}

		m_mutexPacketMgr.lock();
		reinterpret_cast<CPlayer*>((*CObjMgr::GetInstance()->GetPlayerList())[_iID])->SetAnimationEnd(true);
		m_mutexPacketMgr.unlock();
		break;
	}
	case CS_CHAT_GAME:
	{
		if (eTmepGameType != GAME_PLAY) break;

		CS_ChatGame* pChatPacket = reinterpret_cast<CS_ChatGame*>(_pPacket);
		if (!CObjMgr::GetInstance()->CheckPlayerConnect(_iID))
		{
			cout << "CS_CHAT_GAME error" << endl;
			break;
		}
		cout << "<Game Chat>" << _iID << " : " << pChatPacket->m_szChat << endl;

		//수정필요
		//저장해놓는 인게임채팅창에 추가

		//chat resetting
		memcpy(m_tChatGame.m_szChat, pChatPacket->m_szChat, sizeof(char) * MAX_CHAT);
		m_tChatGame.m_iID = _iID;
		//packet send to all
		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tChatGame);
		}
		break;
	}
	case CS_COLLISION_PLAYER_PLAYER:
	{
		if (eTmepGameType != GAME_PLAY) break;

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
		m_mutexPacketMgr.lock();
		if (!CObjMgr::GetInstance()->CheckPlayerConnect(pPacket->m_iTargetPlayerID)) m_mutexPacketMgr.unlock();
		else
		{
			if ((*CObjMgr::GetInstance()->GetPlayerList())[pPacket->m_iMyID]->GetObjState() == OBJ_STATE_DIE)
			{
				cout << "Attacking Player_" << pPacket->m_iTargetPlayerID << " is already die state!" << endl;
				m_mutexPacketMgr.unlock();
				break;
			}
			if ((*CObjMgr::GetInstance()->GetPlayerList())[pPacket->m_iTargetPlayerID]->GetObjState() == OBJ_STATE_DIE)
			{
				cout << "Attacked Player_" << pPacket->m_iTargetPlayerID << " is already die state!" << endl;
				m_mutexPacketMgr.unlock();
				break;
			}

			//change player's state
			((*CObjMgr::GetInstance()->GetPlayerList())[pPacket->m_iTargetPlayerID])->SetObjState(OBJ_STATE_DIE);
			m_mutexPacketMgr.unlock();
		}


		//died player's bead regen
		unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
		unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
		for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
		{
			int iBeadOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
			if (iBeadOwnerID == pPacket->m_iTargetPlayerID)
			{
				m_mutexPacketMgr.lock();
				reinterpret_cast<CBead*>(bead_iter->second)->SetOwnerID(UNKNOWN_VALUE);
				m_mutexPacketMgr.unlock();

				//packet setting
				m_tChangeStateBead.m_iOwnerPlayerID = UNKNOWN_VALUE;
				m_tChangeStateBead.m_iBeadID = bead_iter->first;
				//packet send to all
				unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					SendPacket(iter->first, &m_tChangeStateBead);
				}
			}
		}


		//packet setting
		m_tCheckCollision.m_iType = 1;//1.Player Player
		m_tCheckCollision.m_iMainID = pPacket->m_iMyID;
		m_tCheckCollision.m_iTargetID = pPacket->m_iTargetPlayerID;
		//packet send to all
		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tCheckCollision);
		}
		break;
	}
	case CS_COLLISION_PLAYER_BOT:
	{
		if (eTmepGameType != GAME_PLAY) break;

		CS_CollisionPlayerBot* pPacket = reinterpret_cast<CS_CollisionPlayerBot*>(_pPacket);
		if (pPacket->m_iMyID != _iID)
		{
			cout << "CS_COLLISION_PLAYER_BOT m_iMyID error" << endl;
			break;
		}

		//bot's state check
		m_mutexPacketMgr.lock();
		if ((*CObjMgr::GetInstance()->GetBotList())[pPacket->m_iBotID]->GetObjState() == OBJ_STATE_DIE)
		{
			cout << "Bot_" << pPacket->m_iBotID << " is already die state!" << endl;
			m_mutexPacketMgr.unlock();
			break;
		}
		//change bot's state
		(*CObjMgr::GetInstance()->GetBotList())[pPacket->m_iBotID]->SetObjState(OBJ_STATE_DIE);
		m_mutexPacketMgr.unlock();

		//packet setting
		m_tCheckCollision.m_iType = 2;//2.Player Bot
		m_tCheckCollision.m_iMainID = pPacket->m_iMyID;
		m_tCheckCollision.m_iTargetID = pPacket->m_iBotID;
		//packet send to all
		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tCheckCollision);
		}
		break;
	}
	case CS_COLLISION_PLAYER_BEAD:
	{
		if (eTmepGameType != GAME_PLAY) break;

		CS_CollisionPlayerBead* pPacket = reinterpret_cast<CS_CollisionPlayerBead*>(_pPacket);
		if (pPacket->m_iMyID != _iID)
		{
			cout << "CS_COLLISION_PLAYER_BEAD m_iMyID error" << endl;
			break;
		}

		//bead's state check
		m_mutexPacketMgr.lock();
		if (reinterpret_cast<CBead*>((*CObjMgr::GetInstance()->GetBeadList())[pPacket->m_iBeadID])->GetOwnerID() != UNKNOWN_VALUE)
		{
			cout << "Bead_" << pPacket->m_iBeadID << " is already die state!" << endl;
			m_mutexPacketMgr.unlock();
			break;
		}
		//change bead's state
		reinterpret_cast<CBead*>((*CObjMgr::GetInstance()->GetBeadList())[pPacket->m_iBeadID])->SetOwnerID(pPacket->m_iMyID);
		m_mutexPacketMgr.unlock();


		//packet setting
		m_tCheckCollision.m_iType = 3;//3.Player Bead
		m_tCheckCollision.m_iMainID = pPacket->m_iMyID;
		m_tCheckCollision.m_iTargetID = pPacket->m_iBeadID;
		//packet send to all
		unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
		unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
		for (iter; iter != iter_end; ++iter)
		{
			SendPacket(iter->first, &m_tCheckCollision);
		}
		break;
	}

	default:
		cout << "Unknown Packet Type!" << endl;
		return false;
	}
	return true;
}