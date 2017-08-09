#pragma once

#include "Obj.h"

class CBot
	: public CObj
{
private:
	int	m_iID;
public:
	void SetBotID(int _iID) { m_iID = _iID; }
	int GetBotID(void) { return m_iID; }

private:
	OBJSTATE		m_eCurState;	//실제의 현재 봇의 상태
	float			m_fInitTime;
	bool			m_bCreate;
	bool			m_bRotateEnd;	// Move를 서버로 부터 받았을 때 Bot이 제자리 회전하는 Flag
	bool			m_bInitState;	// 처음으로 State를 받았는지의 Flag
	bool			m_bMoveEnd;		// Move가 끝났다는 Flag (이 Flag 가 TRUE 가 되면 서버로 송신)

private:
	bool			m_bCheckAniEnd;
	int				m_iSaveAniCount;
	int				m_iCurAniCount;
	int				m_iAniCount;
	float			m_fRadious;

private:
	DWORD			m_dwIdx;	//Move On Navi 체크용 변수
	bool			m_bCulling;
	bool			m_bFillStart;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	HRESULT	AddComponent(void);
	void	Collision_AABB(void);

	void	StateCheck(void);
	void	ActionCheck(void);
	void	PlayAnimation(void);
	void	StateSetting(OBJSTATE eState, int iCount, float fRadious); //이 함수로 움직임 제어

public:
	static CBot* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CBot(CDevice* pDevice);
public:
	virtual ~CBot();
};