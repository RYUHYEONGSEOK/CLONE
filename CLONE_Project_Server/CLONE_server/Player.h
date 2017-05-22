#pragma once

#include "Obj.h"

class CPlayer :
	public CObj
{

public:
	virtual int Progress(float _fFrameTime);



//using bead
private:
	bool bIsGetBead[BEAD_END];
public:
	void SetPlayerGetBead(int _iBeadType, bool _bIsBeadSet);
	bool GetPlayerGetBead(int _iBeadType);
	void ClearGetBead(void);




//using key
private:
	DWORD	m_dwNowKey;
public:
	void	SetKey(DWORD _dwKey) {m_dwNowKey = _dwKey;}
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
public:
	void KeyCheck(float _fFrameTime);
	void StateCheck(float _fFrameTime);
	void SetAnimationEnd(bool _bIsEndAni){m_bIsEndAnimation = _bIsEndAni;}
	bool AnimationEnd(void){return m_bIsEndAnimation;}



public:
	CPlayer();
	CPlayer(D3DXVECTOR3 _pos, int _iID);
	virtual ~CPlayer();
};