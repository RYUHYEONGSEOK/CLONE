#pragma once

class CMainServer
{
	DECLARE_SINGLETON(CMainServer)

private:
	thread*			m_pAcceptThread;
	vector<thread*> m_WorkThreadVector;
	thread*			m_pReservationThread;
	thread*			m_pLogicThread;

private:
	SYSTEM_INFO m_tSystemInfo;
	int			m_iWorkThreadCount;

private:
	bool MakeThread(void);
	bool ClearThread(void);
	bool NaviLoad(void);
	bool NaviDelete(void);

public:
	bool Initialize(void);
	bool Progress(void);
	bool Release(void);

private:
	CMainServer();
	~CMainServer();
};