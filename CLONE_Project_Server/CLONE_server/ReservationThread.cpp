#include "stdafx.h"
#include "ReservationThread.h"

#include "GameTimer.h"

#include "TimerQMgr.h"

DWORD WINAPI ReservationThread(void)
{
	priority_queue<EVENTTYPE, vector<EVENTTYPE>, CMyComparison>* pTimerQ = CTimerQMgr::GetInstance()->GetTimerQ();
	CRITICAL_SECTION* pCsTimer = CTimerQMgr::GetInstance()->GetCsTimer();
	CGameTimer* pTimer = CTimerQMgr::GetInstance()->GetTimerQTimer();

	while (true)
	{
		//timerQ timer setting
		pTimer->Tick();

		if (pTimerQ->size() == 0)
			continue;

		EVENTTYPE tTopEvent = pTimerQ->top();

		while (tTopEvent.m_fTime <= pTimer->TotalTime())
		{
			EnterCriticalSection(pCsTimer);
			pTimerQ->pop();
			LeaveCriticalSection(pCsTimer);
			
			CTimerQMgr::GetInstance()->ProgressTImerQ(&tTopEvent);
			
			if (pTimerQ->size() == 0)
				break;
			else
			{
				EnterCriticalSection(pCsTimer);
				tTopEvent = pTimerQ->top();
				LeaveCriticalSection(pCsTimer);
			}
		}
	}
	pTimer->Reset();
	pTimer = nullptr;
	pTimerQ = nullptr;
	return 0;
}