#pragma once

//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

class CGameTimer
{
private:
	bool mStopped;

private:
	double mSecondsPerCount;
	double mDeltaTime;

private:
	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;


public:
	float TotalTime(void)const;  // in seconds
	float DeltaTime(void)const; // in seconds

public:
	void Reset(void); // Call before message loop.
	void Start(void); // Call when unpaused.
	void Stop(void);  // Call when paused.
	void Tick(void);  // Call every frame.

public:
	CGameTimer();
	~CGameTimer();
};