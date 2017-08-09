#include "stdafx.h"
#include "WorkerThread.h"

#include "PacketMgr.h"
#include "TimerQMgr.h"
#include "GameMgr.h"
#include "ObjMgr.h"

#include "MainServer.h"
#include "Protocol.h"

#include "Player.h"
#include "Bot.h"
#include "Bead.h"

DWORD WINAPI WorkerThread(void)
{
	while (true)
	{
		DWORD dwIOSize = 0;
		unsigned long long ullID;
		OVERLAPPED_EX* pOverEx;

		//thread -> waiting
		BOOL bIsResult = GetQueuedCompletionStatus(g_hIOCP, &dwIOSize, &ullID, reinterpret_cast<LPOVERLAPPED*>(&pOverEx), INFINITE);
		if (FALSE == bIsResult)
		{
			int iErrNo = WSAGetLastError();
			if (64 == iErrNo)
			{
				//client disconnect
				CObjMgr::GetInstance()->DisconnectPlayer((int)ullID);

				//reset roomindex & ResetAllPlayerReady
				CGameMgr::GetInstance()->DisconnectRoomMember((int)ullID);
				CObjMgr::GetInstance()->ResetAllPlayerReady();

				//setting unready packet
				SC_AllUnready tUnReadyPacket;
				//packet send to all
				unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CPacketMgr::GetInstance()->SendPacket(iter->first, &tUnReadyPacket);
				}

				g_GlobalMutex.lock();
				size_t stPlayerCount = CObjMgr::GetInstance()->GetPlayerList()->size();
				unsigned long long ullHostID = (unsigned long long)(CGameMgr::GetInstance()->GetHostID());
				g_GlobalMutex.unlock();

				//packet setting
				SC_RemoveClient tRemovePacket;
				tRemovePacket.m_iID = (int)ullID;
				if (ullHostID != ullID) tRemovePacket.m_iHostID = (int)ullHostID;
				else
				{
					if (stPlayerCount > 0)
					{
						g_GlobalMutex.lock();
						unordered_map<int, CObj*>::iterator iterHost = CObjMgr::GetInstance()->GetPlayerList()->begin();
						reinterpret_cast<CPlayer*>(iterHost->second)->SetIsHost(true);
						g_GlobalMutex.unlock();

						tRemovePacket.m_iHostID = iterHost->first;

						CGameMgr::GetInstance()->SetHostID(iterHost->first);
					}
					else
					{
						CGameMgr::GetInstance()->SetHostID(UNKNOWN_VALUE);
						continue;
					}
				}
				//packet send to all
				unordered_map<int, CObj*>::iterator iterRemovePacket = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iterRemovePacket_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iterRemovePacket; iterRemovePacket != iterRemovePacket_end; ++iterRemovePacket)
				{
					CPacketMgr::GetInstance()->SendPacket(iterRemovePacket->first, &tRemovePacket);
				}

				//case InGame
				g_GlobalMutex.lock();
				if (CGameMgr::GetInstance()->GetGameType() == GAME_PLAY)
				{
					g_GlobalMutex.unlock();

					unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
					unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
					for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
					{
						g_GlobalMutex.lock();
						int iBeadOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
						if (iBeadOwnerID == ullID)
						{
							reinterpret_cast<CBead*>(bead_iter->second)->SetOwnerID(UNKNOWN_VALUE);
							g_GlobalMutex.unlock();

							//packet setting
							SC_ChangeStateBead tBeadPacket;
							tBeadPacket.m_iBeadID = bead_iter->first;
							tBeadPacket.m_iOwnerPlayerID = UNKNOWN_VALUE;
							//packet send to all
							unordered_map<int, CObj*>::iterator iterChangeStateBeadPacket = CObjMgr::GetInstance()->GetPlayerList()->begin();
							unordered_map<int, CObj*>::iterator iterChangeStateBeadPacket_end = CObjMgr::GetInstance()->GetPlayerList()->end();
							for (iterChangeStateBeadPacket; iterChangeStateBeadPacket != iterChangeStateBeadPacket_end; ++iterChangeStateBeadPacket)
							{
								CPacketMgr::GetInstance()->SendPacket(iterChangeStateBeadPacket->first, &tBeadPacket);
							}
						}
						else g_GlobalMutex.unlock();
					}
				}
				else g_GlobalMutex.unlock();
				continue;
			}
			cout << "WorkerThread GetQueuedCompletionStatus error" << endl;
			while (true);
		}
		//case error(disconnect)
		if (dwIOSize == 0)
		{
			//client disconnect
			CObjMgr::GetInstance()->DisconnectPlayer((int)ullID);

			//reset roomindex & ResetAllPlayerReady
			CGameMgr::GetInstance()->DisconnectRoomMember((int)ullID);
			CObjMgr::GetInstance()->ResetAllPlayerReady();
			
			//setting unready packet
			SC_AllUnready tUnReadyPacket;
			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CPacketMgr::GetInstance()->SendPacket(iter->first, &tUnReadyPacket);
			}

			g_GlobalMutex.lock();
			size_t stPlayerCount = CObjMgr::GetInstance()->GetPlayerList()->size();
			unsigned long long ullHostID = (unsigned long long)(CGameMgr::GetInstance()->GetHostID());
			g_GlobalMutex.unlock();

			//packet setting
			SC_RemoveClient tRemovePacket;
			tRemovePacket.m_iID = (int)ullID;
			if (ullHostID != ullID) tRemovePacket.m_iHostID = (int)ullHostID;
			else
			{
				if (stPlayerCount > 0)
				{
					g_GlobalMutex.lock();
					unordered_map<int, CObj*>::iterator iterHost = CObjMgr::GetInstance()->GetPlayerList()->begin();
					reinterpret_cast<CPlayer*>(iterHost->second)->SetIsHost(true);
					g_GlobalMutex.unlock();

					tRemovePacket.m_iHostID = iterHost->first;

					CGameMgr::GetInstance()->SetHostID(iterHost->first);
				}
				else
				{
					CGameMgr::GetInstance()->SetHostID(UNKNOWN_VALUE);
					continue;
				}
			}
			//packet send to all
			unordered_map<int, CObj*>::iterator iterRemovePacket = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iterRemovePacket_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iterRemovePacket; iterRemovePacket != iterRemovePacket_end; ++iterRemovePacket)
			{
				CPacketMgr::GetInstance()->SendPacket(iterRemovePacket->first, &tRemovePacket);
			}

			//case InGame
			g_GlobalMutex.lock();
			if (CGameMgr::GetInstance()->GetGameType() == GAME_PLAY)
			{
				g_GlobalMutex.unlock();

				unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
				unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
				for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
				{
					g_GlobalMutex.lock();
					int iBeadOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
					if (iBeadOwnerID == ullID)
					{
						reinterpret_cast<CBead*>(bead_iter->second)->SetOwnerID(UNKNOWN_VALUE);
						g_GlobalMutex.unlock();

						//packet setting
						SC_ChangeStateBead tBeadPacket;
						tBeadPacket.m_iBeadID = bead_iter->first;
						tBeadPacket.m_iOwnerPlayerID = UNKNOWN_VALUE;
						//packet send to all
						unordered_map<int, CObj*>::iterator iterChangeStateBeadPacket = CObjMgr::GetInstance()->GetPlayerList()->begin();
						unordered_map<int, CObj*>::iterator iterChangeStateBeadPacket_end = CObjMgr::GetInstance()->GetPlayerList()->end();
						for (iterChangeStateBeadPacket; iterChangeStateBeadPacket != iterChangeStateBeadPacket_end; ++iterChangeStateBeadPacket)
						{
							CPacketMgr::GetInstance()->SendPacket(iterChangeStateBeadPacket->first, &tBeadPacket);
						}
					}
					else g_GlobalMutex.unlock();
				}
			}
			else g_GlobalMutex.unlock();
			continue;
		}

		//operatino_type
		if (pOverEx->m_iOperationType == OP_SEND)
		{
			if (dwIOSize != pOverEx->wsaBuf.len)
			{
				cout << "Send Incomplete Error!" << endl;

				//client disconnect
				CObjMgr::GetInstance()->DisconnectPlayer((int)ullID);

				//reset roomindex & ResetAllPlayerReady
				CGameMgr::GetInstance()->DisconnectRoomMember((int)ullID);
				CObjMgr::GetInstance()->ResetAllPlayerReady();

				//setting unready packet
				SC_AllUnready tUnReadyPacket;
				//packet send to all
				unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CPacketMgr::GetInstance()->SendPacket(iter->first, &tUnReadyPacket);
				}

				g_GlobalMutex.lock();
				size_t stPlayerCount = CObjMgr::GetInstance()->GetPlayerList()->size();
				unsigned long long ullHostID = (unsigned long long)(CGameMgr::GetInstance()->GetHostID());
				g_GlobalMutex.unlock();

				//packet setting
				SC_RemoveClient tRemovePacket;
				tRemovePacket.m_iID = (int)ullID;
				if (ullHostID != ullID) tRemovePacket.m_iHostID = (int)ullHostID;
				else
				{
					if (stPlayerCount > 0)
					{
						g_GlobalMutex.lock();
						unordered_map<int, CObj*>::iterator iterHost = CObjMgr::GetInstance()->GetPlayerList()->begin();
						reinterpret_cast<CPlayer*>(iterHost->second)->SetIsHost(true);
						g_GlobalMutex.unlock();

						tRemovePacket.m_iHostID = iterHost->first;

						CGameMgr::GetInstance()->SetHostID(iterHost->first);
					}
					else
					{
						CGameMgr::GetInstance()->SetHostID(UNKNOWN_VALUE);
						delete pOverEx;
						continue;
					}
				}
				//packet send to all
				unordered_map<int, CObj*>::iterator iterRemovePacket = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iterRemovePacket_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iterRemovePacket; iterRemovePacket != iterRemovePacket_end; ++iterRemovePacket)
				{
					CPacketMgr::GetInstance()->SendPacket(iterRemovePacket->first, &tRemovePacket);
				}

				//case InGame
				g_GlobalMutex.lock();
				if (CGameMgr::GetInstance()->GetGameType() == GAME_PLAY)
				{
					g_GlobalMutex.unlock();

					unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
					unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
					for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
					{
						g_GlobalMutex.lock();
						int iBeadOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
						if (iBeadOwnerID == ullID)
						{
							reinterpret_cast<CBead*>(bead_iter->second)->SetOwnerID(UNKNOWN_VALUE);
							g_GlobalMutex.unlock();

							//packet setting
							SC_ChangeStateBead tBeadPacket;
							tBeadPacket.m_iBeadID = bead_iter->first;
							tBeadPacket.m_iOwnerPlayerID = UNKNOWN_VALUE;
							//packet send to all
							unordered_map<int, CObj*>::iterator iterChangeStateBeadPacket = CObjMgr::GetInstance()->GetPlayerList()->begin();
							unordered_map<int, CObj*>::iterator iterChangeStateBeadPacket_end = CObjMgr::GetInstance()->GetPlayerList()->end();
							for (iterChangeStateBeadPacket; iterChangeStateBeadPacket != iterChangeStateBeadPacket_end; ++iterChangeStateBeadPacket)
							{
								CPacketMgr::GetInstance()->SendPacket(iterChangeStateBeadPacket->first, &tBeadPacket);
							}
						}
						else g_GlobalMutex.unlock();
					}
				}
				else g_GlobalMutex.unlock();
			}
			delete pOverEx;
		}
		else if (pOverEx->m_iOperationType == OP_RECV)
		{
			int iRestSize = dwIOSize;
			int iPacketSize = pOverEx->m_iCurPacketSize;
			char* pTempBuf = pOverEx->m_szIOCPbuf;

			while (iRestSize > 0)
			{
				//datasize is 0
				if (iPacketSize == 0) iPacketSize = reinterpret_cast<HEADER*>(pTempBuf)->size;
				int iRemainData = iPacketSize - pOverEx->m_iPrevReceived;

				//error(lack buffer)
				if (iRemainData > iRestSize)
				{
					memcpy(pOverEx->m_szPacketBuf + pOverEx->m_iPrevReceived, pTempBuf, iRestSize);
					pOverEx->m_iPrevReceived += iRestSize;
				}
				//ok(making packet)
				else
				{
					memcpy(pOverEx->m_szPacketBuf + pOverEx->m_iPrevReceived, pTempBuf, iRemainData);

					//processs packet -> process data
					CPacketMgr::GetInstance()->ProcessPacket(pOverEx->m_szPacketBuf, (int)ullID);

					//clear
					iRestSize -= iRemainData;
					iPacketSize = 0;
					pOverEx->m_iPrevReceived = 0;
					pTempBuf += iRemainData;
				}
			}

			//check connecting
			if (!CObjMgr::GetInstance()->CheckPlayerConnect((int)ullID))
			{
				delete pOverEx;
				continue;
			}
			
			//wsarecv
			CObj* pPlayer = (*CObjMgr::GetInstance()->GetPlayerList())[(int)ullID];
			DWORD dwRecvFlag = 0;
			WSARecv(reinterpret_cast<CPlayer*>(pPlayer)->GetSocket(), &(pOverEx->wsaBuf), 1, nullptr, &dwRecvFlag, &(pOverEx->m_tOverLapped), nullptr);
		}
		else if (pOverEx->m_iOperationType == OP_END) {}
		else if (pOverEx->m_iOperationType == OP_EVENT_COUNTDOWN)
		{
			//timerQ registry( 3 -> 2 -> 1 -> 0 )
			if (ullID > 0)
			{
				CTimerQMgr::GetInstance()->AddTimerQ(EVENT_COUNTDOWN, high_resolution_clock::now() + 1s, (int)ullID - 1);
			}
			else if (ullID == 0)
			{
				//packet setting
				SC_GameState tPacket;
				tPacket.m_iGameState = STAGE_PLAY;
				tPacket.m_bGameWinOrLose = true;

				//packet send to all
				unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CPacketMgr::GetInstance()->SendPacket(iter->first, &tPacket);
				}

				CGameMgr::GetInstance()->SetStageStateType(STAGE_PLAY);
			}

			//packet setting
			SC_CountDown tPacket;
			tPacket.m_iCountDownNum = (int)ullID;

			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CPacketMgr::GetInstance()->SendPacket(iter->first, &tPacket);
			}

			
			delete pOverEx;
		}
		else if (pOverEx->m_iOperationType == OP_EVENT_BOT_MOVE)
		{
			if (CGameMgr::GetInstance()->GetStageStateType() != STAGE_PLAY)
			{
				delete pOverEx;
				continue;
			}

			g_GlobalMutex.lock();
			CObj* pBot = (*CObjMgr::GetInstance()->GetBotList())[(int)ullID];
			g_GlobalMutex.unlock();

			if (pBot->GetObjState() == OBJ_STATE_DIE)
			{
				delete pOverEx;
				continue;
			}

			//packet setting
			SC_ChangeStateBot tPacket;
			tPacket.m_iBotID = (int)ullID;
			tPacket.m_iBotState = OBJ_STATE_MOVE;
			int iRandTime = rand() % 5 + 6;	//6~10
			tPacket.m_iBotTimeCount = iRandTime;
			float fRandRadious = (float)D3DXToRadian((float)(rand() % 360));
			tPacket.m_fAngleY = fRandRadious;
			tPacket.m_vBotPos = pBot->GetPos();

			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CPacketMgr::GetInstance()->SendPacket(iter->first, &tPacket);
			}
			
			//server's bot setting
			g_GlobalMutex.lock();
			reinterpret_cast<CBot*>(pBot)->SetState(OBJ_STATE_MOVE, iRandTime, fRandRadious);
			g_GlobalMutex.unlock();

			//timerQ registry next bot's moving -> 13~18
			CTimerQMgr::GetInstance()->AddTimerQ(EVENT_BOT_MOVE, high_resolution_clock::now() + (seconds)(rand() % 6 + 13), (int)ullID);

			
			delete pOverEx;
		}
		else if (pOverEx->m_iOperationType == OP_EVENT_SYNC_OBJ)
		{
			g_GlobalMutex.lock();
			if (CGameMgr::GetInstance()->GetStageStateType() != STAGE_PLAY)
			{
				g_GlobalMutex.unlock();
				delete pOverEx;
				continue;
			}
			else g_GlobalMutex.unlock();

			
			g_GlobalMutex.lock();

			//player sync
			SC_SyncPlayer tSyncPlayerPacket;
			for (int i = 0; i < MAX_USER; ++i)
			{
				tSyncPlayerPacket.m_fPlayerAngleY[i] = -1.f;
				tSyncPlayerPacket.m_vPlayerPos[i].y = 1.f;
			}
			unordered_map<int, CObj*>::iterator player_iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator player_iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (player_iter; player_iter != player_iter_end; ++player_iter)
			{
				tSyncPlayerPacket.m_fPlayerAngleY[player_iter->first] = player_iter->second->GetAngleY();
				tSyncPlayerPacket.m_vPlayerPos[player_iter->first] = player_iter->second->GetPos();
			}

			//bot sync
			SC_SyncBot tSyncBotPacket;
			for (int i = 0; i < MAX_BOT_COUNT; ++i) tSyncBotPacket.m_vBotPos[i].y = 1.f;

			int iBotCount = 0;
			unordered_map<int, CObj*>::iterator bot_iter = CObjMgr::GetInstance()->GetBotList()->begin();
			unordered_map<int, CObj*>::iterator bot_iter_end = CObjMgr::GetInstance()->GetBotList()->end();
			for (bot_iter; bot_iter != bot_iter_end; ++bot_iter)
			{
				tSyncBotPacket.m_fBotAngleY[bot_iter->first] = bot_iter->second->GetAngleY();
				tSyncBotPacket.m_vBotPos[bot_iter->first] = bot_iter->second->GetPos();
				++iBotCount;
			}
			tSyncBotPacket.m_iBotCount = iBotCount;

			g_GlobalMutex.unlock();

			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CPacketMgr::GetInstance()->SendPacket(iter->first, &tSyncBotPacket);
				CPacketMgr::GetInstance()->SendPacket(iter->first, &tSyncPlayerPacket);
			}

			//timerQ registry next sync 10 packet per 1 second
			CTimerQMgr::GetInstance()->AddTimerQ(EVENT_SYNC_OBJ, high_resolution_clock::now() + 1ms, (int)ullID);

			
			delete pOverEx;
		}
		else if (pOverEx->m_iOperationType == OP_EVENT_MOVE_ROOM)
		{
			//packet setting
			SC_GameState tPacket;
			tPacket.m_iGameState = STAGE_READY;
			tPacket.m_bGameWinOrLose = true;	//in this situation, Don't using this flag
												//when using this flag, STAGE_RESULT

			//packet send to all
			unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
			unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CPacketMgr::GetInstance()->SendPacket(iter->first, &tPacket);
			}

			//ObjMgr clear
			CObjMgr::GetInstance()->ClearObject();

			//GameState ready
			CGameMgr::GetInstance()->SetStageStateType(STAGE_READY);

			//timerQ Clear
			CTimerQMgr::GetInstance()->ClearTimerQ();

			cout << "Reset & Go to WaitingRoom" << endl;
			cout << "=========================" << endl;

			
			delete pOverEx;
		}
	}

	return 0;
}