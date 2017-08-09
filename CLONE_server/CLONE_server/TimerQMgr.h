#pragma once

class CTimerQMgr
{
	DECLARE_SINGLETON(CTimerQMgr)

private:
	mutex	m_mutexTimer;
	priority_queue<EVENTTYPE, vector<EVENTTYPE>, CMyComparison> m_timerQ;

public:
	inline mutex* GetMutexTimer(void)
	{
		return &m_mutexTimer;
	}
	inline priority_queue<EVENTTYPE, vector<EVENTTYPE>, CMyComparison>* GetTimerQ(void)
	{
		return &m_timerQ;
	}

public:
	void AddTimerQ(int _iDoEvent, high_resolution_clock::time_point _ptWakeUpTime, int _iID = NO_TARGET, int _iTarget = NO_TARGET);
	void ProgressTImerQ(EVENTTYPE* _pEventtype);
	void ClearTimerQ(void);

private:
	CTimerQMgr();
	~CTimerQMgr();
};