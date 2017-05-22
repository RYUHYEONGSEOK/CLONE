#pragma once

class CClientMgr
{
	DECLARE_SINGLETON(CClientMgr)

	//client manage
private:
	unordered_map<int, CLIENT*> m_ClientList;
	bool m_bIsUseID[MAX_USER];	//using ID check

private:
	CRITICAL_SECTION m_ClientMgrCS;

public:
	inline unordered_map<int, CLIENT*>* GetClientList(void)
	{
		return &m_ClientList;
	}
	inline bool GetIsUseID(int _iID)
	{
		return m_bIsUseID[_iID];
	}
	inline void SetIsUseID(int _iID, bool _bIsUse = false)
	{
		EnterCriticalSection(&m_ClientMgrCS);
		m_bIsUseID[_iID] = _bIsUse;
		LeaveCriticalSection(&m_ClientMgrCS);
	}

public:
	bool	Initialize(void);
	int		GetClientID(void);
	bool	InsertClient(int _iID, CLIENT* _pClient);
	bool	EraseClient(int _iID);
	CLIENT*	FindClient(int _iID);
private:
	void	Clear(void);
public:
	void	ResetAllPlayerReady(void);
	bool	CheckReadyByHost(void);

	//send packet, process packet
public:
	bool SendPacket(int _iID, void* _pPacket);
	bool ProcessPacket(char* _pPacket, int _iID);

private:
	CClientMgr();
	~CClientMgr();
};