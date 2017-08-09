#pragma once

class CSoundMgr
{
	DECLARE_SINGLETON(CSoundMgr)
	//Fmod 인터페이스
private:
	FMOD_SYSTEM*	m_pSystem;			//fmod 시스템 포인터
	FMOD_CHANNEL*	m_pEffect;	
	FMOD_CHANNEL*	m_pBGM;
	FMOD_CHANNEL*	m_pBGM2;
	FMOD_CHANNEL*	m_pSkill;
	FMOD_CHANNEL*	m_pUISound;
	FMOD_CHANNEL*	m_pEtcSound;
	FMOD_CHANNEL*	m_pPlayerSound;

	FMOD_RESULT		m_Result;		//실행잘되는지 확인하는 변수.

	unsigned int m_iVersion;
	
	map<TCHAR*, FMOD_SOUND*>		m_mapSound;		//사운드 관리(map)

public:
	void Initialize(void);
	void LoadSoundFile(void);
	void PlaySound(TCHAR* pSoundKey);
	void PlayBGM(TCHAR* pSoundKey);
	void PlayBGM2(TCHAR* pSoundKey);
	void PlaySkillSound(TCHAR* pSoundKey);
	void PlayPlayerSound(TCHAR* pSoundKey);
	void PlayUISound(TCHAR* pSoundKey);
	void PlayEtcSound(TCHAR* pSoundKey);
	void StopSoundAll(void);
	void StopBGMSound(void);

public:
	map<TCHAR*, FMOD_SOUND*>* GetSoundMap(void)
	{
		return &m_mapSound;
	}

private:
	void ErrorCheck(FMOD_RESULT _result);

private:
	CSoundMgr(void);
public:
	~CSoundMgr(void);
};
