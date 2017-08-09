#include "stdafx.h"
#include "ObjMgr.h"

#include "Player.h"
#include "Bot.h"
#include "Bead.h"

#include "NaviMgr.h"
#include "GameMgr.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
	//client isconnect
	for (int i = 0; i < MAX_USER; ++i)
	{
		m_bIsUseID[i] = false;
	}

	ZeroMemory(m_vReserveBeadPos, sizeof(D3DXVECTOR3) * BEAD_COUNT);
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
void CObjMgr::SetPlayerPos(eMapType _eMapType)
{
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator iter = m_PlayerList.begin();
	unordered_map<int, CObj*>::iterator iter_end = m_PlayerList.end();
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

	//		CNaviMgr::GetInstance()->GetNaviIndex(&vPos, dwIdxCheck, _eMapType);
	//		if (dwIdxCheck != UNKNOWN_VALUE)
	//		{
	//			bPosCheck = true;
	//		}
	//	}

	//	m_ObjMgrMutex.lock();
	//	iter->second->SetPos(vPos);
	//	iter->second->SetMapType(_eMapType);
	//	m_ObjMgrMutex.unlock();
	//}

	//수정필요 -> 나중에 삭제
	//임시 확인용
	switch (_eMapType)
	{
	case MAP_KJK_PLAZA:
	{
		for (iter; iter != iter_end; ++iter)
		{
			D3DXVECTOR3 vPos = D3DXVECTOR3(50.f, 0.f, 20.f + iter->first * 10.f);

			m_ObjMgrMutex.lock();
			iter->second->SetObjState(OBJ_STATE_IDEL);
			iter->second->SetPos(vPos);
			iter->second->SetMapType(_eMapType);
			m_ObjMgrMutex.unlock();
		}
		break;
	}
	case MAP_KJK_POOL:
	{
		for (iter; iter != iter_end; ++iter)
		{
			D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 0.f, iter->first * 10.f);

			m_ObjMgrMutex.lock();
			iter->second->SetObjState(OBJ_STATE_IDEL);
			iter->second->SetPos(vPos);
			iter->second->SetMapType(_eMapType);
			m_ObjMgrMutex.unlock();
		}
		break;
	}
	}
}
void CObjMgr::SetBotPos(eMapType _eMapType)
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

			CNaviMgr::GetInstance()->GetNaviIndex(&vPos, dwIdxCheck, _eMapType);
			if (dwIdxCheck != UNKNOWN_VALUE)
			{
				bPosCheck = true;
			}
		}

		m_ObjMgrMutex.lock();
		CObj* pObj = new CBot(vPos, i);
		pObj->SetMapType(_eMapType);
		m_BotList.insert(make_pair(i, pObj));
		m_ObjMgrMutex.unlock();
	}
}
void CObjMgr::SetBeadPos(eMapType _eMapType)
{
	if (_eMapType == MAP_KJK_POOL) return;

	for (int i = 0; i < BEAD_END; ++i)
	{
		bool bCheckOverlap = false;
		D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 0.f, 0.f);

		while (!bCheckOverlap)
		{
			int iRand = rand() % BEAD_COUNT;
			bool bCheckSameValue = false;

			vPos = m_vReserveBeadPos[iRand];

			for (size_t i = 0; i < m_BeadList.size(); ++i)
			{
				if (m_BeadList[(int)i]->GetPos() == vPos)
				{
					bCheckSameValue = true;
				}
			}
			if (!bCheckSameValue)
			{
				bCheckOverlap = true;
			}
		}

		m_ObjMgrMutex.lock();
		CObj* pObj = new CBead(vPos, i);
		pObj->SetMapType(_eMapType);
		m_BeadList.insert(make_pair(i, pObj));
		m_ObjMgrMutex.unlock();
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
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
	unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
	m_ObjMgrMutex.unlock();
	for (player_iter; player_iter != player_iter_end; ++player_iter)
	{
		player_iter->second->Progress(_fFrameTime);
	}

	//bot list progress
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator bot_iter = m_BotList.begin();
	unordered_map<int, CObj*>::iterator bot_iter_end = m_BotList.end();
	m_ObjMgrMutex.unlock();
	for (bot_iter; bot_iter != bot_iter_end; ++bot_iter)
	{
		bot_iter->second->Progress(_fFrameTime);
	}

	//bead list progress
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
	unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
	m_ObjMgrMutex.unlock();
	for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
	{
		bead_iter->second->Progress(_fFrameTime);
	}

	return true;
}
int CObjMgr::GetClientID(void)
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
			m_ObjMgrMutex.lock();
			m_bIsUseID[i] = true;
			m_ObjMgrMutex.unlock();
			return iClientID;
		}
	}
	return UNKNOWN_VALUE;
}
bool CObjMgr::CheckPlayerConnect(int _iID)
{
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator iter = m_PlayerList.find(_iID);
	if (iter == m_PlayerList.end())
	{
		cout << "Don't Exist Client ID!" << endl;
		m_ObjMgrMutex.unlock();
		return false;
	}
	if (iter->second == nullptr)
	{
		cout << "Don't Exist Client OBJ!" << endl;
		m_ObjMgrMutex.unlock();
		return false;
	}
	if (reinterpret_cast<CPlayer*>(iter->second)->GetSocket() == NULL)
	{
		cout << "Don't Exist Client SOCKET!" << endl;
		m_ObjMgrMutex.unlock();
		return false;
	}
	m_ObjMgrMutex.unlock();
	return true;
}
bool CObjMgr::ConnectPlayer(int _iID, CObj* _pPlayer)
{
	m_ObjMgrMutex.lock();
	m_PlayerList.insert(make_pair(_iID, _pPlayer));
	m_ObjMgrMutex.unlock();

	return true;
}
bool CObjMgr::DisconnectPlayer(int _iID)
{
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator iter = m_PlayerList.find(_iID);
	if (iter == m_PlayerList.end())
	{
		cout << "Don't Erase Client!" << endl;
		m_ObjMgrMutex.unlock();
		return false;
	}

	m_bIsUseID[_iID] = false;

	SAFE_DELETE(iter->second);
	iter = m_PlayerList.erase(iter);
	m_ObjMgrMutex.unlock();

	cout << "Disconnected ID : " << _iID << endl;

	return true;
}
void CObjMgr::ResetAllPlayerReady(void)
{
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
	unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
	for (player_iter; player_iter != player_iter_end; ++player_iter)
	{
		reinterpret_cast<CPlayer*>(player_iter->second)->SetIsReady(false);
	}
	m_ObjMgrMutex.unlock();
}
bool CObjMgr::CheckReadyByHost(void)
{
	m_ObjMgrMutex.lock();
	unordered_map<int, CObj*>::iterator iter = m_PlayerList.begin();
	unordered_map<int, CObj*>::iterator iter_end = m_PlayerList.end();
	for (iter; iter != iter_end; ++iter)
	{
		if (!reinterpret_cast<CPlayer*>(iter->second)->GetIsReady())
		{
			if (iter->first == CGameMgr::GetInstance()->GetHostID()) continue;
			m_ObjMgrMutex.unlock();
			return false;
		}
	}
	m_ObjMgrMutex.unlock();
	return true;
}

void CObjMgr::SetObjectsPos(eMapType _eMapType)
{
	switch (_eMapType)
	{
	case MAP_KJK_PLAZA:
		SetPlayerPos(_eMapType);
		SetBotPos(_eMapType);
		SetBeadPos(_eMapType);
		break;
	case MAP_KJK_POOL:
		SetPlayerPos(_eMapType);
		SetBotPos(_eMapType);
		break;
	default:
		cout << "CObjMgr SetObjectsPos error" << endl;
		break;
	}

	cout << "---------------------------------------------" << endl;
	cout << "Player size: " << m_PlayerList.size() << " / Bot: " << m_BotList.size() << " / Bead: " << m_BeadList.size() << endl;
	cout << "---------------------------------------------" << endl;
}
void CObjMgr::ClearObject(void)
{
	//player setting clear
	unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
	unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
	for (player_iter; player_iter != player_iter_end; ++player_iter)
	{
		m_ObjMgrMutex.lock();
		reinterpret_cast<CPlayer*>(player_iter->second)->SetNaviIndex(0);
		player_iter->second->SetMapType(MAP_END);
		player_iter->second->SetObjState(OBJ_STATE_IDEL);
		m_ObjMgrMutex.unlock();
	}

	//bot list clear
	unordered_map<int, CObj*>::iterator bot_iter = m_BotList.begin();
	unordered_map<int, CObj*>::iterator bot_iter_end = m_BotList.end();
	for (bot_iter; bot_iter != bot_iter_end; ++bot_iter)
	{
		m_ObjMgrMutex.lock();
		SAFE_DELETE(bot_iter->second);
		m_ObjMgrMutex.unlock();
	}
	m_BotList.clear();

	//bead list clear
	unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
	unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
	for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
	{
		m_ObjMgrMutex.lock();
		SAFE_DELETE(bead_iter->second);
		m_ObjMgrMutex.unlock();
	}
	m_BeadList.clear();

	//player data reset
	ZeroMemory(m_pHavingBeadPerPlayer, sizeof(int) * MAX_USER);
}
bool CObjMgr::CheckAllBeadHaveOwner(void)
{
	unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
	unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
	for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
	{
		int iOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
		if (iOwnerID == UNKNOWN_VALUE) return false;
	}
	return true;
}
bool CObjMgr::CheckPlayerState(eGameModeType _eGameModeType)
{
	switch (_eGameModeType)
	{
	case MODE_SOLO:
	{
		int iAlivedPlayerCount = 0;

		unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
		unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
		for (player_iter; player_iter != player_iter_end; ++player_iter)
		{
			if (player_iter->second->GetObjState() == OBJ_STATE_DIE) continue;
			else ++iAlivedPlayerCount;
		}

		if (iAlivedPlayerCount < 2) return true;
		else return false;

		break;
	}
	case MODE_TEAM:
	{
		int iAlivedRedPlayerCount = 0;
		int iAlivedBluePlayerCount = 0;

		unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
		unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
		for (player_iter; player_iter != player_iter_end; ++player_iter)
		{
			//red
			if (0 == reinterpret_cast<CPlayer*>(player_iter->second)->GetRoomIndex() % 2)
			{
				if (player_iter->second->GetObjState() == OBJ_STATE_DIE) continue;
				else ++iAlivedRedPlayerCount;
			}
			//blue
			else
			{
				if (player_iter->second->GetObjState() == OBJ_STATE_DIE) continue;
				else ++iAlivedBluePlayerCount;
			}
		}

		if (iAlivedRedPlayerCount == 0 || iAlivedBluePlayerCount == 0) return true;
		else return false;

		break;
	}
	case MODE_END:
	default:
		cout << "CObjMgr CheckPlayerState type error" << endl;
		break;
	}

	return false;
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
int CObjMgr::GetTeamAlivedPlayerCount(ePlayerType _ePlayerType)
{
	int iCount = 0;
	switch (_ePlayerType)
	{
	case PLAYER_RED:
	{
		unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
		unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
		for (player_iter; player_iter != player_iter_end; ++player_iter)
		{
			if (player_iter->second->GetObjState() == OBJ_STATE_DIE) continue;
			else
			{
				int iRoomIndex = reinterpret_cast<CPlayer*>(player_iter->second)->GetRoomIndex();
				if (iRoomIndex % 2 == 0) ++iCount;
			}
		}
		break;
	}
	case PLAYER_BLUE:
	{
		unordered_map<int, CObj*>::iterator player_iter = m_PlayerList.begin();
		unordered_map<int, CObj*>::iterator player_iter_end = m_PlayerList.end();
		for (player_iter; player_iter != player_iter_end; ++player_iter)
		{
			if (player_iter->second->GetObjState() == OBJ_STATE_DIE) continue;
			else
			{
				int iRoomIndex = reinterpret_cast<CPlayer*>(player_iter->second)->GetRoomIndex();
				if (iRoomIndex % 2 == 1) ++iCount;
			}
		}
		break;
	}
	default: return UNKNOWN_VALUE;
	}

	return iCount;
}
int CObjMgr::GetTeamBeadCount(ePlayerType _ePlayerType)
{
	int iCount = 0;
	switch (_ePlayerType)
	{
	case PLAYER_RED:
	{
		unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
		unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
		for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
		{
			int iOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
			if (iOwnerID != UNKNOWN_VALUE)
			{
				int iRoomIndex = reinterpret_cast<CPlayer*>(m_PlayerList[iOwnerID])->GetRoomIndex();
				if (iRoomIndex % 2 == 0) ++iCount;
			}
		}
		break;
	}
	case PLAYER_BLUE:
	{
		unordered_map<int, CObj*>::iterator bead_iter = m_BeadList.begin();
		unordered_map<int, CObj*>::iterator bead_iter_end = m_BeadList.end();
		for (bead_iter; bead_iter != bead_iter_end; ++bead_iter)
		{
			int iOwnerID = reinterpret_cast<CBead*>(bead_iter->second)->GetOwnerID();
			if (iOwnerID != UNKNOWN_VALUE)
			{
				int iRoomIndex = reinterpret_cast<CPlayer*>(m_PlayerList[iOwnerID])->GetRoomIndex();
				if (iRoomIndex % 2 == 1) ++iCount;
			}
		}
		break;
	}
	default: return UNKNOWN_VALUE;
	}

	return iCount;
}