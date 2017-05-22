#pragma once

#include "Obj.h"

class CObjMgr
{
	DECLARE_SINGLETON(CObjMgr)

private:
	D3DXVECTOR3 m_vReserveBeadPos[8];

private:
	mutex						m_ObjMgrMutex;
	int							m_pHavingBeadPerPlayer[MAX_USER];
	unordered_map<int, CObj*>	m_PlayerList;
	unordered_map<int, CObj*>	m_BotList;
	unordered_map<int, CObj*>	m_BeadList;

private:
	void ReserveBeadPos(void);
	void SetPlayerPos(void);
	void SetBotPos(void);
	void SetBeadPos(void);

public:
	inline int* GetHavingBeadPerPlayer(void)
	{
		return m_pHavingBeadPerPlayer;
	}
	inline unordered_map<int, CObj*>* GetPlayerList(void)
	{
		return &m_PlayerList;
	}
	inline unordered_map<int, CObj*>* GetBotList(void)
	{
		return &m_BotList;
	}
	inline unordered_map<int, CObj*>* GetBeadList(void)
	{
		return &m_BeadList;
	}

public:
	bool Initialize(void);
	bool Progress(float _fFrameTime);
	void Clear(void);
public:
	void SetObjectsPos(void);
	bool CheckBeadHaveOwner(void);
	int CheckHavingBeadPerPlayerAndReturnMaxCount(void);


private:
	CObjMgr();
	~CObjMgr();
};