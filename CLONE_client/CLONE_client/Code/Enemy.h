#pragma once

#include "Obj.h"

class CEnemy
	: public CObj
{
private:
	float			m_fInitTime;
	bool			m_bCreate;
	bool			m_bMoveEnd;		// Move가 끝났다는 Flag (이 Flag 가 TRUE 가 되면 서버로 송신)

private:
	bool			m_bCheckAniEnd;
	int				m_iSaveAniCount;
	int				m_iCurAniCount;
	int				m_iAniCount;
	float			m_fRadious;

private:
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
	static CEnemy* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CEnemy(CDevice* pDevice);
public:
	virtual ~CEnemy();
};