#include "stdafx.h"
#include "LogicThread.h"

#include "GameMgr.h"
#include "ObjMgr.h"
#include "PacketMgr.h"
#include "TimeMgr.h"

#include "Protocol.h"

DWORD WINAPI LogicThread(void)
{
	//game time setting
	int iGameTime = PLAY_TIME;
	auto m_ptExGameTime = high_resolution_clock::now();

	//timeMgr setting
	CTimeMgr::GetInstance()->InitTime();

	while (true)
	{
		g_GlobalMutex.lock();
		eGameType eNowGameType = CGameMgr::GetInstance()->GetGameType();
		eStageType eNowStageType = CGameMgr::GetInstance()->GetStageStateType();
		eGameModeType eNowGameModeType = CGameMgr::GetInstance()->GetGameModeType();
		g_GlobalMutex.unlock();
		
		//game state check
		if (eNowGameType == GAME_PLAY && eNowStageType == STAGE_PLAY)
		{
			//connected client count is 0
			g_GlobalMutex.lock();
			if (CObjMgr::GetInstance()->GetPlayerList()->size() <= 0)
			{
				iGameTime = PLAY_TIME;
				g_GlobalMutex.unlock();

				CGameMgr::GetInstance()->SetGameType(GAME_ROOM);
				continue;
			}
			else g_GlobalMutex.unlock();


			//player count
			g_GlobalMutex.lock();
			if (CObjMgr::GetInstance()->CheckPlayerState(eNowGameModeType))
			{
				iGameTime = PLAY_TIME;
				g_GlobalMutex.unlock();

				CGameMgr::GetInstance()->SetGameType(GAME_ROOM);
				continue;
			}
			else g_GlobalMutex.unlock();


			//bead count
			g_GlobalMutex.lock();
			if (CGameMgr::GetInstance()->GetMapType() == MAP_KJK_POOL) g_GlobalMutex.unlock();
			else
			{
				if (CObjMgr::GetInstance()->CheckAllBeadHaveOwner())
				{
					iGameTime = PLAY_TIME;
					g_GlobalMutex.unlock();

					CGameMgr::GetInstance()->SetGameType(GAME_ROOM);
					continue;
				}
				else g_GlobalMutex.unlock();
			}


			//calculate frame time
			g_GlobalMutex.lock();
			CTimeMgr::GetInstance()->SetTime();
			float fFrameTime = CTimeMgr::GetInstance()->GetTime();
			g_GlobalMutex.unlock();


			//ObjMgr Progress
			CObjMgr::GetInstance()->Progress(fFrameTime);


			//per 1 sec
			g_GlobalMutex.lock();
			if (m_ptExGameTime + 1s < high_resolution_clock::now())
			{
				m_ptExGameTime = high_resolution_clock::now();
				iGameTime -= 1;
				g_GlobalMutex.unlock();

				//packet setting
				SC_ServerGameTime tPacket;
				tPacket.m_iTime = iGameTime;
				//packet send to all
				unordered_map<int, CObj*>::iterator iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
				unordered_map<int, CObj*>::iterator iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
				for (iter; iter != iter_end; ++iter)
				{
					CPacketMgr::GetInstance()->SendPacket(iter->first, &tPacket);
				}

				g_GlobalMutex.lock();
				if (iGameTime == 0)
				{
					iGameTime = PLAY_TIME;
					g_GlobalMutex.unlock();

					CGameMgr::GetInstance()->SetGameType(GAME_ROOM);
					continue;
				}
				else g_GlobalMutex.unlock();
			}
			else g_GlobalMutex.unlock();
		}
		else if(iGameTime != PLAY_TIME) iGameTime = PLAY_TIME;
	}

	//destroy timeMgr
	CTimeMgr::DestroyInstance();

	return 0;
}