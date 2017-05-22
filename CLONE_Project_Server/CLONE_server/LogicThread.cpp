#include "stdafx.h"
#include "LogicThread.h"

#include "GameTimer.h"

#include "GameMgr.h"
#include "ClientMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"

#include "Protocol.h"

DWORD WINAPI LogicThread(void)
{
	CGameTimer cTimer[TIMER_END];

	//game time setting
	g_GlobalMutex.lock();
	int iGameTime = PLAY_TIME;
	g_GlobalMutex.unlock();

	while (true)
	{
		//timeMgr setting
		g_GlobalMutex.lock();
		CTimeMgr::GetInstance()->SetTime();
		g_GlobalMutex.unlock();
		
		//game state check
		if ((CGameMgr::GetInstance()->GetGameType() == GAME_PLAY)
			&& (CGameMgr::GetInstance()->GetStageStateType() == STAGE_PLAY))
		{
			//client count
			g_GlobalMutex.lock();
			int iClientCount = CClientMgr::GetInstance()->GetClientList()->size();
			g_GlobalMutex.unlock();
			if (iClientCount <= 0)
			{
				CGameMgr::GetInstance()->SetGameType(GAME_ROOM);
				g_GlobalMutex.lock();
				iGameTime = PLAY_TIME;
				g_GlobalMutex.unlock();

				cTimer[TIMER_TIME].Reset();
				cTimer[TIMER_SYNC].Reset();
				continue;
			}

			//bead count
			if (CObjMgr::GetInstance()->CheckBeadHaveOwner())
			{
				CGameMgr::GetInstance()->SetGameType(GAME_ROOM);
				g_GlobalMutex.lock();
				iGameTime = PLAY_TIME;
				g_GlobalMutex.unlock();

				cTimer[TIMER_TIME].Reset();
				cTimer[TIMER_SYNC].Reset();
				continue;
			}

			//game logic
			cTimer[TIMER_TIME].Tick();
			cTimer[TIMER_SYNC].Tick();

			//per 1 sec
			if (cTimer[TIMER_TIME].TotalTime() > 1.f)
			{
				g_GlobalMutex.lock();
				iGameTime -= 1;
				g_GlobalMutex.unlock();

				//packet setting
				SC_ServerGameTime tPacket;
				tPacket.m_iTime = iGameTime;

				//packet send to all
				unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
				unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CClientMgr::GetInstance()->SendPacket(iter->first, &tPacket);
				}

				//timer reset
				cTimer[TIMER_TIME].Reset();

				if (iGameTime <= 0)
				{
					CGameMgr::GetInstance()->SetGameType(GAME_ROOM);
					g_GlobalMutex.lock();
					iGameTime = PLAY_TIME;
					g_GlobalMutex.unlock();
				}
			}

			//timeMgr setting
			g_GlobalMutex.lock();
			float fFrameTime = CTimeMgr::GetInstance()->GetTime();
			g_GlobalMutex.unlock();

			//ObjMgr Progress
			CObjMgr::GetInstance()->Progress(fFrameTime);

			//4 frame -> pos sync
			if (cTimer[TIMER_SYNC].TotalTime() > 0.25f)
			{
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
				for (int i = 0; i < MAX_BOT_COUNT; ++i)
				{
					tSyncBotPacket.m_vBotPos[i].y = 1.f;
				}
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


				//packet send to all
				unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
				unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CClientMgr::GetInstance()->SendPacket(iter->first, &tSyncBotPacket);
					CClientMgr::GetInstance()->SendPacket(iter->first, &tSyncPlayerPacket);
				}

				//timer reset
				cTimer[TIMER_SYNC].Reset();
			}
		}
		else
		{
			g_GlobalMutex.lock();
			iGameTime = PLAY_TIME;
			g_GlobalMutex.unlock();

			cTimer[TIMER_TIME].Reset();
			cTimer[TIMER_SYNC].Reset();
		}
	}

	return 0;
}