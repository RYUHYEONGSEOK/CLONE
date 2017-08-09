#include "StdAfx.h"
#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr(void)
{
	m_pSystem = NULL;
	m_iVersion = 0;

	Initialize();
}

CSoundMgr::~CSoundMgr(void)
{
}


void CSoundMgr::Initialize(void)
{
	m_Result = FMOD_System_Create(&m_pSystem);
	ErrorCheck(m_Result);

	m_Result = FMOD_System_GetVersion(m_pSystem, &m_iVersion);
	ErrorCheck(m_Result);

	m_Result = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	ErrorCheck(m_Result);
}

void CSoundMgr::ErrorCheck(FMOD_RESULT _result)
{
	if(_result != FMOD_OK)
	{
		cout << "오류" << endl;
		return;
	}
}


void CSoundMgr::LoadSoundFile(void)
{
	_finddata_t fd;

	long handle;
	int iResult = 1;

	handle = _findfirst("../bin/Resources/Sound/*.*", &fd);		//해당경로 모두 읽어라.

	if (handle == -1)
	{
		return;
	}

	while(iResult != -1)
	{
		TCHAR szName[256];

		ZeroMemory(szName, sizeof(szName));	//메크로 함수.

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
							fd.name, strlen(fd.name), szName, 256);
		//멀티바이트를 유니코드로 변경.

		TCHAR* pName = new TCHAR[256];
		ZeroMemory(pName, sizeof(TCHAR) * 256);
		lstrcpy(pName, szName);

		char szFullName[256];

		strcpy_s(szFullName, "../bin/Resources/Sound/");
		strcat_s(szFullName, fd.name);


		FMOD_SOUND* pSound;

		m_Result = FMOD_System_CreateSound(m_pSystem, szFullName, FMOD_HARDWARE, 0, &pSound);

		if(m_Result == FMOD_OK)
		{
			m_mapSound.insert( make_pair(pName, pSound) );
		}
		else
		{
			delete [] pName;
		}

		iResult = _findnext(handle, &fd);		//모든탐색을 한다.

	}

	_findclose(handle);

	m_Result = FMOD_System_Update(m_pSystem);

	ErrorCheck(m_Result);

}

void CSoundMgr::PlaySound(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE,
		iter->second, 0, &m_pEffect);

	ErrorCheck(m_Result);
}

void CSoundMgr::PlayBGM(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE,
		iter->second, 0, &m_pBGM);

	ErrorCheck(m_Result);
}

void CSoundMgr::PlayBGM2(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE,
		iter->second, 0, &m_pBGM2);

	ErrorCheck(m_Result);
}

void CSoundMgr::PlaySkillSound(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE,
		iter->second, 0, &m_pSkill);

	ErrorCheck(m_Result);
}

void CSoundMgr::PlayPlayerSound(TCHAR * pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCmp(pSoundKey));

	if (iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE,
		iter->second, 0, &m_pPlayerSound);

	ErrorCheck(m_Result);
}

void CSoundMgr::PlayUISound(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE,
		iter->second, 0, &m_pUISound);

	ErrorCheck(m_Result);
}

void CSoundMgr::PlayEtcSound(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE,
		iter->second, 0, &m_pEtcSound);

	ErrorCheck(m_Result);
}


void CSoundMgr::StopSoundAll(void)
{
	m_Result  = FMOD_Channel_Stop(m_pBGM);
	ErrorCheck(m_Result);
	m_Result  = FMOD_Channel_Stop(m_pEffect);
	ErrorCheck(m_Result);
	m_Result  = FMOD_Channel_Stop(m_pSkill);
	ErrorCheck(m_Result);
	m_Result  = FMOD_Channel_Stop(m_pBGM2);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pUISound);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pEtcSound);
	ErrorCheck(m_Result);
}

void CSoundMgr::StopBGMSound(void)
{
	m_Result  = FMOD_Channel_Stop(m_pBGM);
}