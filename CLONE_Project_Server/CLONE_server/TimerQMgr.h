#pragma once

#include "GameTimer.h"

class CTimerQMgr
{
	DECLARE_SINGLETON(CTimerQMgr)

private:
	CGameTimer			m_cTimerQTimer;
	CRITICAL_SECTION	m_csTimer;
	priority_queue<EVENTTYPE, vector<EVENTTYPE>, CMyComparison> m_timerQ;

public:
	inline CGameTimer* GetTimerQTimer(void)
	{
		return &m_cTimerQTimer;
	}
	inline CRITICAL_SECTION* GetCsTimer(void)
	{
		return &m_csTimer;
	}
	inline priority_queue<EVENTTYPE, vector<EVENTTYPE>, CMyComparison>* GetTimerQ(void)
	{
		return &m_timerQ;
	}

public:
	void AddTimerQ(int _iDoEvent, float _fWakeUpTime, int _iID = NO_TARGET, int _iTarget = NO_TARGET);
	void ProgressTImerQ(EVENTTYPE* _pEventtype);
	void ClearTimerQ(void);

private:
	CTimerQMgr();
	~CTimerQMgr();
};