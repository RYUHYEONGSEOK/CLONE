#pragma once

#include "Obj.h"

class CBead
	: public CObj
{
private:
	float			m_fInitTime;	//Collision Check 오류 방지용
	BEADTYPE		m_eBeadType;
	int				m_iOwnerID;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

public:
	HRESULT	AddComponent(void);

public:
	void SetBeadType(int iType);
	BEADTYPE GetBeadType(void) {return m_eBeadType;};
	void SetOwnerID(int _iID) {m_iOwnerID = _iID;}
	int GetOwnerID(void) { return m_iOwnerID; };

public:
	static CBead* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CBead(CDevice* pDevice);
public:
	virtual ~CBead();
};