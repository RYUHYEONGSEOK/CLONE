#pragma once

#include <process.h>
#include <windef.h>

class CDevice;
class CLoading
{
public:
	enum LOADID { LOADID_STAGE, LOADID_STAGE1, LOADID_STAGE2 };

public:
	const TCHAR* GetLoadingMessage(void);
	bool GetComplete(void);
	float GetLoadingPercent(void);

public:
	HRESULT InitLoading(void);
	void NeviLoading(void);
	void StageLoading1(void);
	void StageLoading2(void);
	void StageLoading3(void);

public:
	static CLoading* Create(LOADID eLoadID);
	static UINT WINAPI LoadingFunction(void* pArg);

private:
	void Release(void);

private:
	CDevice*				m_pDevice;
	LOADID					m_eLoadID;
	CRITICAL_SECTION		m_CSKey;
	HANDLE					m_hThread;

private:
	TCHAR					m_szLoadingMessage[128];
	bool					m_bComplete;
	float					m_bCurrentLoadingCnt;
	float					m_bLoadingPercent;

private:
	CLoading(LOADID eLoadID);
public:
	~CLoading(void);
};