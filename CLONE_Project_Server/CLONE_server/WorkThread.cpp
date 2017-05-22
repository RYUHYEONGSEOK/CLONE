#include "stdafx.h"
#include "WorkThread.h"

#include "ClientMgr.h"
#include "TimerQMgr.h"
#include "GameMgr.h"
#include "ObjMgr.h"

#include "MainServer.h"
#include "Protocol.h"

#include "Bot.h"
#include "Bead.h"

DWORD WINAPI WorkThread(void)
{
	while (true)
	{
		DWORD dwIOSize = 0;
		DWORD dwKey = 0;
		OVERLAPPED_EX* pOverEx;

		//thread -> waiting
		GetQueuedCompletionStatus(g_hIOCP, &dwIOSize, &dwKey, reinterpret_cast<LPOVERLAPPED*>(&pOverEx), INFINITE);

		//case error(disconnect)
		if (dwIOSize == 0)
		{
			cout << "Disconnected ID : " << dwKey << endl;

			//client erase
			CClientMgr::GetInstance()->EraseClient(dwKey);
			CClientMgr::GetInstance()->SetIsUseID(dwKey);

			//reset roomindex
			CGameMgr::GetInstance()->DisconnectRoomMember(dwKey);
			//setting GameMgr data
			CClientMgr::GetInstance()->ResetAllPlayerReady();


			//setting unready packet
			SC_AllUnready tUnReadyPacket;
			//packet send to all
			unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
			unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CClientMgr::GetInstance()->SendPacket(iter->first, &tUnReadyPacket);
			}

			//packet setting
			SC_RemoveClient tPacket;
			tPacket.m_iID = dwKey;

			if (CGameMgr::GetInstance()->GetHostID() == dwKey)
			{
				if (CClientMgr::GetInstance()->GetClientList()->size() > 0)
				{
					int iNewHostID = CClientMgr::GetInstance()->GetClientList()->begin()->first;
					CLIENT* pClient = CClientMgr::GetInstance()->GetClientList()->begin()->second;

					CGameMgr::GetInstance()->SetHostID(iNewHostID);

					g_GlobalMutex.lock();
					pClient->m_bIsHost = true;
					g_GlobalMutex.unlock();

					tPacket.m_iHostID = iNewHostID;
				}
				else
				{
					CGameMgr::GetInstance()->SetHostID(UNKNOWN_VALUE);
					continue;
				}
			}
			else
			{
				tPacket.m_iHostID = CGameMgr::GetInstance()->GetHostID();
			}
			//packet send to all
			iter = CClientMgr::GetInstance()->GetClientList()->begin();
			iter_end = CClientMgr::GetInstance()->GetClientList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
			}

			//case InGame
			if (CGameMgr::GetInstance()->GetGameType() == GAME_PLAY)
			{
				unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
				unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
				for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
				{
					int iBeadOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
					if (iBeadOwnerID == dwKey)
					{
						g_GlobalMutex.lock();
						reinterpret_cast<CBead*>(bead_iter->second)->SetOwnerID(UNKNOWN_VALUE);
						g_GlobalMutex.unlock();

						//packet setting
						SC_ChangeStateBead tBeadPacket;
						tBeadPacket.m_iBeadID = bead_iter->first;
						tBeadPacket.m_iOwnerPlayerID = UNKNOWN_VALUE;
						//packet send to all
						iter = CClientMgr::GetInstance()->GetClientList()->begin();
						iter_end = CClientMgr::GetInstance()->GetClientList()->end();
						for (iter; iter != iter_end; ++iter)
						{
							CClientMgr::GetInstance()->SendPacket(iter->first, &tBeadPacket);
						}
					}
				}
			}

			continue;
		}

		//operatino_type
		if (pOverEx->m_iOperationType == OP_SEND)
		{
			if (dwIOSize != pOverEx->wsaBuf.len)
			{
				cout << "Send Incomplete Error!" << endl;
				
				//diconnect client
				cout << "Disconnected ID : " << dwKey << endl;

				//client erase
				CClientMgr::GetInstance()->EraseClient(dwKey);
				CClientMgr::GetInstance()->SetIsUseID(dwKey);

				//reset roomindex
				CGameMgr::GetInstance()->DisconnectRoomMember(dwKey);
				//setting GameMgr data
				CClientMgr::GetInstance()->ResetAllPlayerReady();


				//setting unready packet
				SC_AllUnready tUnReadyPacket;
				//packet send to all
				unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
				unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CClientMgr::GetInstance()->SendPacket(iter->first, &tUnReadyPacket);
				}

				//packet setting
				SC_RemoveClient tPacket;
				tPacket.m_iID = dwKey;

				if (CGameMgr::GetInstance()->GetHostID() == dwKey)
				{
					if (CClientMgr::GetInstance()->GetClientList()->size() > 0)
					{
						int iNewHostID = CClientMgr::GetInstance()->GetClientList()->begin()->first;
						CLIENT* pClient = CClientMgr::GetInstance()->GetClientList()->begin()->second;

						CGameMgr::GetInstance()->SetHostID(iNewHostID);

						g_GlobalMutex.lock();
						pClient->m_bIsHost = true;
						g_GlobalMutex.unlock();

						tPacket.m_iHostID = iNewHostID;
					}
					else
					{
						CGameMgr::GetInstance()->SetHostID(UNKNOWN_VALUE);
						continue;
					}
				}
				else
				{
					tPacket.m_iHostID = CGameMgr::GetInstance()->GetHostID();
				}
				//packet send to all
				iter = CClientMgr::GetInstance()->GetClientList()->begin();
				iter_end = CClientMgr::GetInstance()->GetClientList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
				}

				//case InGame
				if (CGameMgr::GetInstance()->GetGameType() == GAME_PLAY)
				{
					unordered_map<int, CObj*>::iterator bead_iter = CObjMgr::GetInstance()->GetBeadList()->begin();
					unordered_map<int, CObj*>::iterator bead_iter_end = CObjMgr::GetInstance()->GetBeadList()->end();
					for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
					{
						int iBeadOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
						if (iBeadOwnerID == dwKey)
						{
							g_GlobalMutex.lock();
							reinterpret_cast<CBead*>(bead_iter->second)->SetOwnerID(UNKNOWN_VALUE);
							g_GlobalMutex.unlock();

							//packet setting
							SC_ChangeStateBead tBeadPacket;
							tBeadPacket.m_iBeadID = bead_iter->first;
							tBeadPacket.m_iOwnerPlayerID = UNKNOWN_VALUE;
							//packet send to all
							iter = CClientMgr::GetInstance()->GetClientList()->begin();
							iter_end = CClientMgr::GetInstance()->GetClientList()->end();
							for (iter; iter != iter_end; ++iter)
							{
								CClientMgr::GetInstance()->SendPacket(iter->first, &tBeadPacket);
							}
						}
					}
				}
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
				if (iPacketSize == 0)
				{
					iPacketSize = reinterpret_cast<HEADER*>(pTempBuf)->size;
				}

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
					CClientMgr::GetInstance()->ProcessPacket(pOverEx->m_szPacketBuf, dwKey);

					//clear
					iRestSize -= iRemainData;
					iPacketSize = 0;
					pOverEx->m_iPrevReceived = 0;
					pTempBuf += iRemainData;
				}
			}

			//find client socket
			CLIENT* pTempClient = CClientMgr::GetInstance()->FindClient(dwKey);

			//wsarecv
			DWORD dwRecvFlag = 0;
			WSARecv(pTempClient->m_tSock, &(pOverEx->wsaBuf), 1, nullptr, &dwRecvFlag, &(pOverEx->m_tOverLapped), nullptr);
		}
		else if (pOverEx->m_iOperationType == OP_END) {}
		else if (pOverEx->m_iOperationType == OP_EVENT_COUNTDOWN)
		{
			//timerQ registry( 3 -> 2 -> 1 -> 0 )
			if (dwKey > 0)
			{
				CTimerQMgr::GetInstance()->AddTimerQ(EVENT_COUNTDOWN, 1.f, dwKey - 1);
			}
			else if (dwKey == 0)
			{
				//packet setting
				SC_GameState tPacket;
				tPacket.m_iGameState = STAGE_PLAY;
				tPacket.m_bGameWinOrLose = true;

				//packet send to all
				unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
				unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
				}

				CGameMgr::GetInstance()->SetStageStateType(STAGE_PLAY);
			}

			//packet setting
			SC_CountDown tPacket;
			tPacket.m_iCountDownNum = dwKey;

			//packet send to all
			unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
			unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
			}
		}
		else if (pOverEx->m_iOperationType == OP_EVENT_BOT_MOVE)
		{
			if (CGameMgr::GetInstance()->GetStageStateType() != STAGE_PLAY)
				continue;

			g_GlobalMutex.lock();
			CObj* pBot = (*CObjMgr::GetInstance()->GetBotList())[dwKey];
			g_GlobalMutex.unlock();

			if (pBot->GetObjState() == OBJ_STATE_DIE)
				continue;

			//packet setting
			SC_ChangeStateBot tPacket;
			tPacket.m_iBotID = dwKey;
			tPacket.m_iBotState = OBJ_STATE_MOVE;
			int iRandTime = rand() % 5 + 6;	//6~10
			tPacket.m_iBotTimeCount = iRandTime;
			float fRandRadious = D3DXToRadian((float)(rand() % 360));
			tPacket.m_fAngleY = fRandRadious;
			tPacket.m_vBotPos = pBot->GetPos();

			//packet send to all
			unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
			unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
			}


			//server's bot setting
			g_GlobalMutex.lock();
			reinterpret_cast<CBot*>(pBot)->SetState(OBJ_STATE_MOVE, iRandTime, fRandRadious);
			g_GlobalMutex.unlock();


			//timerQ registry
			int iRandNum = rand() % 6 + 13; //13~18
			CTimerQMgr::GetInstance()->AddTimerQ(EVENT_BOT_MOVE, (float)iRandNum, dwKey);
		}
		else if (pOverEx->m_iOperationType == OP_EVENT_MOVE_ROOM)
		{
			//packet setting
			SC_GameState tPacket;
			tPacket.m_iGameState = STAGE_READY;
			tPacket.m_bGameWinOrLose = true;	//in this situation, Don't using this flag

			//packet send to all
			unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
			unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
			for (iter; iter != iter_end; ++iter)
			{
				CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
			}

			//ObjMgr clear
			CObjMgr::GetInstance()->Clear();

			//GameState ready
			CGameMgr::GetInstance()->SetStageStateType(STAGE_READY);

			//timerQ Clear
			CTimerQMgr::GetInstance()->ClearTimerQ();

			cout << "Reset & Go to WaitingRoom" << endl;
			cout << "=========================" << endl;
		}
	}

	return 0;
}