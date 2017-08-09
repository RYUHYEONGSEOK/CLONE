#include "stdafx.h"
#include "TimerQMgr.h"

IMPLEMENT_SINGLETON(CTimerQMgr)

CTimerQMgr::CTimerQMgr()
{
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
		m_mutexTimer.lock();
		m_timerQ.pop();
		m_mutexTimer.unlock();
	}
}

void CTimerQMgr::AddTimerQ(int _iDoEvent, high_resolution_clock::time_point _ptWakeUpTime, int _iID/* = NO_TARGET*/, int _iTarget /* = NO_TARGET */)
{
	EVENTTYPE tNewEventType;
	tNewEventType.m_iDoEvent = _iDoEvent;
	tNewEventType.m_iPlayerID = _iID;
	tNewEventType.m_iTarget = _iTarget;
	tNewEventType.m_ptExecTime = _ptWakeUpTime;

	m_mutexTimer.lock();
	m_timerQ.push(tNewEventType);
	m_mutexTimer.unlock();
}
void CTimerQMgr::ProgressTImerQ(EVENTTYPE* _pEventtype)
{
	switch (_pEventtype->m_iDoEvent)
	{
	case EVENT_COUNTDOWN:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_EVENT_COUNTDOWN;
		pEventOver->m_iTargetID = _pEventtype->m_iTarget;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	case EVENT_BOT_MOVE:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_EVENT_BOT_MOVE;
		pEventOver->m_iTargetID = _pEventtype->m_iTarget;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	case EVENT_SYNC_OBJ:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_EVENT_SYNC_OBJ;
		pEventOver->m_iTargetID = _pEventtype->m_iTarget;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	case EVENT_MOVE_ROOM:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_EVENT_MOVE_ROOM;
		pEventOver->m_iTargetID = _pEventtype->m_iTarget;
		PostQueuedCompletionStatus(g_hIOCP, 1, _pEventtype->m_iPlayerID, reinterpret_cast<LPOVERLAPPED>(pEventOver));
		break;
	}
	case EVENT_END:
	{
		OVERLAPPED_EX* pEventOver = new OVERLAPPED_EX;
		pEventOver->m_iOperationType = OP_END;
		pEventOver->m_iTargetID = _pEventtype->m_iTarget;
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
	while (!m_timerQ.empty())
	{
		m_mutexTimer.lock();
		m_timerQ.pop();
		m_mutexTimer.unlock();
	}
	cout << "TimerQ size: " << m_timerQ.size() << endl;
}