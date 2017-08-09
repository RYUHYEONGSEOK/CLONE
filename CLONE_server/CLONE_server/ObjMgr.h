#pragma once

#include "Obj.h"

class CObjMgr
{
	DECLARE_SINGLETON(CObjMgr)

private:
	D3DXVECTOR3 m_vReserveBeadPos[BEAD_COUNT];

private:
	mutex m_ObjMgrMutex;

private:
	int							m_pHavingBeadPerPlayer[MAX_USER];
	bool						m_bIsUseID[MAX_USER];	//using ID check
	unordered_map<int, CObj*>	m_PlayerList;
	unordered_map<int, CObj*>	m_BotList;
	unordered_map<int, CObj*>	m_BeadList;

private:
	void ReserveBeadPos(void);
	void SetPlayerPos(eMapType _eMapType);
	void SetBotPos(eMapType _eMapType);
	void SetBeadPos(eMapType _eMapType);

public:
	inline int* GetHavingBeadPerPlayer(void){return m_pHavingBeadPerPlayer;}
	inline unordered_map<int, CObj*>* GetPlayerList(void){return &m_PlayerList;}
	inline unordered_map<int, CObj*>* GetBotList(void){return &m_BotList;}
	inline unordered_map<int, CObj*>* GetBeadList(void){return &m_BeadList;}

public:
	bool Initialize(void);
	bool Progress(float _fFrameTime);
	void ClearObject(void);

public:
	int GetClientID(void);
	bool CheckPlayerConnect(int _iID);
	bool ConnectPlayer(int _iID, CObj* _pPlayer);
	bool DisconnectPlayer(int _iID);
	void ResetAllPlayerReady(void);
	bool CheckReadyByHost(void);

public:
	void SetObjectsPos(eMapType _eMapType);
	bool CheckAllBeadHaveOwner(void);
	bool CheckPlayerState(eGameModeType _eGameModeType);
	int CheckHavingBeadPerPlayerAndReturnMaxCount(void);
	int GetTeamAlivedPlayerCount(ePlayerType _ePlayerType);
	int GetTeamBeadCount(ePlayerType _ePlayerType);


private:
	CObjMgr();
	~CObjMgr();
};