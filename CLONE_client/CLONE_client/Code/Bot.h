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
	OBJSTATE		m_eCurState;	//������ ���� ���� ����
	float			m_fInitTime;
	bool			m_bCreate;
	bool			m_bRotateEnd;	// Move�� ������ ���� �޾��� �� Bot�� ���ڸ� ȸ���ϴ� Flag
	bool			m_bInitState;	// ó������ State�� �޾Ҵ����� Flag
	bool			m_bMoveEnd;		// Move�� �����ٴ� Flag (�� Flag �� TRUE �� �Ǹ� ������ �۽�)

private:
	bool			m_bCheckAniEnd;
	int				m_iSaveAniCount;
	int				m_iCurAniCount;
	int				m_iAniCount;
	float			m_fRadious;

private:
	DWORD			m_dwIdx;	//Move On Navi üũ�� ����
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
	void	StateSetting(OBJSTATE eState, int iCount, float fRadious); //�� �Լ��� ������ ����

public:
	static CBot* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CBot(CDevice* pDevice);
public:
	virtual ~CBot();
};