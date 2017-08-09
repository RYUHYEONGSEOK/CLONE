#pragma once

#include "AcceptModule.h"

class CMainServer
	: public CAcceptModule
{
private:
	volatile bool	m_bIsShutDownServer;
	SYSTEM_INFO		m_tSystemInfo;
	int				m_iWorkerThreadCount;
	int				m_iPortNumber;
	char			m_szIPAddress[VERY_SHORT_BUFFER];

private:
	thread*			m_pReservationThread;
	thread*			m_pLogicThread;
	vector<thread*> m_WorkerThreadVector;

private:
	bool MakeManager(void);
	bool DestroyManager(void);
	bool MakeThread(void);
	bool ClearThread(void);

private:
	bool LoadServerInfo(void);
	bool GetInfoInData(char* _pInfoStr, char* _pTitle, char* _pValue);

private:
	bool LoadNavi(void);
	bool DeleteNavi(void);

public:
	bool Initialize(void);
	bool Progress(void);
	bool Release(void);

public:
	CMainServer();
	~CMainServer();
};