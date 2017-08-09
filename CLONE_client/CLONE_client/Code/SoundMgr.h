#pragma once

class CSoundMgr
{
	DECLARE_SINGLETON(CSoundMgr)
	//Fmod �������̽�
private:
	FMOD_SYSTEM*	m_pSystem;			//fmod �ý��� ������
	FMOD_CHANNEL*	m_pEffect;	
	FMOD_CHANNEL*	m_pBGM;
	FMOD_CHANNEL*	m_pBGM2;
	FMOD_CHANNEL*	m_pSkill;
	FMOD_CHANNEL*	m_pUISound;
	FMOD_CHANNEL*	m_pEtcSound;
	FMOD_CHANNEL*	m_pPlayerSound;

	FMOD_RESULT		m_Result;		//�����ߵǴ��� Ȯ���ϴ� ����.

	unsigned int m_iVersion;
	
	map<TCHAR*, FMOD_SOUND*>		m_mapSound;		//���� ����(map)

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
