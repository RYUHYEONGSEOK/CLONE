#pragma once

#include "Protocol.h"

class CGameMgr
{
	DECLARE_SINGLETON(CGameMgr)

private:
	SC_AllUnready	m_tUnReadyPacket;
	SC_InitBot		m_tInitBotPacket;
	SC_InitBead		m_tInitBeadPacket;
	SC_InitPlayer	m_tInitPlayerPacket;
	SC_GameState	m_tGameStatePacket;

private:
	int				m_iHostID;
	int				m_iRoomIndexID[MAX_USER];	//check using room index

private:
	eMapType		m_eMapType;
	int				m_iBotCount;
	eGameType		m_eGameType;
	eGameModeType	m_eGameModeType;
	eStageType		m_eStageType;

private:
	mutex m_mutexGameMgr;

public:
	bool Initialize(void);
	bool Release(void);

public:
	bool SetHostID(int _iID);
	bool SetMapType(void);
	bool SetBotCount(bool _bUpAndDown);
	bool SetGameModeType(eGameModeType _eGameModeType);
	bool SetGameType(eGameType _eGameType);
	void SetStageStateType(eStageType _eStageType);

public:
	void DisconnectRoomMember(int _iID);
	int ChangeRoomIndex(int _iID);
	int	GetEmptyRoomIndex(int _iID);
private:
	int CalculateChangeIndex(int _iIndex, int _iID);


public:
	inline int GetHostID(void) { return m_iHostID; }
	inline eMapType GetMapType(void) { return m_eMapType; }
	inline int GetBotCount(void) { return m_iBotCount; }
	inline eGameType GetGameType(void) { return m_eGameType; }
	inline eGameModeType GetGameModeType(void) { return m_eGameModeType; }
	inline eStageType GetStageStateType(void) { return m_eStageType; }

private:
	CGameMgr();
	~CGameMgr();
};