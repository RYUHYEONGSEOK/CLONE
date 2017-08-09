#include "stdafx.h"
#include "ReservationThread.h"

#include "TimerQMgr.h"

DWORD WINAPI ReservationThread(void)
{
	priority_queue<EVENTTYPE, vector<EVENTTYPE>, CMyComparison>* pTimerQ = CTimerQMgr::GetInstance()->GetTimerQ();
	mutex* pMutexTimer = CTimerQMgr::GetInstance()->GetMutexTimer();

	high_resolution_clock::time_point ptNowTime = high_resolution_clock::now();

	while (true)
	{
		pMutexTimer->lock();
		if (pTimerQ->size() == 0)
		{
			pMutexTimer->unlock();
			continue;
		}

		EVENTTYPE tTopEvent = pTimerQ->top();
		if (tTopEvent.m_ptExecTime > high_resolution_clock::now())
		{
			pMutexTimer->unlock();
			continue;
		}

		pTimerQ->pop();
		pMutexTimer->unlock();

		CTimerQMgr::GetInstance()->ProgressTImerQ(&tTopEvent);
	}

	pMutexTimer = nullptr;
	pTimerQ = nullptr;
	return 0;
}