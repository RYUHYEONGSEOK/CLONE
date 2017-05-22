#include "stdafx.h"
#include "TimerQMgr.h"

IMPLEMENT_SINGLETON(CTimerQMgr)

CTimerQMgr::CTimerQMgr()
{
	//TimerQ, TImerCS Initialize
	InitializeCriticalSection(&m_csTimer);
	if (m_timerQ.size() != 0)
	{
		cout << "TimerQ's Size is not 0" << endl;
		exit(1);
	}
}
CTimerQMgr::~CTimerQMgr()
{
	//TimerQ release
	while (!m_timerQ.empty())
	{
		EnterCriticalSection(&m_csTimer);
		m_timerQ.pop();
		LeaveCriticalSection(&m_csTimer);
	}

	//CsTimer Release
	DeleteCriticalSection(&m_csTimer);
}

void CTimerQMgr::AddTimerQ(int _iDoEvent, float _fWakeUpTime, int _iID/* = NO_TARGET*/, int _iTarget /* = NO_TARGET */)
{
	EVENTTYPE tNewEventType;
	tNewEventType.m_iDoEvent = _iDoEvent;
	tNewEventType.m_iPlayerID = _iID;
	tNewEventType.m_iTarget = _iTarget;
	tNewEventType.m_fTime = _fWakeUpTime + m_cTimerQTimer.TotalTime();

	EnterCriticalSection(&m_csTimer);
	m_timerQ.push(tNewEventType);
	LeaveCriticalSection(&m_csTimer);
}
void CTimerQMgr::ProgressTImerQ(EVENTTYPE* _pEventtype)
{
	switch (_pEventtype->m_iDoEvent)
	{
	case EVENT_COUNTDOWN:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_EVENT_COUNTDOWN;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	case EVENT_BOT_MOVE:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_EVENT_BOT_MOVE;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	case EVENT_MOVE_ROOM:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_EVENT_MOVE_ROOM;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	case EVENT_END:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_END;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	default:
		cout << "Unkown Event Type" << endl;
		break;
	}
}
void CTimerQMgr::ClearTimerQ(void)
{
	//TimerQ clear
	while (!m_timerQ.empty())
	{
		EnterCriticalSection(&m_csTimer);
		m_timerQ.pop();
		LeaveCriticalSection(&m_csTimer);
	}
	cout << "TimerQ size: " << m_timerQ.size() << endl;
}