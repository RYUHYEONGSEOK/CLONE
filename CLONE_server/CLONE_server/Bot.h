#pragma once

#include "Obj.h"
#include "Protocol.h"

class CBot :
	public CObj
{
private:
	SC_ChangeStateBot m_tChangeStateBot;

private:
	bool		m_bRotateEnd;
	bool		m_bInitState;
	DWORD		m_dwIdx;

private:
	eObjState	m_eCurState;
	float		m_fAccrueTime;
	int			m_iCurAniCount;
	int			m_iSaveAniCount;

public:
	virtual int Progress(float _fFrameTime);

public:
	void CheckAni(float _fFrameTime);
	void CheckState(float _fFrameTime);
	void SetState(eObjState _eState, int _iCount = 0, float _fRadious = 0.f);



public:
	CBot();
	CBot(D3DXVECTOR3 _pos, int _iID);
	virtual ~CBot();
};