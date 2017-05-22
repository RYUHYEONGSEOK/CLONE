#include "stdafx.h"
#include "ObjMgr.h"

#include "Player.h"
#include "Bot.h"
#include "Bead.h"

#include "NaviMgr.h"
#include "GameMgr.h"
#include "ClientMgr.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
	ZeroMemory(m_vReserveBeadPos, sizeof(D3DXVECTOR3) * 8);
	ZeroMemory(m_pHavingBeadPerPlayer, sizeof(int) * MAX_USER);
}
CObjMgr::~CObjMgr()
{
}

void CObjMgr::ReserveBeadPos(void)
{
	m_vReserveBeadPos[0] = D3DXVECTOR3(3.f - HALFMAPX, 1.f, 3.f - HALFMAPZ);
	m_vReserveBeadPos[1] = D3DXVECTOR3(5.5f - HALFMAPX, 1.f, 70.f - HALFMAPZ);
	m_vReserveBeadPos[2] = D3DXVECTOR3(35.f - HALFMAPX, 1.f, 47.f - HALFMAPZ);
	m_vReserveBeadPos[3] = D3DXVECTOR3(60.f - HALFMAPX, 1.f, 67.f - HALFMAPZ);
	m_vReserveBeadPos[4] = D3DXVECTOR3(92.f - HALFMAPX, 1.f, 4.f - HALFMAPZ);
	m_vReserveBeadPos[5] = D3DXVECTOR3(107.f - HALFMAPX, 1.f, 30.f - HALFMAPZ);
	m_vReserveBeadPos[6] = D3DXVECTOR3(150.f - HALFMAPX, 1.f, 10.f - HALFMAPZ);
	m_vReserveBeadPos[7] = D3DXVECTOR3(120.f - HALFMAPX, 1.f, 72.f - HALFMAPZ);
}
void CObjMgr::SetPlayerPos(void)
{
	m_ObjMgrMutex.lock();
	unordered_map<int, CLIENT*>::iterator iter = CClientMgr::GetInstance()->GetClientList()->begin();
	unordered_map<int, CLIENT*>::iterator iter_end = CClientMgr::GetInstance()->GetClientList()->end();
	m_ObjMgrMutex.unlock();

	//for (iter; iter != iter_end; ++iter)
	//{
	//	D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	//	DWORD dwIdxCheck = UNKNOWN_VALUE;
	//	bool bPosCheck = false;

	//	while (!bPosCheck)
	//	{
	//		int iPlus_Minus_X = rand() % 2;
	//		int iPlus_Minus_Y = rand() % 2;
	//		float fX = (float)(rand() % 78);
	//		float fZ = (float)(rand() % 38);
	//		if (iPlus_Minus_X == 0)		// 0 == +   ,   1 == -
	//		{
	//			if (iPlus_Minus_Y == 0)
	//				vPos = D3DXVECTOR3(fX, 0.f, fZ);
	//			else
	//				vPos = D3DXVECTOR3(fX, 0.f, -fZ);
	//		}
	//		else
	//		{
	//			if (iPlus_Minus_Y == 0)	// 0 == +   , 1 == -
	//				vPos = D3DXVECTOR3(-fX, 0.f, fZ);
	//			else
	//				vPos = D3DXVECTOR3(-fX, 0.f, -fZ);
	//		}

	//		CNaviMgr::GetInstance()->GetNaviIndex(&vPos, dwIdxCheck);
	//		if (dwIdxCheck != UNKNOWN_VALUE)
	//		{
	//			bPosCheck = true;
	//		}
	//	}
	//	CObj* pObj = new CPlayer(vPos, iter->first);
	//	m_PlayerList.insert(make_pair(iter->first, pObj));
	//}

	//수정필요 -> 나중에 삭제
	//임시 확인용
	
	for (iter; iter != iter_end; ++iter)
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(50.f, 0.f, 20.f + iter->first * 10.f);

		CObj* pObj = new CPlayer(vPos, iter->first);
		m_PlayerList.insert(make_pair(iter->first, pObj));
	}
}
void CObjMgr::SetBotPos(void)
{
	int iBotCount = CGameMgr::GetInstance()->GetBotCount();
	for (int i = 0; i < iBotCount; ++i)
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		DWORD dwIdxCheck = UNKNOWN_VALUE;
		bool bPosCheck = false;

		while (!bPosCheck)
		{
			int iPlus_Minus_X = rand() % 2;
			int iPlus_Minus_Y = rand() % 2;
			float fX = (float)(rand() % 78);
			float fZ = (float)(rand() % 38);
			if (iPlus_Minus_X == 0)		// 0 == +   ,   1 == -
			{
				if (iPlus_Minus_Y == 0)
					vPos = D3DXVECTOR3(fX, 0.f, fZ);
				else
					vPos = D3DXVECTOR3(fX, 0.f, -fZ);
			}
			else
			{
				if (iPlus_Minus_Y == 0)	// 0 == +   , 1 == -
					vPos = D3DXVECTOR3(-fX, 0.f, fZ);
				else
					vPos = D3DXVECTOR3(-fX, 0.f, -fZ);
			}

			CNaviMgr::GetInstance()->GetNaviIndex(&vPos, dwIdxCheck);
			if (dwIdxCheck != UNKNOWN_VALUE)
			{
				bPosCheck = true;
			}
		}
		CObj* pObj = new CBot(vPos, i);
		m_BotList.insert(make_pair(i, pObj));
	}
}
void CObjMgr::SetBeadPos(void)
{
	for (int i = 0; i < BEAD_END; ++i)
	{
		bool bCheckOverlap = false;
		D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 0.f, 0.f);

		while (!bCheckOverlap)
		{
			int iRand = rand() % 8;
			bool bCheckSameValue = false;

			vPos = m_vReserveBeadPos[iRand];

			for (size_t i = 0; i < m_BeadList.size(); ++i)
			{
				if (m_BeadList[i]->GetPos() == vPos)
				{
					bCheckSameValue = true;
				}
			}
			if (!bCheckSameValue)
			{
				bCheckOverlap = true;
			}
		}

		CObj* pObj = new CBead(vPos, i);
		m_BeadList.insert(make_pair(i, pObj));
	}
}

bool CObjMgr::Initialize(void)
{
	ReserveBeadPos();

	return true;
}
bool CObjMgr::Progress(float _fFrameTime)
{
	//player list progress
	unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
	unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
	for (player_iter; player_iter != player_iter_end; ++player_iter)
	{
		player_iter->second->Progress(_fFrameTime);
	}

	//bot list progress
	unordered_map<int, CObj*>::iterator bot_iter = m_BotList.begin();
	unordered_map<int, CObj*>::iterator bot_iter_end = m_BotList.end();
	for (bot_iter; bot_iter != bot_iter_end; ++bot_iter)
	{
		bot_iter->second->Progress(_fFrameTime);
	}

	//bead list progress
	unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
	unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
	for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
	{
		bead_iter->second->Progress(_fFrameTime);
	}

	return true;
}
void CObjMgr::SetObjectsPos(void)
{
	SetPlayerPos();
	SetBotPos();
	SetBeadPos();

	cout << "Player size: " << m_PlayerList.size() << " / Bot: " << m_BotList.size() << " / Bead: " << m_BeadList.size() << endl;
}
void CObjMgr::Clear(void)
{
	//player list clear
	unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
	unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
	for (player_iter; player_iter != player_iter_end; ++player_iter)
	{
		m_ObjMgrMutex.lock();
		delete player_iter->second;
		player_iter->second = nullptr;
		m_ObjMgrMutex.unlock();
	}
	m_PlayerList.clear();

	//bot list clear
	unordered_map<int, CObj*>::iterator bot_iter = m_BotList.begin();
	unordered_map<int, CObj*>::iterator bot_iter_end = m_BotList.end();
	for (bot_iter; bot_iter != bot_iter_end; ++bot_iter)
	{
		m_ObjMgrMutex.lock();
		delete bot_iter->second;
		bot_iter->second = nullptr;
		m_ObjMgrMutex.unlock();
	}
	m_BotList.clear();

	//bead list clear
	unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
	unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
	for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
	{
		m_ObjMgrMutex.lock();
		delete bead_iter->second;
		bead_iter->second = nullptr;
		m_ObjMgrMutex.unlock();
	}
	m_BeadList.clear();

	//player data reset
	ZeroMemory(m_pHavingBeadPerPlayer, sizeof(int) * MAX_USER);
}
bool CObjMgr::CheckBeadHaveOwner(void)
{
	unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
	unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
	for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
	{
		int iOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
		if (iOwnerID == UNKNOWN_VALUE)
		{
			return false;
		}
	}
	return true;
}
int CObjMgr::CheckHavingBeadPerPlayerAndReturnMaxCount(void)
{
	int iMaxCount = 0;

	//check having player bead's count
	unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
	unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
	for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
	{
		int iOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
		if (iOwnerID != UNKNOWN_VALUE)
		{
			m_ObjMgrMutex.lock();
			m_pHavingBeadPerPlayer[iOwnerID] += 1;
			m_ObjMgrMutex.unlock();
		}
	}

	//check maxCount
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_pHavingBeadPerPlayer[i] > iMaxCount)
		{
			iMaxCount = m_pHavingBeadPerPlayer[i];
		}
	}

	return iMaxCount;
}