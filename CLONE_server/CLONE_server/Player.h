#pragma once

#include "Obj.h"

class CPlayer :
	public CObj
{

public:
	virtual int Progress(float _fFrameTime);
	void Release(void);

//server
private:
	mutex			m_mutexPlayer;
private:
	SOCKET			m_tSock;
	OVERLAPPED_EX	tOverLapped;
	bool			m_bIsHost;
	bool			m_bIsReady;
	int				m_iRoomIndex;
	char			m_szName[MAX_NAME];

public:
	inline SOCKET GetSocket(void) { return m_tSock; }
	inline void SetSocket(SOCKET& _tSocket) { m_tSock = _tSocket; }
	inline OVERLAPPED_EX* GetOverLapped(void) { return &tOverLapped; }
	inline void SetOverLapped(OVERLAPPED_EX _tOverlapp) { tOverLapped = _tOverlapp; }
	inline bool GetIsHost(void) { return m_bIsHost; }
	inline void SetIsHost(bool _bIsHost) { m_bIsHost = _bIsHost; }
	inline bool GetIsReady(void) { return m_bIsReady; }
	inline void SetIsReady(bool _bIsReady) { m_bIsReady = _bIsReady; }
	inline int GetRoomIndex(void) { return m_iRoomIndex; }
	inline void SetRoomIndex(int _iRoomIndex) { m_iRoomIndex = _iRoomIndex; }
	inline char* GetName(void) { return m_szName; }
	inline void SetName(char* _pName) { memcpy(m_szName, _pName, sizeof(m_szName)); }



//using key
private:
	DWORD	m_dwNowKey;
public:
	void	SetKey(DWORD _dwKey) { m_mutexPlayer.lock(); m_dwNowKey = _dwKey; m_mutexPlayer.unlock(); }
	DWORD	GetKey(void) { return m_dwNowKey; }




//using NaviMgr
private:
	D3DXMATRIX	m_matWorld;
	D3DXVECTOR3 m_vScale;
	DWORD		m_dwIdx;
	float		m_fStamina;
	float		m_fMaxStamina;
	bool		m_bIsEndAnimation;
	bool		m_bRun;		//Run Check
	bool		m_bSit;		//Sit Check
	bool		m_bAtt;		//Att Check
	bool		m_bJump;	//Jump Check
	bool		m_bKeyDown;	//Key Down Check
private:
	void SetWorldMatrix(void);
	void CheckDirKey(float _fFrameTime);
	void CheckActionKey(float _fFrameTime);
	void CheckPosKey(float _fFrameTime);
	void CheckState(float _fFrameTime);
public:
	inline float GetPlayerStamina(void) {return m_fStamina;}
	inline void SetPlayerStamina(float _fStamina) { m_fStamina = _fStamina; }
public:
	inline void SetNaviIndex(DWORD _dwIndex) { m_dwIdx = _dwIndex; }
	inline void SetAnimationEnd(bool _bIsEndAni) { m_bIsEndAnimation = _bIsEndAni; }
	inline bool AnimationEnd(void) { return m_bIsEndAnimation; }


public:
	CPlayer();
	CPlayer(int _iID, D3DXVECTOR3 _vPos = D3DXVECTOR3(0.f, 0.f, 0.f));
	virtual ~CPlayer();
};