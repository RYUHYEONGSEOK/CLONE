#pragma once
class CGameMgr
{
	DECLARE_SINGLETON(CGameMgr)
	
private:
	int				m_iHostID;
	int				m_iRoomIndexID[MAX_USER];	//check using room index

private:
	int				m_iRoundCount;
	int				m_iBotCount;
	eGameType		m_eGameType;
	eGameModeType	m_eGameModeType;
	eStageType		m_eStageType;

private:
	CRITICAL_SECTION m_GameMgrCS;

public:
	bool Initialize(void);
	bool Release(void);

public:
	bool SetHostID(int _iID);
	bool SetRoundCount(bool _bUpAndDown, int _iValue);
	bool SetBotCount(bool _bUpAndDown, int _iValue);
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
	inline int GetHostID(void)
	{
		return m_iHostID;
	}
	inline int GetRoundCount(void)
	{
		return m_iRoundCount;
	}
	inline int GetBotCount(void)
	{
		return m_iBotCount;
	}
	inline eGameType GetGameType(void)
	{
		return m_eGameType;
	}
	inline eGameModeType GetGameModeType(void)
	{
		return m_eGameModeType;
	}
	inline eStageType GetStageStateType(void)
	{
		return m_eStageType;
	}

private:
	CGameMgr();
	~CGameMgr();
};