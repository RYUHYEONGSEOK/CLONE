#pragma once

class CTimeMgr
{
	DECLARE_SINGLETON(CTimeMgr)

private:
	LARGE_INTEGER m_FrameTime;
	LARGE_INTEGER m_FixTime;
	LARGE_INTEGER m_LastTime;
	LARGE_INTEGER m_CpuTick;

private:
	float	m_fTime;

public:
	bool InitTime(void);
	void SetTime(void);

public:
	inline float GetTime(void) { return m_fTime; }

private:
	CTimeMgr();
	~CTimeMgr();
};